#! /usr/bin/env python

import sys
import argparse
import pprint
import json
import subprocess
import tempfile


class PythonScript(object):
    def run(self, args):
        if len(args) != 3:
            raise Exception("usage: START-POINT END-POINT VIDEO-PATH")
        
        start_point = float(args[0])
        end_point = float(args[1])
        path = args[2]
        
        data = self.ffprobe("-show_frames", path)
        keyframes = self.get_keyframes(data['frames'])

        kf_start = self.find_kf_start(start_point, keyframes)
        kf_end = self.find_kf_end(end_point, keyframes)

        print "keyframe start =", kf_start
        print "keyframe end =", kf_end
            
            
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
        return [f for f in data if f['key_frame'] == 1]
        
    def find_kf_start(self, point, data):
        for idx, val in enumerate(data):
            if float(val['pkt_dts_time']) > point:
                # Horrible bug when idx is really 0
               return data[idx - 1]['pkt_dts_time']
                    
    def find_kf_end(self, point, data):
        for idx, val in enumerate(data):
            if float(val['pkt_dts_time']) > point:
                # Horrible bug when idx is really 0
               return val['pkt_dts_time']

        
if __name__ == "__main__":
    obj = PythonScript()
    obj.run(sys.argv[1:])
        
