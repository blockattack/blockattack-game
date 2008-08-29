# Option      #
opts = Options(args = ARGUMENTS)
opts.Add('destdir', 'Staging area to install BlockAttack to.  Useful for packagers.', '')
opts.Add('prefix', 'Destination directory', '/usr/local')
opts.Add('sharedir', 'Directory to use to store data file', '$prefix/share/blockattack')
opts.Add('bindir', 'Directory to use to store data file', '$prefix/bin')
opts.Add('mandir', 'Directory to use to store data file', '$prefix/share/man')

# Sources #
src = Split("""main.cpp
highscore.cpp
SFont.c
ReadKeyboard.cpp
joypad.cpp
listFiles.cpp
replay.cpp""")

# Copy Build Environment #
env = Environment(options = opts)
Export("env")
SConsignFile()

# Modify Build Environment #
env.ParseConfig('sdl-config --cflags --libs')
env.Append(CPPDEFINES = [('SHAREDIR', '\\"$sharedir\\"')])
env.Append(LIBS = ['SDL', 'enet','SDL_image','SDL_mixer'])
env.Append(LIBPATH = ['/usr/lib'])


# Compile Executable #
blockattack = env.Program('blockattack', src)
Default(blockattack)

# game materials (sharefiles)
SConscript('Game/SConscript')

# manpage
SConscript('man/SConscript')
# icons menu
env.Install ('$destdir/$prefix/share/pixmaps/',['blockattack32.xpm','blockattack64.xpm'])
env.Alias('install', ['$destdir/$prefix/share/pixmaps/'])

env.Install ('$destdir/$prefix/share/applications/',['blockattack.desktop'])
env.Alias('install', ['$destdir/$prefix/share/applications'])

# Install blockattack itself
env.Install('$destdir/$bindir/', blockattack)
env.Alias('install', ['$destdir/$bindir/'])
