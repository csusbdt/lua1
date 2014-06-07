#include "global.h"

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
	} 
	return *ud;
}

static int open_font(lua_State * L) {
	const char * filename;
	lua_Integer fontsize;
	TTF_Font * font;
	TTF_Font ** ud;
	SDL_RWops * file;
	
	// Check arguments.
	if (lua_gettop(L) != 2) {
		luaL_error(L, "open_font takes 2 arguments: filename and size");
	}
	if (lua_type(L, 1) != LUA_TSTRING) {
		luaL_error(L, "first argument to open_font should be a filename");
	}
	if (lua_type(L, 2) != LUA_TNUMBER) {
		luaL_error(L, "second argument to open_font should be a font size");
	}

	// Extract arguments.
	filename = luaL_checkstring(L, 1);
	fontsize = luaL_checkinteger(L, 2);

	// Open file.
	file = SDL_RWFromFile(resource_path(filename), "rt");
	if (!file) {
		luaL_error(L, "%s", SDL_GetError());
	}

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

static int font_texture_size(lua_State * L) {
	TTF_Font ** ud;
	TTF_Font * font;
	const char * text;
	int w;
	int h;

	// Check arguments.
	if (lua_gettop(L) != 2) {
		luaL_error(L, "texture_font_size takes 2 arguments: texture userdata and string");
	}
	if (lua_type(L, 1) != LUA_TUSERDATA) {
		luaL_error(L, "first argument to texture_font_size should be light userdata");
	}
	if (lua_type(L, 2) != LUA_TSTRING) {
		luaL_error(L, "second argument to texture_font_size should be a string");
	}

	// Extract arguments.
	ud = (TTF_Font **) lua_touserdata(L, 1);
	if (ud == NULL) {
		luaL_error(L, "userdata pointer unexpectedly null in font_texture_size");
	}
	font = *ud;
	if (font == NULL) {
		luaL_error(L, "font_texture_size called with null value");
	}
	text = luaL_checkstring(L, 2);
	if (text == NULL) {
		luaL_error(L, "luaL_checkstring returned NULL in texture_font_size");
	}

	// Determine and return texture width and height.
	if (TTF_SizeUTF8(font, text, &w, &h)) {
		luaL_error(L, "%s", TTF_GetError());
	}
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);
	return 2;
}

void register_font_functions(lua_State * L) {
	lua_register(L, "open_font"         , open_font         );
	lua_register(L, "close_font"        , close_font        );
	lua_register(L, "font_texture_size" , font_texture_size );
}

