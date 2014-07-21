cd ~/temp
cp -r ~/SDL-2.0.3-8717/android-project android
cd proj
cd jni
ln -s ../../../SDL-2.0.3-8717 SDL

cd src

Create file main.c with a hello program in it.

Edit Android.mk and replace "YourSourceHere.c" with "main.c".

In SDL/include, overwrite SDL_config.h with SDL_config_android.h.

Update the android project:

    android update project --target android-20 --path .

Build the native code:

    ndk-build

Test:

    cd android/proj
    ant debug
    < connect device >
    ant installd
    ant uninstall

Test with SDL2_ttf.

cd android/proj/jni
ln -s ../../../SDL2_ttf-2.0.12 SDL2_ttf

Add the following to android/proj/jni/src/Android.mk:

    SDL_TTF_PATH := ../SDL2_ttf

    < after LOCAL_C_INCLUDES := ...>
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(SDL_TTF_PATH)

    LOCAL_SHARED_LIBRARIES += SDL2_ttf

Uncomment the following line in SDLActivity.java:

    System.loadLibrary("SDL2_ttf");

Copy DroidSansMono.ttf into android/res/

Add the following code to main.c:

~~~~
#include "SDL_ttf.h"

if (TTF_Init() == -1) {
    printf("TTF_Init: %s\n", TTF_GetError());
    exit(2);
}

SDL_RWops * file;
TTF_Font * font;

file = SDL_RWFromFile(resource_path(filename), "rt");
if (!file) {
    printf("SDL_RWFromFile: %s\n", SDL_GetError());
    exit(3);
}

font = TTF_OpenFontRW(file, 1, 16);
if (!font) {
    exit(4);
}

font = TTF_OpenFont("DroidSansMono.ttf", 16);
if (!font) {
    printf("TTF_OpenFont: %s\n", TTF_GetError());
    exit(5);
}

TTF_Quit();
~~~~


