#include "global.h"

extern SDL_Renderer * renderer;
TTF_Font * get_font(lua_State * L, int stack_pos);

static int texture_from_surface(lua_State * L, SDL_Surface * surface) {
	SDL_Texture * texture;
	SDL_Texture ** ud;
	int w;
	int h;
	
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	if (!texture) {
		fatal(SDL_GetError());
	}
	ud = (SDL_Texture **) lua_newuserdata(L, sizeof(SDL_Texture *));
	if (ud == NULL) {
		fatal("failed to create userdata texture_from_surface");
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
	
	filename = luaL_checkstring(L, 1);
	file = SDL_RWFromFile(resource_path(filename), "rb");
	if (!file) {
		fatal(SDL_GetError());
	}
	surface = IMG_Load_RW(file, 1);
	if (!surface) fatal(SDL_GetError()); 
	return texture_from_surface(L, surface);
}

static int texture_from_font(lua_State * L) {
	TTF_Font * font;
	SDL_Surface * surface;
	const char * text;
	
	font = get_font(L, 1);
	text = luaL_checkstring(L, 2);
	if (strlen(text) == 0) {
		lua_pushnil(L);
		return 1;
	}
	surface = TTF_RenderText_Blended(font, text, APP_WHITE);
	return texture_from_surface(L, surface);
}

static int destroy_texture(lua_State * L) {
	SDL_Texture ** ud;
	SDL_Texture * texture;
	
	ud = (SDL_Texture **) lua_touserdata(L, 1);
	if (ud == NULL) {
		fatal("destroy_texture called with bad argument");
	}
	texture = *ud;
	if (texture == NULL) {
		fatal("destroy_texture called with bad argument 2");
	}
	SDL_DestroyTexture(texture);
	return 0;
}

static int render_texture(lua_State * L) {
	SDL_Texture ** ud;
	SDL_Texture * texture;
	SDL_Rect src;
	SDL_Rect dst;
	
	ud = (SDL_Texture **) lua_touserdata(L, 1);
	if (ud == NULL) {
		fatal("render_texture called with bad argument");
	}
	texture = *ud;
	if (lua_gettop(L) == 5) {
	        dst.x = luaL_checknumber(L, 2);
	        dst.y = luaL_checknumber(L, 3);
		SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
	        SDL_RenderCopy(renderer, texture, NULL, &dst);
	} else {
        src.x = luaL_checknumber(L, 2);
        src.y = luaL_checknumber(L, 3);
		src.w = luaL_checknumber(L, 4);
		src.h = luaL_checknumber(L, 5);
        dst.x = luaL_checknumber(L, 6);
        dst.y = luaL_checknumber(L, 7);
		dst.w = luaL_checknumber(L, 8);
		dst.h = luaL_checknumber(L, 9);
        SDL_RenderCopy(renderer, texture, &src, &dst);
	}
	return 0;
}

void register_texture_functions(lua_State * L) {
	lua_register(L, "texture_from_file" , texture_from_file );
	lua_register(L, "texture_from_font" , texture_from_font );
	lua_register(L, "destroy_texture"   , destroy_texture   );
	lua_register(L, "render_texture"    , render_texture    );
}
