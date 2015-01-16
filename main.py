#! /usr/bin/env python

import sys
import argparse
import pprint
import json
import subprocess

class PythonScript(object):
    def run(self, args):
        for path in args:
#            pprint.pprint(self.ffprobe("-show_format", path))
            width, height = self.get_dimensions(path)
            print "%dx%d" % (width, height)

    def get_dimensions(self, path):
            stream_info = self.ffprobe("-show_streams", path)

            video_streams = [
                x for x in stream_info['streams']
                if x['codec_type'] == 'video'
            ]

            if not video_streams:
                raise Exception("no video stream found")

            first_video_stream = video_streams[0]
            height = first_video_stream['height']
            width = first_video_stream['width']

            return width, height

            
    def ffprobe(self, format_option, path):
        return json.loads(
            subprocess.check_output(
                [
                    "/usr/local/bin/ffprobe", "-v", "quiet", "-print_format",
                    "json", format_option, path
                ]
            )
        )

        


if __name__ == "__main__":
    obj = PythonScript()
    obj.run(sys.argv[1:])
        
