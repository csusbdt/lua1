# Ta generate an empty plist.
# pkgbuild --analyze --root ./build/${APP}.app Info.plist

default : osx

clean :
	rm -rf build

osx :
	if [ ! -d build ]; then mkdir -p build; fi
	clang 	-o build/a.out                 \
		`sdl2-config --libs --cflags`  \
		-I /usr/local/include          \
		-llua                          \
		-lSDL2_ttf                     \
		-lSDL2_image                   \
		-D SDL_ASSERT_LEVEL=2          \
		src/*.c

test :
	if [ ! -e build/a.out ]; then make; fi
	cp build/a.out working
	cd working; ./a.out

