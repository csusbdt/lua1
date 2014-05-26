#include "global.h"

#define SDL_INIT_FLAGS (SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS)
#define DESIRED_MILLIS_PER_FRAME (1000 / 60)
#define RENDERER_INIT_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)

bool process_event_queue(lua_State * L);

bool running = true;
SDL_Renderer * renderer;
char * resource_dir = NULL;
char * save_dir = NULL;
int app_width = 16 * 50;
int app_height = 9 * 50;
char * app_title = "No Title Set";

static lua_State  * L;
static SDL_Window * window;

void register_util_functions(lua_State * L);
void register_texture_functions(lua_State * L);
void register_audio_functions(lua_State * L);
void register_font_functions(lua_State * L);

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

static void clear_canvas() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
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

int SDLCALL eventFilter(void * userdata, SDL_Event * event)
{
	if (event->type == SDL_APP_LOWMEMORY) {
		lua_gc(L, LUA_GCCOLLECT, 0);
	} else if (event->type == SDL_APP_TERMINATING) {
		running = false;
	} else {
		// Not a special event to handle, so let it go onto the event queue.
		return 1;
	}
        return 0;
}

/*
	Initialize SDL, SDL_ttf and Lua.
	Register the app's C functions with Lua.
	Call the app's init script.

	The main purpose of the init script is to define the following
	global functions:
		on_draw
		on_touch
		on_quit
*/
static void init() {
	char * base_path;
	int img_support;

	if (SDL_Init(SDL_INIT_FLAGS)) fatal(SDL_GetError());

	img_support = IMG_INIT_PNG | IMG_INIT_JPG;
	if ((IMG_Init(img_support) & img_support) != img_support) {
		fatal("Required image support missing.");
	}

	// Determine the resource dir.
	base_path = SDL_GetBasePath();
	if (base_path) {
		resource_dir = SDL_strdup(base_path);
		SDL_free(base_path);
	} else {
		resource_dir = SDL_strdup("./");
	}

	// Determine the save dir.
	base_path = SDL_GetPrefPath("lua1", app_title);
	if (base_path) {
		save_dir = SDL_strdup(base_path);
		SDL_free(base_path);
	} else {
		fatal("Writing save file not possible.");
	}

	L = luaL_newstate();
	if (!L) fatal("Can not create Lua state.");

	luaL_openlibs(L);

	set_package_path(L, resource_path("./?.lua"));

	SDL_assert(lua_gettop(L) == 0);
	if (luaL_dofile(L, resource_path("config.lua"))) fatal(luaL_checkstring(L, -1));
	lua_getglobal(L, "app_width");
	if (!lua_isnil(L, 1)) {
		app_width = luaL_checkinteger(L, 1);
	}
	lua_settop(L, 0);
	lua_getglobal(L, "app_height");
	if (!lua_isnil(L, 1)) {
		app_height = luaL_checkinteger(L, 1);
	}
	lua_settop(L, 0);
	lua_getglobal(L, "app_title");
	if (!lua_isnil(L, 1)) {
		app_title = SDL_strdup(luaL_checkstring(L, 1));
	}
	lua_settop(L, 0);

	window = SDL_CreateWindow(
		app_title, 
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		app_width,
		app_height,
		SDL_WINDOW_OPENGL);
	if (!window) fatal(SDL_GetError());

	renderer = SDL_CreateRenderer(window, -1, RENDERER_INIT_FLAGS);
	if (!renderer) fatal(SDL_GetError());
    
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	if (SDL_RenderSetLogicalSize(renderer, app_width, app_height)) {
		fatal(SDL_GetError());
	}

	SDL_AddEventWatch(eventFilter, NULL);

	clear_canvas();

	if (TTF_Init()) fatal(TTF_GetError());

	SDL_assert(lua_gettop(L) == 0);
	register_global_functions(L);
	SDL_assert(lua_gettop(L) == 0);

	if (luaL_dofile(L, resource_path("init.lua"))) fatal(luaL_checkstring(L, -1));
	SDL_assert(lua_gettop(L) == 0);
}

/*
	The draw function is called after the canvas has been cleared to black
	and before the next call to present the canvas, so this is the time for
	Lua scripts to draw to the canvas.

	If on_draw is found in global scope, then call it. Otherwise do nothing.
*/
static void draw() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_assert(lua_gettop(L) == 0);
	lua_getglobal(L, "on_draw");
	SDL_assert(lua_gettop(L) == 1);
        if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		SDL_assert(lua_gettop(L) == 0);
		return;
	}
	if (lua_pcall(L, 0, 0, 0)) {
		fatal(lua_tostring(L, -1));
	}
	SDL_assert(lua_gettop(L) == 0);
	SDL_RenderPresent(renderer);
}

/*
	Pad each pass through the main loop with a delay
	to achieve the DESIRED_MILLIS_PER_FRAME.

	Loop until either process_event_queue returns false
	or running gets set to false.
*/
void loop() {
	Uint32 frame_start_time;
	Uint32 frame_elapsed_time;

	SDL_assert(lua_gettop(L) == 0);
	while (running) {
		frame_start_time = SDL_GetTicks();

		// Process all queued events;
		// quit loop when event processor returns false.
		SDL_assert(lua_gettop(L) == 0);
		if (!process_event_queue(L)) break;
		SDL_assert(lua_gettop(L) == 0);

		draw();
		SDL_assert(lua_gettop(L) == 0);

		// Add delay to achieve DESIRED_MILLIS_PER_FRAME;
		// delay at least 1 milliseconds to let any waiting threads run.
		frame_elapsed_time = SDL_GetTicks() - frame_start_time;
		if (frame_elapsed_time < DESIRED_MILLIS_PER_FRAME) {
			SDL_Delay(DESIRED_MILLIS_PER_FRAME - frame_elapsed_time);
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

