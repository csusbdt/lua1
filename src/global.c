#include "global.h"

extern char * resource_dir;
extern char * save_dir;
const SDL_Color APP_WHITE = { 255, 255, 255, 255 };

bool is_ios() {
	return SDL_strcmp(SDL_GetPlatform(), "iOS") == 0;
}

bool is_android() {
	return SDL_strcmp(SDL_GetPlatform(), "Android") == 0;
}

bool is_osx() {
	return SDL_strcmp(SDL_GetPlatform(), "Mac OS X") == 0;
}

bool is_windows() {
	return SDL_strcmp(SDL_GetPlatform(), "Windows") == 0;
}

bool is_linux() {
	return SDL_strcmp(SDL_GetPlatform(), "Linux") == 0;
}

void fatal(const char * fmt, ...) {
	va_list v;
	char buf[1024];

	va_start(v, fmt);
	vsnprintf(buf, 1024, fmt, v);
	va_end(v);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, app_title, buf, NULL);
	exit(1);
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

