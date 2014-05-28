# To generate an empty plist.
# pkgbuild --analyze --root ./build/${APP}.app Info.plist

.PHONY : osx

default : osx

clean :
	rm -rf build

osx :
	if [ ! -d build ]; then mkdir -p build; fi
	if [ -e build/a.out ]; then rm build/a.out; fi
	clang 	-o build/a.out               \
		-D _THREAD_SAFE              \
		-D SDL_ASSERT_LEVEL=2        \
		-I include                   \
		-L osx/lib                   \
		-l SDL2                      \
		-l z                         \
		-l bz2                       \
		-l m                         \
		-l iconv                     \
		-l objc                      \
		-l lua                       \
		-l png                       \
		-l SDL2_ttf                  \
		-l SDL2_image                \
		-l freetype                  \
		-Wl,-framework,OpenGL        \
		-Wl,-framework,ForceFeedback \
		-Wl,-framework,Cocoa         \
		-Wl,-framework,Carbon        \
		-Wl,-framework,IOKit         \
		-Wl,-framework,CoreAudio     \
		-Wl,-framework,AudioToolbox  \
		-Wl,-framework,AudioUnit     \
		src/*.c


test :
	if [ ! -e build/a.out ]; then make; fi
	cp build/a.out working
	cd working; ./a.out

