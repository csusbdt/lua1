#include "global.h"

extern int design_width;
extern int design_height;
extern SDL_DisplayMode display_mode;

static void on_window_closing(lua_State * L) {
	lua_getglobal(L, "on_window_closing");
	if (lua_isnil(L, 1)) return;
	if (lua_pcall(L, 0, 0, 0)) fatal(lua_tostring(L, -1));
}

void on_render_device_reset(lua_State * L) { // Called from main.
	lua_getglobal(L, "on_device_reset");
	if (lua_isnil(L, 1)) return;
	if (lua_pcall(L, 0, 0, 0)) fatal(lua_tostring(L, -1));
}

void on_render_targets_reset(lua_State * L) { // Called from main.
	lua_getglobal(L, "on_render_targets_reset");
	if (lua_isnil(L, 1)) return;
	if (lua_pcall(L, 0, 0, 0)) fatal(lua_tostring(L, -1));
}

static void on_mouse_down(lua_State * L, const SDL_MouseButtonEvent * e) {
	int window_w;
	int window_h;
	int drawable_w;
	int drawable_h;
	float x;
	float y;
	SDL_DisplayMode mode;

	if (e->type == SDL_MOUSEBUTTONDOWN && e->button == SDL_BUTTON_LEFT) {
		SDL_assert(lua_gettop(L) == 0);
		lua_getglobal(L, "on_touch");
		SDL_assert(lua_gettop(L) == 1);
		if (lua_isnil(L, 1)) {
			lua_settop(L, 0);
			return;
		}

		SDL_GetWindowSize(window, &window_w, &window_h);
		SDL_GL_GetDrawableSize(window, &drawable_w, &drawable_h);

		x = 2 * e->x;
		y = 2 * e->y;

		if (app_fullscreen) {
			if (drawable_w > window_w) {
				x = x + (display_mode.w * 2 - design_width) / 2.0;
				y = y + (display_mode.h * 2 - design_height) / 2.0;
			} else {
				x = e->x;
				y = e->y;
			}
		} else {
			x = e->x * drawable_w / (float) window_w;
			y = e->y * drawable_h / (float) window_h;
		}

		lua_pushinteger(L, x);
		lua_pushinteger(L, y);
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
}

static void on_window_event(lua_State * L, Uint8 id) {
	SDL_assert(lua_gettop(L) == 0);
	if (id == SDL_WINDOWEVENT_EXPOSED) {
		lua_getglobal(L, "on_window_exposed");
		if (lua_isnil(L, 1)) {
			lua_settop(L, 0);
			return;
		}
		if (lua_pcall(L, 0, 0, 0)) fatal(lua_tostring(L, -1));
	} else if (id == SDL_WINDOWEVENT_SIZE_CHANGED) {
		lua_getglobal(L, "on_window_size_changed");
		if (lua_isnil(L, 1)) {
			lua_settop(L, 0);
			return;
		}
		if (lua_pcall(L, 0, 0, 0)) fatal(lua_tostring(L, -1));
	} else if (id == SDL_WINDOWEVENT_RESTORED) {
		lua_getglobal(L, "on_window_restored");
		if (lua_isnil(L, 1)) {
			lua_settop(L, 0);
			return;
		}
		if (lua_pcall(L, 0, 0, 0)) fatal(lua_tostring(L, -1));
	}
}

bool process_event_queue(lua_State * L) {
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		if      (e.type == SDL_QUIT)               { on_window_closing(L);  return false; } 
		else if (e.type == SDL_MOUSEBUTTONDOWN)      on_mouse_down(L, &e.button);
		else if (e.type == SDL_KEYDOWN)              on_key_down(L, &e.key);
		else if (e.type == SDL_WINDOWEVENT)          on_window_event(L, e.window.event);
		else if (e.type == SDL_RENDER_DEVICE_RESET)  on_render_device_reset(L);
		else if (e.type == SDL_RENDER_TARGETS_RESET) on_render_targets_reset(L);
	}
	lua_settop(L, 0);
	return true;
}

