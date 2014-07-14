#include "global.h"

extern bool running;

static int fullscreen(lua_State * L) {
	if (is_ios() || is_android()) return 0;
	if (app_fullscreen) return 0;
	app_fullscreen = true;
	if (SDL_SetWindowFullscreen(window, SDL_GetWindowFlags(window) | SDL_WINDOW_FULLSCREEN_DESKTOP)) {
		luaL_error(L, "%s", SDL_GetError());
	}
	return 0;
}

int windowed(lua_State * L) {
	if (is_ios() || is_android()) return 0;
	if (!app_fullscreen) return 0;
	app_fullscreen = false;
	if (SDL_SetWindowFullscreen(window, SDL_GetWindowFlags(window) ^ SDL_WINDOW_FULLSCREEN_DESKTOP)) {
		luaL_error(L, "%s", SDL_GetError());
	}
	return 0;
}

static int quit(lua_State * L) {
	running = false;
	return 0;
}

static int get_ticks(lua_State * L) {
        lua_pushnumber(L, SDL_GetTicks());
        return 1;
}

static int msgbox(lua_State * L) {
	const char * msg;
	
	msg = luaL_checkstring(L, 1);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, app_title, msg, NULL); 
	SDL_RaiseWindow(window);
	return 0;
}

static int set_draw_color(lua_State * L) {
	int r, g, b, a;

	r = luaL_checkinteger(L, 1);
	g = luaL_checkinteger(L, 2);
	b = luaL_checkinteger(L, 3);
	a = luaL_checkinteger(L, 4);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	return 0;
}

static int set_window_size(lua_State * L) {
	int w, h;

	w = luaL_checkinteger(L, 1);
	h = luaL_checkinteger(L, 2);
	SDL_SetWindowSize(window, w, h);
	return 0;
}

static int set_window_position(lua_State * L) {
	int x, y;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	SDL_SetWindowPosition(window, x, y);
	return 0;
}

static int draw_line(lua_State * L) {
	int x1, y1, x2, y2;

	x1 = luaL_checkinteger(L, 1);
	y1 = luaL_checkinteger(L, 2);
	x2 = luaL_checkinteger(L, 3);
	y2 = luaL_checkinteger(L, 4);
	if (SDL_RenderDrawLine(renderer, x1, y1, x2, y2)) {
		luaL_error(L, "%s", SDL_GetError());
	}
	return 0;
}

static int fill_rect(lua_State * L) {
	SDL_Rect rect;

	rect.x = luaL_checkinteger(L, 1);
	rect.y = luaL_checkinteger(L, 2);
	rect.w  = luaL_checkinteger(L, 3);
	rect.h  = luaL_checkinteger(L, 4);
	if (SDL_RenderFillRect(renderer, &rect)) {
		luaL_error(L, "%s", SDL_GetError());
	}
	return 0;
}

static int blendmode_none(lua_State * L) {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	return 0;
}

static int blendmode_blend(lua_State * L) {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	return 0;
}

static int blendmode_add(lua_State * L) {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
	return 0;
}

static int blendmode_mod(lua_State * L) {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MOD);
	return 0;
}

static int set_hint_render_scale_quality_nearest(lua_State * L) {
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	return 0;
}

static int set_hint_render_scale_quality_linear(lua_State * L) {
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	return 0;
}

static int render(lua_State * L) {
	SDL_RenderPresent(renderer);
	return 0;
}

static int render_clear(lua_State * L) {
	SDL_RenderClear(renderer);
	return 0;
}

static int read_string(lua_State * L, bool from_resource_path) {
	const char * filename;
	SDL_RWops * file;
	Sint64 len;
	char * buf;
	
	filename = luaL_checkstring(L, 1);
	if (from_resource_path) {
		file = SDL_RWFromFile(resource_path(filename), "rb");
	} else {
		file = SDL_RWFromFile(save_path(filename), "rb");
	}
	if (!file) {
		lua_pushnil(L);
		return 0;
	}
	len = SDL_RWseek(file, 0, SEEK_END);
	if (len < 0) {
		luaL_error(L, "Failed to seek to end of file.");
	}
	if (SDL_RWseek(file, 0, RW_SEEK_SET) < 0) {
		luaL_error(L, "Failed to seek to beginning of file.");
	}
	buf = SDL_malloc(len);
	if (!buf) {
		luaL_error(L, "Malloc failed in read_string.");
	}
	SDL_RWread(file, buf, len, 1);
	SDL_RWclose(file);
	lua_pushlstring(L, buf, len);
	SDL_free(buf);
	return 1;
}

static int read_file(lua_State * L) {
	return read_string(L, false);
}

static int load_chunk(lua_State * L) {
	return read_string(L, true);
}

static int write_file(lua_State * L) {
	const char * filename;
	const char * data;
	size_t len;
	SDL_RWops * file;
	
	filename = luaL_checkstring(L, 1);
	if (lua_isnil(L, 2)) {
		data = NULL;
		len = 0;
	} else {
		data = luaL_checklstring(L, 2, &len); // The string from Lua is null terminated.
	}
	file = SDL_RWFromFile(save_path(filename), "wb");
	if (!file) {
		luaL_error(L, "%s", SDL_GetError());
	}
	if (data && SDL_RWwrite(file, data, 1, len) != len) {
		luaL_error(L, "%s", SDL_GetError());
	}
	SDL_RWclose(file);
	return 0;
}

void register_util_functions(lua_State * L) {
	lua_register(L, "fullscreen"          , fullscreen          );
	lua_register(L, "windowed"            , windowed            );
	lua_register(L, "set_window_size"     , set_window_size     );
	lua_register(L, "set_window_position" , set_window_position );
	lua_register(L, "quit"                , quit                );
	lua_register(L, "get_ticks"       , get_ticks       );
	lua_register(L, "msgbox"          , msgbox          );
	lua_register(L, "set_draw_color"  , set_draw_color  );
	lua_register(L, "blendmode_none"  , blendmode_none  );
	lua_register(L, "blendmode_blend" , blendmode_blend );
	lua_register(L, "blendmode_add"   , blendmode_add   );
	lua_register(L, "blendmode_mod"   , blendmode_mod   );
	lua_register(L, "set_hint_render_scale_quality_linear"  , set_hint_render_scale_quality_linear  );
	lua_register(L, "set_hint_render_scale_quality_nearest" , set_hint_render_scale_quality_nearest );
	lua_register(L, "draw_line"       , draw_line       );
	lua_register(L, "fill_rect"       , fill_rect       );
	lua_register(L, "render"          , render          );
	lua_register(L, "render_clear"    , render_clear    );
	lua_register(L, "read_file"       , read_file       );
	lua_register(L, "write_file"      , write_file      );
	lua_register(L, "load_chunk"      , load_chunk      );
}

