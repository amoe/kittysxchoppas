#! /usr/bin/env python

import sys
import argparse
import pprint
import json
import subprocess
import tempfile


class PythonScript(object):
    def run(self, args):
        if len(args) != 4:
            raise Exception("usage: START-POINT END-POINT INPUT-PATH OUTPUT-PATH, all values in seconds")
        
        start_point = float(args[0])
        end_point = float(args[1])
        input_path = args[2]
        output_path = args[3]

        print "Generating frame list..."
        data = self.ffprobe("-show_frames", input_path)
        print "Locating correct keyframes..."
        keyframes = self.get_keyframes(data['frames'])

        kf_start = self.find_kf_start(start_point, keyframes)
        kf_end = self.find_kf_end(end_point, keyframes)

        print "keyframe start =", kf_start
        print "keyframe end =", kf_end

        self.transcode(input_path, output_path, kf_start, kf_end)
            
            
    def ffprobe(self, format_option, path):
        return json.loads(
            subprocess.check_output(
                [
                    "/usr/local/bin/ffprobe", "-v", "quiet", "-print_format",
                    "json", format_option, path
                ]
            )
        )

    def get_keyframes(self, data):
        return [f for f in data if f['key_frame'] == 1 and f['media_type'] == 'video']
        
    def find_kf_start(self, point, data):
        for idx, val in enumerate(data):
            if float(val['pkt_dts_time']) > point:
                # Horrible bug when idx is really 0
               return float(data[idx - 1]['pkt_dts_time'])
                    
    def find_kf_end(self, point, data):
        for idx, val in enumerate(data):
            if float(val['pkt_dts_time']) > point:
                # Horrible bug when idx is really 0
               return float(val['pkt_dts_time'])


    # Need to specify -ss before file here otherwise it acts as non-frame acccurate
    def transcode(self, input_file, output_file, start_time, end_time):
        duration = end_time - start_time
        cmd = ["ffmpeg", "-fflags", "+genpts", "-ss", str(start_time), "-i", input_file, "-t", str(duration), "-acodec", "copy",
              "-vcodec", "copy", output_file]
        print ' '.join(cmd)
        
        subprocess.check_call(cmd)

               
if __name__ == "__main__":
    obj = PythonScript()
    obj.run(sys.argv[1:])
        
