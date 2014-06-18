#include "global.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

extern SDL_Renderer * renderer;
TTF_Font * get_font(lua_State * L, int stack_pos);

static int texture_from_file(lua_State * L) {
	const char     * filename;
	SDL_Texture    * texture;
	SDL_Texture   ** ud;
	int              w;
	int              h;
	int              bytes_per_pixel;
	Uint32           rmask;
	Uint32           gmask;
	Uint32           bmask;
	Uint32           amask;
	unsigned char  * pixels;
	SDL_Surface    * surface;

	// Check arguments.
	if (lua_gettop(L) != 1) {
		luaL_error(L, "texture_from_file takes 1 argument: filename");
	}
	if (lua_type(L, 1) != LUA_TSTRING) {
		luaL_error(L, "first argument to texture_from_file should be a filename");
	}

	// Extract arguments.
	filename = luaL_checkstring(L, 1);

	// Load pixel data.
	pixels = stbi_load(filename, &w, &h, &bytes_per_pixel, 0);
	if (pixels == NULL) {
		luaL_error(L, "%s", stbi_failure_reason());
	}

	// Create SDL surface from pixel data.
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
	#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
	#endif
	if (bytes_per_pixel == 4) {
		surface = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
	} else if (bytes_per_pixel == 3) {
		surface = SDL_CreateRGBSurface(0, w, h, 24, rmask, gmask, bmask, 0);
	} else {
		stbi_image_free(pixels);
		luaL_error(L, "Pixel format not supported.");
	}
	memcpy(surface->pixels, pixels, bytes_per_pixel * w * h);
	stbi_image_free(pixels);

	// Create texture from surface.
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		luaL_error(L, "%s", SDL_GetError());
	}
	SDL_FreeSurface(surface);

	// Set the blend mode for the texture.
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	// Return texture pointer, width and height.
	ud = (SDL_Texture **) lua_newuserdata(L, sizeof(SDL_Texture *));
	if (ud == NULL) {
		luaL_error(L, "Failed to create userdata in texture_from_file.");
	}
	*ud = texture;
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);
	return 3;
}

// Push texture pointer as userdata, followed by width and height of texture.
// This is currently used only by texture_from_font but could be used by other functions in future.
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

/*
	3 Possible ways to call texture_from_font from Lua:

	texture_from_font(ud, text)
	texture_from_font(ud, text, r, g, b)
	texture_from_font(ud, text, r, g, b, a)
NOTE: I don't have a need to specify alpha at this point.
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

	// Do it.
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

	// Extract arguments.
	ud = (SDL_Texture **) lua_touserdata(L, 1);
	if (ud == NULL) {
		luaL_error(L, "userdata pointer unexpectedly null");
	}
	texture = *ud;
	if (texture == NULL) {
		luaL_error(L, "destroy_texture called with null value");
	}

	// Do it.
	SDL_DestroyTexture(texture);
	*ud = NULL; // Set to NULL to help debugging.
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

	// Check arguments.
	numargs = lua_gettop(L);
	if (numargs != 1 && numargs != 3 && numargs != 5 && numargs != 7 && numargs != 9) {
		luaL_error(L, "render_texture called with wrong number of arguments");
	}
	if (lua_type(L, 1) != LUA_TUSERDATA) {
		luaL_error(L, "First argument to render_texture not userdata.");
	}
	
	// Extract userdata argument.
	ud = (SDL_Texture **) lua_touserdata(L, 1);
	if (!ud) {
		luaL_error(L, "Userdata pointer unexpectedly null.");
	}
	texture = *ud;
	if (texture == NULL) {
		luaL_error(L, "Null texture passed to render_texture");
	}

	// Extract rectangle arguments and do it.
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

static int texture_alpha_mod(lua_State * L) {
	SDL_Texture ** ud;
	SDL_Texture * texture;
	int alpha;	

	// Check arguments.
	if (lua_gettop(L) != 2) {
		luaL_error(L, "texture_blendmode_blend takes 2 arguments: texture userdata and alpha value");
	}
	if (lua_type(L, 1) != LUA_TUSERDATA) {
		luaL_error(L, "first argument to texture_alpha_mod should be light userdata");
	}
	if (lua_type(L, 2) != LUA_TNUMBER) {
		luaL_error(L, "second argument to texture_alpha_mod should be an integer in 0 .. 255");
	}

	// Extract arguments.
	ud = (SDL_Texture **) lua_touserdata(L, 1);
	if (ud == NULL) {
		luaL_error(L, "userdata pointer unexpectedly null in texture_blendmode_blend");
	}
	texture = *ud;
	if (texture == NULL) {
		luaL_error(L, "texture_blendmode_blend called with null value");
	}
	alpha = luaL_checkinteger(L, 2);
	if (alpha < 0 || alpha > 255) {
		luaL_error(L, "alpha value in texture_alpha_mod should be an integer in 0 .. 255");
	}

	// Set it.
	if (SDL_SetTextureAlphaMod(texture, alpha)) {
		luaL_error(L, SDL_GetError());
	}
	return 0;
}

void register_texture_functions(lua_State * L) {
	lua_register(L, "texture_from_file" , texture_from_file );
	lua_register(L, "texture_from_font" , texture_from_font );
	lua_register(L, "destroy_texture"   , destroy_texture   );
	lua_register(L, "render_texture"    , render_texture    );
	lua_register(L, "texture_alpha_mod" , texture_alpha_mod );
//	lua_register(L, "set_texture_blend_mode_to_blend" , set_texture_blend_mode_to_blend );
}
