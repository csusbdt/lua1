#include "global.h"

// On failure, returns NULL. Call SDL_GetError() for error string.
TTF_Font * get_font(lua_State * L, int stack_pos) {
	TTF_Font ** ud;
	TTF_Font * font;

	if (!lua_isuserdata(L, stack_pos)) { // lua_islightuserdata does not work here.
		luaL_error(L, "userdata expected");
	}
	ud = (TTF_Font **) lua_touserdata(L, 1);
	if (ud == NULL) {
		luaL_error(L, "Userdata null");
	}
	font = *ud;
	if (font == NULL) {
		luaL_error(L, "font null");
	} else {
		return *ud;
	}
}

static int open_font(lua_State * L) {
	const char * filename;
	lua_Integer fontsize;
	TTF_Font * font;
	TTF_Font ** ud;
	SDL_RWops * file;
	
	// Check argument 1.
	if (lua_gettop(L) != 2) {
		luaL_error(L, "open_font takes 2 arguments: filename and size");
	}
	if (lua_type(L, 1) != LUA_TSTRING) {
		luaL_error(L, "first argument to open_font should be a filename");
	}
	filename = luaL_checkstring(L, 1);
	file = SDL_RWFromFile(resource_path(filename), "rt");
	if (!file) {
		luaL_error(L, "%s", SDL_GetError());
	}

	// Check argument 2.
	if (lua_type(L, 2) != LUA_TNUMBER) {
		luaL_error(L, "second argument to open_font should be a font size");
	}
	fontsize = luaL_checkinteger(L, 2);

	// Open font.
	font = TTF_OpenFontRW(file, 1, fontsize);
	if (!font) {
		luaL_error(L, "%s", TTF_GetError());
	}

	// Return font pointer as userdata.
	ud = (TTF_Font **) lua_newuserdata(L, sizeof(TTF_Font *));
	if (!ud) {
		luaL_error(L, "%s", SDL_GetError());
	}
	*ud = font;
	return 1;
}

static int close_font(lua_State * L) {
	TTF_Font ** ud;
	TTF_Font * font;

	if (lua_gettop(L) != 1) {
		luaL_error(L, "close_font takes 1 argument");
	}
	if (!lua_isuserdata(L, 1)) { // lua_islightuserdata does not work here.
		luaL_error(L, "close_font argument not a font");
	}
	ud = (TTF_Font **) lua_touserdata(L, 1);
	font = *ud;
	if (!font) {
		luaL_error(L, "can not close null font");
	}
	TTF_CloseFont(font);
	*ud = NULL;
	return 0;
}

void register_font_functions(lua_State * L) {
	lua_register(L, "open_font"  , open_font  );
	lua_register(L, "close_font" , close_font );
}

