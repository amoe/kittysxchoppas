env = Environment()

prefix = "/usr/local"
bindir = prefix + "/bin"

pkg_config_packages = [
    "gstreamer-1.0",
    "gtk+-3.0",
    "gstreamer-video-1.0"
]

env.ParseConfig("pkg-config --cflags --libs " + ' '.join(pkg_config_packages))
env.Append(LIBS=['m'])

env.Program('kittysxchoppas', ['kittysxchoppas.c'])

env.Alias('install', env.Install(dir=bindir, source="kittysxchoppas"))
