import json
import sys
import kittysxchoppas.ffprobe


print("Scanning")
data = kittysxchoppas.ffprobe.ffprobe("-show_frames", sys.argv[1])
print("Scan done")

print("Read", len(data['frames']), "frame data")

frames = data['frames']

for f in frames:
    if f['key_frame'] == 1 and f['media_type'] == 'video':
        print("kf")



