#include "global.h"

static void on_window_closing(lua_State * L) {
	lua_getglobal(L, "on_window_closing");
	if (lua_isnil(L, 1)) return;
	if (lua_pcall(L, 0, 0, 0)) fatal(lua_tostring(L, -1));
}

static void on_mouse_down(lua_State * L, const SDL_MouseButtonEvent * e) {
	if (e->type == SDL_MOUSEBUTTONDOWN && e->button == SDL_BUTTON_LEFT) {
		SDL_assert(lua_gettop(L) == 0);
		lua_getglobal(L, "on_touch");
		SDL_assert(lua_gettop(L) == 1);
		if (lua_isnil(L, 1)) {
			lua_settop(L, 0);
			return;
		}
		lua_pushinteger(L, e->x);
		lua_pushinteger(L, e->y);
		if (lua_pcall(L, 2, 0, 0)) fatal(lua_tostring(L, -1));
	}
}

static void on_key_down(lua_State * L, const SDL_KeyboardEvent * e) {
	SDL_assert(lua_gettop(L) == 0);
	lua_getglobal(L, "on_keydown");
	SDL_assert(lua_gettop(L) == 1);
	if (lua_isnil(L, 1)) {
		lua_settop(L, 0);
		return;
	}
	lua_pushinteger(L, e->keysym.sym);
	if (lua_pcall(L, 1, 0, 0)) fatal(lua_tostring(L, -1));
/*
	if (e->keysym.sym == SDLK_r) {
		SDL_assert(lua_gettop(L) == 0);
		lua_getglobal(L, "on_keydown_r");
		SDL_assert(lua_gettop(L) == 1);
		if (lua_isnil(L, 1)) {
			lua_settop(L, 0);
			return;
		}
		if (lua_pcall(L, 0, 0, 0)) fatal(lua_tostring(L, -1));
	} else if (e->keysym.sym == SDLK_ESCAPE) {
		SDL_assert(lua_gettop(L) == 0);
		if (app_fullscreen) {
			windowed(L);
		}
	}
*/
}

bool process_event_queue(lua_State * L) {
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		if      (e.type == SDL_QUIT)          { on_window_closing(L);  return false; } 
		else if (e.type == SDL_MOUSEBUTTONDOWN) on_mouse_down(L, &e.button);
		else if (e.type == SDL_KEYDOWN)         on_key_down(L, &e.key);
	}
	lua_settop(L, 0);
	return true;
}

