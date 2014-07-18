#include "global.h"

bool process_event_queue(lua_State * L);
void on_render_device_reset(lua_State * L);
void on_render_targets_reset(lua_State * L);

bool		  running 	 = true;
SDL_Window	* window	 = NULL;
SDL_Renderer 	* renderer	 = NULL;
char		* resource_dir 	 = NULL;
char 		* save_dir 	 = NULL;
int		  design_width 	 = 16 * 50;
int		  design_height	 =  9 * 50;
int		  window_width   = 800;
int		  window_height  = 600;
bool		  app_fullscreen = false;
bool		  app_resizable  = false;
char 		* app_title 	 = "No Title Set";
SDL_DisplayMode   display_mode;

static int 	  	  app_millis_per_update = 1000 / 60;
static lua_State	* L;

void register_util_functions	(lua_State * L);
void register_texture_functions	(lua_State * L);
void register_audio_functions	(lua_State * L);
void register_font_functions	(lua_State * L);

static void register_global_functions(lua_State * L) {
	register_util_functions(L);
	register_texture_functions(L);
	register_audio_functions(L);
	register_font_functions(L);
}

static void set_package_path(lua_State * L, const char * path) {
	SDL_assert(lua_gettop(L) == 0);
	lua_getglobal(L, "package");
	lua_pushstring(L, "path");
	lua_pushstring(L, path);
	lua_settable(L, 1);
	lua_settop(L, 0);
}

/*
	Shutdown systems in the reverse order in which they were started.
*/
static void shutdown() {
	lua_close(L);
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

/*
int SDLCALL eventFilter(void * userdata, SDL_Event * event) {
	if (event->type == SDL_APP_LOWMEMORY) {
		lua_gc(L, LUA_GCCOLLECT, 0);
	} else if (event->type == SDL_APP_TERMINATING) {
		running = false;
	} else if (event->type == SDL_RENDER_DEVICE_RESET) {
		on_render_device_reset(L);
	} else if (event->type == SDL_RENDER_TARGETS_RESET) {
		on_render_targets_reset(L);
	} else {
		// This is not a special case event to handle, 
		// so return 1 to let it go onto the event queue.
		return 1;
	}
        return 0; // Return 0 to indicate event has been handled.
}
*/

// Get initialization setting from config.lua.
static void config() {
	char * base_path;
	const char * app_save_folder;

	if (luaL_dofile(L, resource_path("config.lua"))) fatal(luaL_checkstring(L, -1));

	// app_title
	lua_getglobal(L, "app_title");
	if (!lua_isnil(L, 1)) {
		app_title = SDL_strdup(luaL_checkstring(L, 1));
	}
	lua_settop(L, 0);

	// design_width
	lua_getglobal(L, "design_width");
	if (!lua_isnil(L, 1)) {
		design_width = luaL_checkinteger(L, 1);
	}
	lua_settop(L, 0);

	// design_height
	lua_getglobal(L, "design_height");
	if (!lua_isnil(L, 1)) {
		design_height = luaL_checkinteger(L, 1);
	}
	lua_settop(L, 0);

	// window_width
	lua_getglobal(L, "window_width");
	if (!lua_isnil(L, 1)) {
		window_width = luaL_checkinteger(L, 1);
	}
	lua_settop(L, 0);

	// window_height
	lua_getglobal(L, "window_height");
	if (!lua_isnil(L, 1)) {
		window_height = luaL_checkinteger(L, 1);
	}
	lua_settop(L, 0);

	// app_fullscreen
	if (!is_ios() && !is_android()) {
		lua_getglobal(L, "app_fullscreen");
		if (!lua_isnil(L, 1)) {
			app_fullscreen = lua_toboolean(L, 1);
		} else {
			app_fullscreen = false;
		}
		lua_settop(L, 0);
	}

	// app_resizable
//	if (!is_ios() && !is_android()) {
//		lua_getglobal(L, "app_resizable");
//		if (!lua_isnil(L, 1)) {
//			app_resizable = lua_toboolean(L, 1);
//		} else {
//			app_resizable = false;
//		}
//		lua_settop(L, 0);
//	}

	// save_dir
	lua_getglobal(L, "app_save_folder");
	if (!lua_isnil(L, 1)) {
		app_save_folder = luaL_checkstring(L, 1);
	} else {
		app_save_folder = app_title;
	}
	base_path = SDL_GetPrefPath("lua1", app_save_folder);
	save_dir = SDL_strdup(base_path);
	if (base_path) {
		SDL_free(base_path);
	} else {
		fatal("Writing save file not possible.");
	}
	lua_settop(L, 0);

	// app_millis_per_update
	lua_getglobal(L, "app_millis_per_update");
	if (!lua_isnil(L, 1)) {
		app_millis_per_update = luaL_checkinteger(L, 1);
	}
	lua_settop(L, 0);
}

/*
	Initialize SDL, SDL_ttf, SDL_image and Lua.
	Register the app's C functions with Lua.
	Call the app's config and init scripts.
*/
static void init() {
	char * base_path;
	int img_support;
	int window_flags;

	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		fatal(SDL_GetError());
	}

	// Determine the resource dir.
	base_path = SDL_GetBasePath();
	if (base_path) {
		resource_dir = SDL_strdup(base_path);
		SDL_free(base_path);
	} else {
		resource_dir = SDL_strdup("./");
	}

	L = luaL_newstate();
	if (!L) fatal("Can not create Lua state.");
	luaL_openlibs(L);
	set_package_path(L, resource_path("./?.lua"));
	SDL_assert(lua_gettop(L) == 0);

	config();

	// Place useful information into Lua global namespace.

	lua_pushstring(L, resource_dir);
	lua_setglobal(L, "resource_dir");
	lua_pushstring(L, save_dir);
	lua_setglobal(L, "save_dir");
	if (is_windows()) {
		lua_pushstring(L, "\\");
	} else {
		lua_pushstring(L, "/");
	}
	lua_setglobal(L, "path_separator");
	SDL_assert(lua_gettop(L) == 0);

	window_flags = SDL_WINDOW_OPENGL;

	if (is_ios()) {
		window_flags |= SDL_WINDOW_RESIZABLE; // required to change orientation
	} else if (is_android()) {
	} else if (is_osx()) { 
		window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
		if (app_fullscreen) window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	} else if (is_windows()) {
		if (app_fullscreen) window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	} else if (is_linux()) {
		if (app_fullscreen) window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	} else {
		fatal("Platform not supported.");
	}

	window = SDL_CreateWindow(
		app_title, 
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		window_flags);
	if (!window) fatal(SDL_GetError());

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) fatal(SDL_GetError());

        if (SDL_GetDisplayMode(0, 0, &display_mode) != 0) {
		fatal(SDL_GetError());
	}
	printf("display_mode = %d %d \n", display_mode.w, display_mode.h);

        if (SDL_GetDesktopDisplayMode(0, &display_mode) != 0) {
	printf("desktop_display_mode = %d %d \n", display_mode.w, display_mode.h);
		fatal(SDL_GetError());
	}

	if (SDL_RenderSetLogicalSize(renderer, design_width, design_height)) {
		fatal(SDL_GetError());
	}

	//SDL_AddEventWatch(eventFilter, NULL);

	if (TTF_Init()) fatal(TTF_GetError());

	SDL_assert(lua_gettop(L) == 0);
	register_global_functions(L);
	SDL_assert(lua_gettop(L) == 0);

	if (luaL_dofile(L, resource_path("init.lua"))) fatal(luaL_checkstring(L, -1));
	SDL_assert(lua_gettop(L) == 0);
}

/*
	If on_update is found in global scope, then call it. Otherwise do nothing.
*/
static void update() {
	SDL_assert(lua_gettop(L) == 0);
	lua_getglobal(L, "on_update");
	if (!lua_isnil(L, 1)) {
		if (lua_pcall(L, 0, 0, 0)) fatal(lua_tostring(L, -1));
	}
	lua_settop(L, 0);
}

/*
	Loop until either process_event_queue returns false
	or running gets set to false.  Pad each pass through 
	the main loop with a delay to achieve the desired 
	app_millis_per_update.
*/
void loop() {
	Uint32 update_start_time;
	Uint32 update_elapsed_time;

	SDL_assert(lua_gettop(L) == 0);
	while (running) {
		update_start_time = SDL_GetTicks();

		// Process all queued events;
		// quit loop when event processor returns false.
		SDL_assert(lua_gettop(L) == 0);
		if (!process_event_queue(L)) break;
		SDL_assert(lua_gettop(L) == 0);

		update();
		SDL_assert(lua_gettop(L) == 0);

		update_elapsed_time = SDL_GetTicks() - update_start_time;
		if (update_elapsed_time < app_millis_per_update) {
			SDL_Delay(app_millis_per_update - update_elapsed_time);
		} else {
			SDL_Delay(1);
		}
	}
}

int main(int argc, char * argv[]) {
	init();
	loop();
	shutdown();
	return 0;
}

