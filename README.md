# kittysxchoppas

This is several things:

1.  A GTK+ app that allows cutting videos at keyframes.
2.  The Python script that implements the actual keyframe cutting.
3.  A script to scale and join several videos.
4.  A script to cut movies based on chapters embedded in the file.

Build deps
----------

libglib2.0-dev
libgstreamer-plugins-base1.0-dev
libgtk-3-dev
ffmpeg
check (unit testing framework)

Build instructions
------------------

Get SCons 3.x or greater, probably from pip.

Run `scons install` to install under `/usr/local/bin/kittysxchoppas`.

Python scripts
--------------

You can use `setup.py` to install them.

Run `python3 setup.py install`.  This will create several scripts under
`/usr/local/bin`.



## keyframe-snap-cut

This will losslessly divide a video at keyframes.


Note: There are rare videos that can trigger a [bug in
ffmpeg](https://trac.ffmpeg.org/ticket/8820).  It's quite difficult to tell
which these are.  Their distinguishing feature is that keyframe-snap-cut will
see them as having very few keyframes.  For instance, they may appear to have
less than 10 keyframes for an entire video.  In reality, they do have a more
normal number of keyframes, but `-skip_frame nokey` does not include these
frames.  Unfortunately without `-skip_frame nokey` the keyframe gathering
process is an order of magnitude slower, and we use a slightly naive strategy,
meaning that dealing with this bug would be prohibitive given the tiny number of
files it affects.
