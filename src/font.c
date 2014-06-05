#include "global.h"

// On failure, returns NULL. Call SDL_GetError() for error string.
TTF_Font * get_font(lua_State * L, int stack_pos) {
	TTF_Font ** ud;

	ud = (TTF_Font **) lua_touserdata(L, 1);
	if (ud) {
		return *ud;
	} else {
		return NULL;
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
		return app_error_2(L, "open_font takes 2 arguments: filename and size");
	}
	if (lua_type(L, 1) != LUA_TSTRING) {
		return app_error_2(L, "first argument to open_font should be a filename");
	}
	filename = luaL_checkstring(L, 1);
	file = SDL_RWFromFile(resource_path(filename), "rt");
	if (!file) return sdl_error_2(L);

	// Check argument 2.
	if (lua_type(L, 2) != LUA_TNUMBER) {
		return app_error_2(L, "second argument to open_font should be a font size");
	}
	fontsize = luaL_checkinteger(L, 2);

	// Open font.
	font = TTF_OpenFontRW(file, 1, fontsize);
	if (!font) {
		return ttf_error_2(L);
	}

	// Return font pointer as userdata.
	ud = (TTF_Font **) lua_newuserdata(L, sizeof(TTF_Font *));
	if (!ud) {
		return sdl_error_2(L);
	}
	*ud = font;
	return 1;
}

static int close_font(lua_State * L) {
	TTF_Font * font;

	if (lua_gettop(L) != 1) {
		return app_error_1(L, "close_font takes 1 argument");
		//return 0;
	}
	if (!lua_isuserdata(L, 1)) { // lua_islightuserdata does not work here.
		return app_error_1(L, "close_font argument not a font");
		//return 0;
	}
	font = get_font(L, 1);
	if (!font) {
		//error("%s %s", SDL_GetError());
		//return 0;
		return sdl_error_1(L);
	}
	TTF_CloseFont(font);
	return 0;
}

void register_font_functions(lua_State * L) {
	lua_register(L, "open_font"  , open_font  );
	lua_register(L, "close_font" , close_font );
}

