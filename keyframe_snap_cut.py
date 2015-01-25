#! /usr/bin/env python

import locale
import sys
import argparse
import pprint
import json
import subprocess
import tempfile
import sqlite3
import os

CACHE_DATABASE_PATH = "~/.keyframe_snap_cut.sqlite"

QRY_FILE = "CREATE TABLE IF NOT EXISTS file (id INTEGER PRIMARY KEY, path VARCHAR(4096), mtime BIGINT);"

class KeyframeSnapCut(object):
    def run(self, args):
        if len(args) != 4:
            raise Exception("usage: START-POINT END-POINT INPUT-PATH OUTPUT-PATH, all values in seconds")

        locale.setlocale(locale.LC_ALL, '')            

        self.conn = sqlite3.connect(os.path.expanduser(CACHE_DATABASE_PATH))
        self.cursor = self.conn.cursor()

        
        start_point = float(args[0])
        end_point = float(args[1])
        input_path = args[2]
        output_path = args[3]

        keyframes = self.get_keyframes(input_path)

        if not keyframes:
            raise Exception("empty list of keyframes, something's borked")

        kf_start = self.find_kf_start(start_point, keyframes)
        kf_end = self.find_kf_end(end_point, keyframes)

        print "keyframe start =", kf_start
        print "keyframe end =", kf_end

        self.transcode(input_path, output_path, kf_start, kf_end)

    def get_keyframes(self, input_path):
        self.cursor.execute(QRY_FILE)
        self.cursor.execute("CREATE TABLE IF NOT EXISTS keyframe (file_id INTEGER, time FLOAT)");

        abspath = os.path.abspath(input_path)

        try:
            u_abspath = abspath.decode(locale.getpreferredencoding())
        except UnicodeDecodeError, e:
            raise Exception("filename has an encoding error, please fix manually")

        
        # sqlite needs unicode strings
        self.cursor.execute(
            "SELECT f.id, f.mtime FROM file f WHERE f.path = ?",
            (u_abspath,)
        )
        result = self.cursor.fetchone()
        current_mtime = os.path.getmtime(u_abspath)

        if result:
            if current_mtime != result[1]:
                keyframes = self.probe_and_update_frames(abspath, result[0], current_mtime)
            else:
                print "Loading keyframes from cache..."
                self.cursor.execute(
                    """
                    SELECT k.time FROM keyframe k WHERE k.file_id = ?
                    ORDER BY k.time
                    """,
                    (result[0],)
                )
                keyframes = [k[0] for k in self.cursor.fetchall()]
        else:
            print "Not found in cache, scanning file..."
            keyframes = self.probe_and_cache_frames(abspath, current_mtime)

        return keyframes


    def probe_and_cache_frames(self, absolute_input_path, mtime):
        keyframes = self.probe_frames(absolute_input_path)
        
        print "Caching keyframes..."

        self.cursor.execute("INSERT INTO file (path, mtime) VALUES (?, ?)",
                  (absolute_input_path, mtime))
        new_file_id = self.cursor.lastrowid
        print "inserted new file with id", new_file_id

        self.populate_cache(new_file_id, keyframes)
        
        self.conn.commit()

        return keyframes

    def probe_and_update_frames(self, input_path, mtime, file_id):
        print  "Clearing old cache..."
        
        keyframes = self.probe_frames(input_path)

        self.cursor.execute("UPDATE file SET mtime = ? WHERE id = ?", (mtime, file_id))
        self.cursor.execute("DELETE FROM keyframe WHERE file_id = ?", (file_id,))
        
        self.populate_cache(file_id, keyframes)

        self.conn.commit()

        return keyframes

    def populate_cache(self, file_id, keyframes):
        print "Populating cache..."
        
        self.cursor.executemany("""
        INSERT INTO keyframe (file_id, time) VALUES (?, ?);
        """,
                [(file_id, k) for k in keyframes])        
        
    def probe_frames(self, input_path):
        print "Generating frame list..."
        data = self.ffprobe("-show_frames", input_path)
        print "Locating correct keyframes..."
        keyframes = self.convert_ffprobe_keyframes(data['frames'])
        return keyframes

        
    def ffprobe(self, format_option, path):
        return json.loads(
            subprocess.check_output(
                [
                    "ffprobe", "-v", "quiet", "-print_format",
                    "json", "-skip_frame", "nokey", format_option, path
                ]
            )
        )

    # sometimes packets won't contain pkt_dts_time for weird reasons
    def convert_ffprobe_keyframes(self, data):
        result = []
        time_key = 'pkt_dts_time'
        
        for f in data:
            if f['key_frame'] == 1 and f['media_type'] == 'video' \
              and time_key in f:
                result.append(float(f[time_key]))

        return result
        
    def find_kf_start(self, point, data):
        for idx, val in enumerate(data):
            if val > point:
               return data[max(idx - 1, 0)]
                    
    def find_kf_end(self, point, data):
        for idx, val in enumerate(data):
            if val > point:
               return val


        return data[-1]   # last keyframe


    # Need to specify -ss before file here otherwise it doesn't snap to
    # keyframes
    def transcode(self, input_file, output_file, start_time, end_time):
        duration = end_time - start_time
        cmd = ["ffmpeg", "-y", "-fflags", "+genpts", "-ss", str(start_time), "-i", input_file, "-t", str(duration), "-acodec", "copy",
              "-vcodec", "copy", output_file]
        print ' '.join(cmd)
        
        subprocess.check_call(cmd)

               
if __name__ == "__main__":
    obj = KeyframeSnapCut()
    obj.run(sys.argv[1:])
        
