#include "global.h"

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
	if (!texture) return sdl_error_2(L);

	//if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND)) {
	//	return sdl_error_2(L);
	//}

	ud = (SDL_Texture **) lua_newuserdata(L, sizeof(SDL_Texture *));
	if (ud == NULL) {
		return app_error_2(L, "Failed to create userdata in texture_from_surface.");
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
		
	// Check number of arguments.
	if (lua_gettop(L) != 1) {
		lua_settop(L, 0);
		lua_pushnil(L);
		lua_pushstring(L, "texture_from_file takes 1 argument: filename");
		return 2;
	}

	// Check argument type.
	if (lua_type(L, 1) != LUA_TSTRING) {
		lua_settop(L, 0);
		lua_pushnil(L);
		lua_pushstring(L, "argument to texture_from_file should be a filename");
		return 2;
	}
	filename = luaL_checkstring(L, 1);

	// Open file.
	file = SDL_RWFromFile(resource_path(filename), "rb");
	if (!file) {
		lua_settop(L, 0);
		lua_pushnil(L);
		lua_pushstring(L, SDL_GetError());
		return 2;
	}

	// Load image data.
	surface = IMG_Load_RW(file, 1);
	if (!surface) {
		lua_settop(L, 0);
		lua_pushnil(L);
		lua_pushstring(L, IMG_GetError());
		return 2;
	}

	return texture_from_surface(L, surface);
}

static int texture_from_font(lua_State * L) {
	TTF_Font * font;
	SDL_Surface * surface;
	const char * text;
	SDL_Color color;
	
	font = get_font(L, 1);
	text = luaL_checkstring(L, 2);
	if (strlen(text) == 0) {
		lua_pushnil(L);
		return 1;
	}
	color = APP_WHITE;
	if (lua_gettop(L) >= 5) {
		color.r = luaL_checkint(L, 3);
		color.g = luaL_checkint(L, 4);
		color.r = luaL_checkint(L, 5);
	}
	if (lua_gettop(L) == 6) {
		color.a = luaL_checkint(L, 6);
	}
	surface = TTF_RenderText_Blended(font, text, color);
	return texture_from_surface(L, surface);
}

static int destroy_texture(lua_State * L) {
	SDL_Texture ** ud;
	SDL_Texture * texture;
		
	// Check number of arguments.
	if (lua_gettop(L) != 1) {
		return app_error_1(L, "destroy_texture called with wrong number of arguments");
	}

	// Check argument type.
	if (lua_type(L, 1) != LUA_TUSERDATA) {
		return app_error_1(L, "Argument to destroy_texture not userdata.");
	}
	
	ud = (SDL_Texture **) lua_touserdata(L, 1);
	if (!ud) {
		return sdl_error_1(L);
	}

	texture = *ud;
	SDL_DestroyTexture(texture);
	return 0;
}

/*
	Possible ways to call this function from Lua:

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

void register_texture_functions(lua_State * L) {
	lua_register(L, "texture_from_file" , texture_from_file );
	lua_register(L, "texture_from_font" , texture_from_font );
	lua_register(L, "destroy_texture"   , destroy_texture   );
	lua_register(L, "render_texture"    , render_texture    );
}
