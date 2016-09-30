#! /usr/bin/env python

import sys
import argparse
import pprint
import json
import subprocess
import tempfile
import os

# make sure we set aspect
# XXX: QUITE OFTEN YOU HAVE TO REENCODE TO SAME CODEC BEFORE YOU CAN JOIN AT ALL
# That is,
# even if all files have the same codec,
# but some have different aspect + resolution,
# you need to rescale them ALL to the correct values
# if you just rescale ONE to match the majority resolution,
# it will end up in a different codec,
# and then joining them will fail.

# We need to use this on the chunks otherwise shit breaks.

# http://stackoverflow.com/questions/14005110/how-to-split-a-video-using-ffmpeg-so-that-each-chunk-starts-with-a-key-frame

#st=fade start time
#d= fade duration

#ffmpeg -i 1.mp4 -i 2.mp4 -f lavfi -i color=black -filter_complex \
#"[0:v]format=pix_fmts=yuva420p,fade=t=out:st=4:d=1:alpha=1,setpts=PTS-STARTPTS[va0];\
#[1:v]format=pix_fmts=yuva420p,fade=t=in:st=0:d=1:alpha=1,setpts=PTS-STARTPTS+4/TB[va1];\
#[2:v]scale=960x720,trim=duration=9[over];\
#[over][va0]overlay[over1];\
#[over1][va1]overlay=format=yuv420[outv]" \
#-vcodec libx264 -map [outv] out.mp4

# or also

# http://stackoverflow.com/questions/22289389/how-do-i-crossfade-between-scenes-in-ffmpeg-using-a-complex-filter

class PythonScript(object):
    def run(self, args):
        output_path = args[0]
        input_paths = args[1:]
        
        # XXX need to scan for aspect.
        # or maybe best to just specify a resolution
        preset = "ultrafast"
        aspect = "16:9"
        sample_rate = 44100
        quality = 4
        channels = 2

        width, height = self.scan_for_resolution(input_paths)
        print "Chosen dimensions: %dx%d" % (width, height)
        
        tempfiles = []
        
        for path in input_paths:
            tmp = tempfile.mkstemp(
                suffix=".mkv", prefix="scaled-"
            )
            scale_arg = "scale=%d:%d" % (width, height)
            subprocess.check_call([
                "ffmpeg", "-y", "-fflags", "+genpts", "-i",
                path, "-preset", preset, "-vf", scale_arg,
                "-ar", str(sample_rate),
                "-aq", str(quality),
                "-ac", str(channels),
                "-aspect", aspect, tmp[1]
            ])
            tempfiles.append(tmp[1])

        self.join_videos(tempfiles, output_path)

        for path in tempfiles:
            os.remove(path)

    def join_videos(self, paths, output):
        print repr(paths)
        with tempfile.NamedTemporaryFile(
            suffix=".lst", prefix="concat_list-", delete=False
        ) as concat_list:
            for path in paths:
                print repr(path)
                concat_list.write("file '%s'\n" % path)

            concat_list_path = concat_list.name

        subprocess.check_call([
            "ffmpeg", "-f", "concat", "-i", concat_list_path,
            "-c", "copy", output
        ])

        os.remove(concat_list_path)
            
    def scan_for_resolution(self, paths):
        min_height = float('inf')
        min_width = float('inf')
        
        for path in paths:
            width, height = self.get_dimensions(path)
            if width < min_width:
                min_width = width

            if height < min_height:
                min_height = height

        return min_width, min_height

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
        
