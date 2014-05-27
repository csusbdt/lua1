# Ta generate an empty plist.
# pkgbuild --analyze --root ./build/${APP}.app Info.plist

default : osx

clean :
	rm -rf build

osx :
	if [ ! -d build ]; then mkdir -p build; fi
	if [ -e build/a.out ]; then rm build/a.out; fi
	clang 	-o build/a.out                 \
		`sdl2-config --libs --cflags`  \
		-I /usr/local/include          \
		-llua                          \
		-lSDL2_ttf                     \
		-lSDL2_image                   \
		-D SDL_ASSERT_LEVEL=2          \
		src/*.c

# http://stackoverflow.com/questions/17620884/static-linking-of-sdl2-libraries
# says to use ./configure --enable-static when building sdl.
osx-static :
	if [ ! -d build ]; then mkdir -p build; fi
	if [ -e build/a.out ]; then rm build/a.out; fi
	clang 	-o build/a.out                                \
		`sdl2-config --libs --cflags  --static-libs`  \
		-L /opt/X11/lib                               \
		-I /usr/local/include                         \
		-Bstatic                                      \
		-llua                                         \
		-lSDL2_ttf                                    \
		-lSDL2_image                                  \
		-lfreetype.6                                  \
		-D SDL_ASSERT_LEVEL=2                         \
		src/*.c

test :
	if [ ! -e build/a.out ]; then make; fi
	cp build/a.out working
	cd working; ./a.out

