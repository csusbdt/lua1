#ifndef APP_GLOBAL_H
#define APP_GLOBAL_H

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

typedef int bool;
#define false 0
#define true 1

extern SDL_Window   * window;
extern SDL_Renderer * renderer;
extern int app_width;
extern int app_height;
extern bool app_fullscreen;
extern char * app_title;

extern const SDL_Color APP_WHITE;

bool is_ios();
bool is_android();
bool is_osx();
bool is_windows();
bool is_linux();
void fatal(const char * fmt, ...);
int windowed(lua_State * L);
int app_error_1(lua_State * L, const char * fmt, ...);
int app_error_2(lua_State * L, const char * fmt, ...);
int sdl_error_1(lua_State * L);
int sdl_error_2(lua_State * L);
int img_error_1(lua_State * L);
int img_error_2(lua_State * L);
int ttf_error_1(lua_State * L);
int ttf_error_2(lua_State * L);

const char * resource_path(const char * path);
const char * save_path(const char * path);

#endif

