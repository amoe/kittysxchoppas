#! /usr/bin/env python3

import sys
import logging
from logging import debug, info, warn
import argparse
import os
import subprocess
import json
import time
import pdb

class MovieCutter(object):
    def run(self, args):
        logging.basicConfig(
            level=getattr(logging, 'DEBUG'),            
            format="%(asctime)s - %(levelname)8s - %(name)s - %(message)s"
        )

        input_file_path = args[0]
        cutpoints_path = args[1]
        output_directory_path = args[2]

        try:
            os.mkdir(output_directory_path)
        except:
            pass
            
        format_info = self.ffprobe(input_file_path)

        duration = float(format_info['format']['duration'])
        print(duration)
        cut_number = 1
        
        with open(cutpoints_path, 'rb') as f:
            for line in f:
                cutpoint_str = line.rstrip()
                print(cutpoint_str)

                seconds_val = self.str_to_seconds(cutpoint_str)
                print(seconds_val)

                start_time = seconds_val - 10
                end_time = seconds_val + 10

                if end_time > duration:
                    warn("end time would run off duration, using real end")
                    end_time = duration
                
                output_path = os.path.join(
                    output_directory_path, "output-%d.mkv" % cut_number
                )

                self.transcode(
                    input_file_path, output_path, start_time, end_time
                )

                cut_number += 1
                
    # Delegate to the frame accurate cutter...
    def transcode(self, input_file, output_file, start_time, end_time):
        cmd = [
            "/usr/local/bin/keyframe-snap-cut", str(start_time), str(end_time),
            input_file,  output_file
        ]
        print(' '.join(cmd))
        
        subprocess.check_call(cmd)
             
    def str_to_seconds(self, desc):
        t = time.strptime(desc, "%M:%S")
        seconds = (t.tm_min * 60) + t.tm_sec
        return seconds

    def ffprobe(self, path):
        return json.loads(
            subprocess.check_output(
                [
                    "ffprobe", "-v", "quiet", "-print_format", "json",
                    "-show_format", path
                ]
            )
        )
           
        

if __name__ == "__main__":
    obj = MovieCutter()
    obj.run(sys.argv[1:])
