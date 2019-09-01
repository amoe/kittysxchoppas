import setuptools

setuptools.setup(
    name='kittysxchoppas',
    version='0.0.1',
    packages=setuptools.find_packages('src'),
    package_dir={'': 'src'},
    entry_points={
        'console_scripts': [
            'keyframe-snap-cut=kittysxchoppas.keyframe_snap_cut:main',
            'cut-based-on-chapters=kittysxchoppas.cut_based_on_chapters:main',
        ]
    }
)
