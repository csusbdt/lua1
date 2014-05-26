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

extern SDL_Renderer * renderer;
extern int app_width;
extern int app_height;
extern char * app_title;

extern const SDL_Color APP_WHITE;

void fatal(const char * fmt, ...);

const char * resource_path(const char * path);
const char * save_path(const char * path);

#endif

