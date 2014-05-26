#include "global.h"

extern char * resource_dir;
extern char * save_dir;
const SDL_Color APP_WHITE = { 255, 255, 255, 255 };

/*
void error(const char * fmt, ...) {
	va_list v;
	char buf[1024];

	va_start(v, fmt);
	vsnprintf(buf, 1024, fmt, v);
	va_end(v);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, app_title, buf, NULL);
}
*/

void fatal(const char * fmt, ...) {
	va_list v;
	char buf[1024];

	va_start(v, fmt);
	vsnprintf(buf, 1024, fmt, v);
	va_end(v);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, app_title, buf, NULL);
	exit(1);
}

// Push error string onto stack.
int app_error_1(lua_State * L, const char * fmt, ...) {
	va_list v;
	char buf[1024];

	va_start(v, fmt);
	vsnprintf(buf, 1024, fmt, v);
	va_end(v);
	lua_settop(L, 0);
	lua_pushstring(L, buf);
	return 1;
}

// Push nil and error string onto stack.
int app_error_2(lua_State * L, const char * fmt, ...) {
	va_list v;
	char buf[1024];

	va_start(v, fmt);
	vsnprintf(buf, 1024, fmt, v);
	va_end(v);
	lua_settop(L, 0);
	lua_pushnil(L);
	lua_pushstring(L, buf);
	return 2;
}

int sdl_error_1(lua_State * L) {
	return app_error_1(L, "%s", SDL_GetError());
//	lua_settop(L, 0);
//	lua_pushstring(L, SDL_GetError());
//	return 1;
}

int sdl_error_2(lua_State * L) {
	return app_error_2(L, "%s", SDL_GetError());
//	lua_settop(L, 0);
//	lua_pushnil(L);
//	lua_pushstring(L, SDL_GetError());
//	return 2;
}

int ttf_error_1(lua_State * L) {
	return app_error_1(L, "%s", TTF_GetError());
//	lua_settop(L, 0);
//	lua_pushstring(L, TTF_GetError());
//	return 1;
}

int ttf_error_2(lua_State * L) {
	return app_error_2(L, "%s", TTF_GetError());
//	lua_settop(L, 0);
//	lua_pushnil(L);
//	lua_pushstring(L, TTF_GetError());
//	return 2;
}

int img_error_1(lua_State * L) {
	return app_error_1(L, "%s", IMG_GetError());
//	lua_settop(L, 0);
//	lua_pushstring(L, IMG_GetError());
//	return 1;
}

int img_error_2(lua_State * L) {
	return app_error_2(L, "%s", IMG_GetError());
//	lua_settop(L, 0);
//	lua_pushnil(L);
//	lua_pushstring(L, IMG_GetError());
//	return 2;
}

#define MAX_PATH 256
static char dir[MAX_PATH];

const char * resource_path(const char * path) {
	if (SDL_strlcpy(dir, resource_dir, MAX_PATH) >= MAX_PATH) {
		fatal("Resource dir pathname too long: %s", resource_dir);
	}
	if (SDL_strlcat(dir, path, MAX_PATH) >= MAX_PATH) {
		fatal("Resource file pathname too long: %s%s", resource_dir, path);
	}
	return dir;
}

const char * save_path(const char * path) {
	int maxlen;

	if (SDL_strlcpy(dir, save_dir, MAX_PATH) >= MAX_PATH) {
		fatal("Save dir pathname too long: %s", save_dir);
	}
	if (SDL_strlcat(dir, path, MAX_PATH) >= MAX_PATH) {
		fatal("Save file pathname too long: %s%s", save_dir, path);
	}
	return dir;
}

