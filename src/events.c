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
	float scale_x;
	float scale_y;
	float offset_x;
	float offset_y;
	float aspect;
	float retina_factor;
	float x;
	float y;
	float gap;
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
			//x = x * drawable_w / (float) window_w;
			//y = y * drawable_h / (float) window_h;
			x = e->x * drawable_w / (float) window_w;
			y = e->y * drawable_h / (float) window_h;
		}

/*
		SDL_GetWindowSize(window, &window_w, &window_h);
		SDL_GL_GetDrawableSize(window, &drawable_w, &drawable_h);

		if (drawable_w > window_w) {
			retina_factor = 2.0;
		} else {
			retina_factor = 1.0;
		}
			
printf("mouse = %d %d \n", e->x, e->y);	
printf("window_w/h = %d %d \n", window_w, window_h);
printf("drawable_w/h = %d %d \n", drawable_w, drawable_h);

		if (app_fullscreen) {
			if (display_mode.w * design_height > design_width * display_mode.h) {
				// top and bottom letterboxing
				gap = window_h - design_height / retina_factor;
				y = (e->y + gap) * retina_factor;
				x = e->x * drawable_w / (float) window_w;
			} else {
				// left and right letterboxing
				gap = window_w - design_width / retina_factor;
				x = (e->x + gap) * retina_factor;
				y = e->y * drawable_h / (float) window_h;
			}
		} else {
			x = e->x * drawable_w / (float) window_w;
			y = e->y * drawable_h / (float) window_h;
		}

			aspect = display_mode.w / display_mode.h;
			if (display_mode.w * design_height > design_width * display_mode.h) {
				// top and bottom letterboxing
	//			scale_y = window_height / display_mode.h;
	//			offset_y = 
			}
				

		//	scale_x = drawable_w / (float) window_w * display_mode.w / (float) design_width;
		//	scale_y = drawable_h / (float) window_h * display_mode.h / (float) design_height;
		//	offset_x = (display_mode.w - design_width) / 2.0;
		//	offset_y = (display_mode.h - design_height) / 2.0;
		//	lua_pushinteger(L, e->x * scale_x + offset_x);
		//	lua_pushinteger(L, e->y * scale_y + offset_y);
			lua_pushinteger(L, e->x * drawable_w / (float) window_w);
			lua_pushinteger(L, e->y * drawable_h / (float) window_h);
		} else {
			lua_pushinteger(L, e->x * drawable_w / (float) window_w);
			lua_pushinteger(L, e->y * drawable_h / (float) window_h);
		}
*/
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

