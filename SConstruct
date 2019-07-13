import os

env = Environment()

maybe_term = os.environ.get('TERM')
if maybe_term:
    env['ENV']['TERM'] = maybe_term

prefix = "/usr/local"
bindir = prefix + "/bin"

pkg_config_packages = [
    "gstreamer-1.0",
    "gtk+-3.0",
    "gstreamer-video-1.0"
]

env.ParseConfig("pkg-config --cflags --libs " + ' '.join(pkg_config_packages))
env.Append(LIBS=['m'])
env.Append(
    CFLAGS=[
        '-Wall',
        #'-Werror'
    ]
)

env.Program(
    'kittysxchoppas',
    [
        'main.c', 'layer1.c', 'kittysxchoppas.c'
    ]
)

env.Program('test', ['test.c'], LIBS=['check', 'subunit', 'rt', 'm', 'pthread'])

install_bin = env.Install(dir=bindir, source="kittysxchoppas")

env.Alias('install', [install_bin])
