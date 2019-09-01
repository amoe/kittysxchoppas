import ffprobe
import pprint
import sys
import keyframe_snap_cut

input_path = sys.argv[1]
info = ffprobe.ffprobe('-show_chapters', input_path)

cutter = keyframe_snap_cut.KeyframeSnapCut()

for chapter in info['chapters']:
    start_point = float(chapter['start_time'])
    end_point = float(chapter['end_time'])
    output_path = "chapter-{}.mkv".format(chapter['id'])
    cutter.run(start_point, end_point, input_path, output_path)


