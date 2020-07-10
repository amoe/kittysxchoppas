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

`keyframe-snap-cut` -- this will divide a video based on keyframe snaps.

