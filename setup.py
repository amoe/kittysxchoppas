import setuptools

setuptools.setup(
    name='kittysxchoppas',
    version='0.0.1',
    packages=['kittysxchoppas'],
    entry_points={
        'console_scripts': ['keyframe-snap-cut=kittysxchoppas.keyframe_snap_cut:main']
    }
)
