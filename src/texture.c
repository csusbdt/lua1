#include "global.h"

// Add the following as a function callable by lua.
	//if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND)) {
	//	return sdl_error_2(L);
	//}

extern SDL_Renderer * renderer;
TTF_Font * get_font(lua_State * L, int stack_pos);

// Push texture pointer as userdata, followed by width and height of texture.
static int texture_from_surface(lua_State * L, SDL_Surface * surface) {
	SDL_Texture * texture;
	SDL_Texture ** ud;
	int w;
	int h;
	
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	if (!texture) {
		luaL_error(L, "%s", SDL_GetError());
	}

	ud = (SDL_Texture **) lua_newuserdata(L, sizeof(SDL_Texture *));
	if (ud == NULL) {
		luaL_error(L, "Failed to create userdata in texture_from_surface.");
	}

	*ud = texture;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);
	return 3;
}

static int texture_from_file(lua_State * L) {
	const char * filename;
	SDL_Surface * surface;
	SDL_RWops * file;

	// Check arguments.
	if (lua_gettop(L) != 1) {
		luaL_error(L, "texture_from_file takes 1 argument: filename");
	}
	if (lua_type(L, 1) != LUA_TSTRING) {
		luaL_error(L, "first argument to texture_from_file should be a filename");
	}

	// Extract arguments.
	filename = luaL_checkstring(L, 1);

	// Open file.
	file = SDL_RWFromFile(resource_path(filename), "rb");
	if (!file) {
		luaL_error(L, "%s", SDL_GetError());
	}

	// Load image data.
	surface = IMG_Load_RW(file, 1);
	if (!surface) {
		luaL_error(L, "%s", IMG_GetError());
	}

	return texture_from_surface(L, surface);
}

/*
	Possible ways to call texture_from_font from Lua:

	texture_from_font(ud, text)
	texture_from_font(ud, text, r, g, b)
	texture_from_font(ud, text, r, g, b, a)
*/
static int texture_from_font(lua_State * L) {
	TTF_Font * font;
	SDL_Surface * surface;
	const char * text;
	SDL_Color color;
	
	// Check arguments.
	if (lua_gettop(L) != 2 && lua_gettop(L) != 5 && lua_gettop(L) != 6) {
		luaL_error(L, "wrong number of arguments passed to texture_from_font");
	}
	if (lua_type(L, 1) != LUA_TUSERDATA) {
		luaL_error(L, "first argument to texture_from_font should be userdata");
	}
	if (lua_type(L, 2) != LUA_TSTRING) {
		luaL_error(L, "second argument to texture_from_font should be a string");
	}
	if (lua_gettop(L) == 5 || lua_gettop(L) == 6) {
		if (lua_type(L, 3) != LUA_TNUMBER) {
			luaL_error(L, "third argument to texture_from_font should be an integer");
		}
		if (lua_type(L, 4) != LUA_TNUMBER) {
			luaL_error(L, "fourth argument to texture_from_font should be an integer");
		}
		if (lua_type(L, 5) != LUA_TNUMBER) {
			luaL_error(L, "fifth argument to texture_from_font should be an integer");
		}
		if (lua_gettop(L) == 6 && lua_type(L, 6) != LUA_TNUMBER) {
			luaL_error(L, "sixth argument to texture_from_font should be an integer");
		}
	}

	// Extract arguments.
	font = get_font(L, 1);
	text = luaL_checkstring(L, 2);
	if (strlen(text) == 0) {
		luaL_error(L, "Empty string passed to texture_from_font");
	}
	color = APP_WHITE;
	if (lua_gettop(L) >= 5) {
		color.r = luaL_checkint(L, 3);
		color.g = luaL_checkint(L, 4);
		color.b = luaL_checkint(L, 5);
	}
	if (lua_gettop(L) == 6) {
		color.a = luaL_checkint(L, 6);
	}
	surface = TTF_RenderUTF8_Blended(font, text, color);
	return texture_from_surface(L, surface);
}

static int destroy_texture(lua_State * L) {
	SDL_Texture ** ud;
	SDL_Texture * texture;

	// Check arguments.
	if (lua_gettop(L) != 1) {
		luaL_error(L, "destroy_texture takes 1 argument: texture as light userdata");
	}
	if (lua_type(L, 1) != LUA_TUSERDATA) {
		luaL_error(L, "first argument to destroy_texture should be light userdata");
	}

	ud = (SDL_Texture **) lua_touserdata(L, 1);
	if (ud == NULL) {
		luaL_error(L, "userdata pointer unexpectedly null");
	}

	texture = *ud;
	if (texture == NULL) {
		luaL_error(L, "destroy_texture called with null value");
	}
	SDL_DestroyTexture(texture);
	*ud = NULL;
	return 0;
}

/*
	Possible ways to call render_texture from Lua:

	render_texture(ud)
	render_texture(ud, dst_x, dst_y) 
	render_texture(ud, dst_x, dst_y, dst_w, dst_h) 
	render_texture(ud, src_x, src_y, src_w, src_h, dst_x, dst_y) 
	render_texture(ud, src_x, src_y, src_w, src_h, dst_x, dst_y, dst_w, dst_h) 
*/
static int render_texture(lua_State * L) {
	int numargs;
	SDL_Texture ** ud;
	SDL_Texture * texture;
	SDL_Rect src;
	SDL_Rect dst;

	numargs = lua_gettop(L);

	if (numargs != 1 && numargs != 3 && numargs != 5 && numargs != 7 && numargs != 9) {
		return app_error_1(L, "render_texture called with wrong number of arguments");
	}

	if (lua_type(L, 1) != LUA_TUSERDATA) {
		return app_error_1(L, "First argument to render_texture not userdata.");
	}
	
	ud = (SDL_Texture **) lua_touserdata(L, 1);
	if (!ud) {
		return sdl_error_1(L);
	}

	texture = *ud;

	if (numargs == 1) {
	        SDL_RenderCopy(renderer, texture, NULL, NULL);
	} else if (numargs == 3) {
	        dst.x = luaL_checknumber(L, 2);
	        dst.y = luaL_checknumber(L, 3);
		SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
	        SDL_RenderCopy(renderer, texture, NULL, &dst);
	} else if (numargs == 5) {
	        dst.x = luaL_checknumber(L, 2);
	        dst.y = luaL_checknumber(L, 3);
		dst.w = luaL_checknumber(L, 4);
		dst.h = luaL_checknumber(L, 5);
	        SDL_RenderCopy(renderer, texture, NULL, &dst);
	} else if (numargs == 7) {
		src.x = luaL_checknumber(L, 2);
		src.y = luaL_checknumber(L, 3);
		src.w = luaL_checknumber(L, 4);
		src.h = luaL_checknumber(L, 5);
		dst.x = luaL_checknumber(L, 6);
		dst.y = luaL_checknumber(L, 7);
		dst.w = src.w;
		dst.h = src.h;
		SDL_RenderCopy(renderer, texture, &src, &dst);
	} else if (numargs == 9) {
		src.x = luaL_checknumber(L, 2);
		src.y = luaL_checknumber(L, 3);
		src.w = luaL_checknumber(L, 4);
		src.h = luaL_checknumber(L, 5);
		dst.x = luaL_checknumber(L, 6);
		dst.y = luaL_checknumber(L, 7);
		dst.w = luaL_checknumber(L, 8);
		dst.h = luaL_checknumber(L, 9);
		SDL_RenderCopy(renderer, texture, &src, &dst);
	} else {
		SDL_assert(false);
	}
	return 0;
}

static int texture_blendmode_blend(lua_State * L) {
	SDL_Texture ** ud;
	SDL_Texture * texture;
		

	// Check arguments.
	if (lua_gettop(L) != 1) {
		luaL_error(L, "texture_blendmode_blend takes 1 argument: texture as light userdata");
	}
	if (lua_type(L, 1) != LUA_TUSERDATA) {
		luaL_error(L, "first argument to texture_blendmode_blend should be light userdata");
	}

	ud = (SDL_Texture **) lua_touserdata(L, 1);
	if (ud == NULL) {
		luaL_error(L, "userdata pointer unexpectedly null in texture_blendmode_blend");
	}

	texture = *ud;
	if (texture == NULL) {
		luaL_error(L, "texture_blendmode_blend called with null value");
	}
	if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND)) {
		luaL_error(L, SDL_GetError());
	}
	return 0;
}

void register_texture_functions(lua_State * L) {
	lua_register(L, "texture_from_file"       , texture_from_file       );
	lua_register(L, "texture_from_font"       , texture_from_font       );
	lua_register(L, "destroy_texture"         , destroy_texture         );
	lua_register(L, "render_texture"          , render_texture          );
	lua_register(L, "texture_blendmode_blend" , texture_blendmode_blend );
}
