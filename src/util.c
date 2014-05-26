#include "global.h"

extern bool running;

static int quit(lua_State * L) {
	running = false;
	return 0;
}

static int msgbox(lua_State * L) {
	const char * msg;
	
	msg = luaL_checkstring(L, 1);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, app_title, msg, NULL); 
	return 0;
}

static int render_clear(lua_State * L) {
	SDL_RenderClear(renderer);
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

static int draw_line(lua_State * L) {
	int x1, y1, x2, y2;

	x1 = luaL_checkinteger(L, 1);
	y1 = luaL_checkinteger(L, 2);
	x2 = luaL_checkinteger(L, 3);
	y2 = luaL_checkinteger(L, 4);
	if (SDL_RenderDrawLine(renderer, x1, y1, x2, y2)) fatal(SDL_GetError());
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
		lua_pushstring(L, "Failed to seek to end of file.");
		lua_error(L);
	}
	if (SDL_RWseek(file, 0, RW_SEEK_SET) < 0) {
		lua_pushstring(L, "Failed to seek to beginning of file.");
		lua_error(L);
	}
	buf = SDL_malloc(len);
	if (!buf) {
		lua_pushstring(L, "Probably not enough memory to store file.");
		lua_error(L);
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
		lua_settop(L, 0);
		lua_pushstring(L, "Failed to open file for writing.");
		lua_error(L);
	}
	if (data && SDL_RWwrite(file, data, 1, len) != len) {
		lua_settop(L, 0);
		lua_pushstring(L, "Failed to write file.");
		lua_error(L);
	}
	SDL_RWclose(file);
	return 0;
}

static int render_present(lua_State * L) {
	SDL_RenderPresent(renderer);
	return 0;
}

void register_util_functions(lua_State * L) {
	lua_register(L, "quit"           , quit           );
	lua_register(L, "msgbox"         , msgbox         );
	lua_register(L, "set_draw_color" , set_draw_color );
	lua_register(L, "draw_line"      , draw_line      );
	lua_register(L, "read_file"      , read_file      );
	lua_register(L, "render_present" , render_present );
	lua_register(L, "render_clear"   , render_clear   );
	lua_register(L, "write_file"     , write_file     );
	lua_register(L, "load_chunk"     , load_chunk     );
}
