#include "global.h"

#define MAX_AUDIO_SOURCES  4
#define NO_INDEX          -1

typedef Sint32 AudioSample;

typedef struct {
	AudioSample * buf;  // audio samples
	int len;            // number of samples
	int refs;           // reference count
} Wave;

typedef struct {
	Wave * wave;
	int    next_sample;
} PlayInstance;

typedef struct {
	Wave * wave;
	int    next_sample;
} LoopInstance;

static SDL_AudioDeviceID dev;
static SDL_AudioSpec app_audio_spec;
static int audio_sources = 0;

static PlayInstance play_instances[MAX_AUDIO_SOURCES];
static LoopInstance loop_instances[MAX_AUDIO_SOURCES];

static void release_wave(Wave * wave) {
	SDL_assert(wave->refs > 0);
	if (--wave->refs == 0) {
		SDL_free(wave->buf);
		SDL_free(wave);
	}
}

static int fill_empty_play_instance(Wave * wave) {
	int i;
	
	for (i = 0; i < MAX_AUDIO_SOURCES; ++i) {
		if (play_instances[i].wave == NULL) {
			play_instances[i].wave = wave;
			play_instances[i].next_sample = 0;
			++audio_sources;
			++wave->refs;
			return i;
		}
	}
	return NO_INDEX;
}

static int fill_empty_loop_instance(Wave * wave) {
	int i;
	
	for (i = 0; i < MAX_AUDIO_SOURCES; ++i) {
		if (loop_instances[i].wave == NULL) {
			loop_instances[i].wave = wave;
			loop_instances[i].next_sample = 0;
			++audio_sources;
			++wave->refs;
			return i;
		}
	}
	return NO_INDEX;
}

static void lock_audio() {
	SDL_LockAudioDevice(dev);
}

static void unlock_audio() {
	SDL_UnlockAudioDevice(dev);
}

static void reset_play_instance(int i) {
	SDL_assert(play_instances[i].wave != NULL);
	release_wave(play_instances[i].wave);
	play_instances[i].wave = NULL;
	play_instances[i].next_sample = 0;
	SDL_assert(audio_sources > 0);
	--audio_sources;
}

static void reset_loop_instance(int i) {
	SDL_assert(loop_instances[i].wave != NULL);
	release_wave(loop_instances[i].wave);
	loop_instances[i].wave = NULL;
	loop_instances[i].next_sample = 0;
	SDL_assert(audio_sources > 0);
	--audio_sources;
}

static void render_play_instance(int i, AudioSample * buf, int len) {
	Wave * wave;
	int s;
	
	wave = play_instances[i].wave;
	for (s = 0; s < len; ++s) {
		buf[s] += wave->buf[play_instances[i].next_sample++];
		if (play_instances[i].next_sample == wave->len) {
			reset_play_instance(i);
			return;
		}
	}
}

static void render_loop_instance(int i, AudioSample * buf, int len) {
	Wave * wave;
	int s;
	
	wave = loop_instances[i].wave;
	for (s = 0; s < len; ++s) {
		buf[s] += wave->buf[loop_instances[i].next_sample++];
		if (loop_instances[i].next_sample == wave->len) {
			loop_instances[i].next_sample = 0;
		}
	}
}

static int wave_from_file(lua_State * L) {
	const char * filename;
	SDL_AudioSpec   wav_spec;
	Uint32          wav_length;
	Uint8         * wav_buffer;
	SDL_AudioCVT cvt;
	AudioSample * buf;
	int len;
	int i;
	Wave * wave;
	Wave ** ud;
	SDL_RWops * file;
	
	filename = luaL_checkstring(L, 1);

	// Load the audio samples.
	file = SDL_RWFromFile(resource_path(filename), "rb");
	if (!file) {
		lua_settop(L, 0);
		lua_pushnil(L);
		lua_pushstring(L, SDL_GetError());
		return 2;
	}
	if (SDL_LoadWAV_RW(file, 1, &wav_spec, &wav_buffer, &wav_length) == NULL) {
		lua_settop(L, 0);
		lua_pushnil(L);
		lua_pushstring(L, SDL_GetError());
		return 2;
	}

	// Convert audio samples to the application's audio specifications.
	SDL_BuildAudioCVT(&cvt, 
		wav_spec.format       , wav_spec.channels       , wav_spec.freq, 
		app_audio_spec.format , app_audio_spec.channels , app_audio_spec.freq);
	cvt.len = wav_length;
	cvt.buf = (Uint8 *) SDL_malloc(cvt.len * cvt.len_mult);
	memcpy(cvt.buf, wav_buffer, wav_length);
	SDL_FreeWAV(wav_buffer);
	SDL_ConvertAudio(&cvt);

	// Reduce the amplitude of the samples to ensure that mixing the maximum number
	// of samples does not result in clipping. 
	buf = (AudioSample *) cvt.buf;
	len = cvt.len_cvt / sizeof(AudioSample);
	for (i = 0; i < len; ++i) {
		buf[i] /= MAX_AUDIO_SOURCES;
	}

	wave = (Wave *) SDL_malloc(sizeof(Wave));
	wave->len = len;
	wave->buf = buf;
	wave->refs = 1;
	ud = (Wave **) lua_newuserdata(L, sizeof(Wave *));
	*ud = wave;

	return 1;
}

static int destroy_wave(lua_State * L) {
	Wave ** ud;
	Wave * wave;
	
	ud = (Wave **) lua_touserdata(L, 1);
	if (ud == NULL) {
		fatal("destroy_wave called with bad argument");
	}
	wave = *ud;
	if (wave == NULL) {
		fatal("destroy_wave called with bad argument 2");
	}
	release_wave(wave);
	return 0;
}

static int play_wave(lua_State * L) {
	Wave ** ud;
	Wave * wave;
	
	ud = (Wave **) lua_touserdata(L, 1);
	if (ud == NULL) {
		fatal("play_wave called with bad argument");
	}
	wave = *ud;
	if (wave == NULL) {
		fatal("play_wave called with bad argument 2");
	}
	lock_audio();
	fill_empty_play_instance(wave);
	unlock_audio();
	return 0;
}

static int loop_wave(lua_State * L) {
	Wave ** ud;
	Wave * wave;
	int i;
	
	ud = (Wave **) lua_touserdata(L, 1);
	if (ud == NULL) {
		lua_settop(L, 0);
		lua_pushnil(L);
		lua_pushstring(L, "loop_wave called with bad argument");
		return 2;
	}
	wave = *ud;
	if (wave == NULL) {
		fatal("loop_wave called with bad argument 2");
	}
	lock_audio();
	i = fill_empty_loop_instance(wave);
	unlock_audio();
	if (i == NO_INDEX) return 0;
	lua_pushinteger(L, i);
	return 1;
}

static int stop_loop(lua_State * L) {
	int i;
	
	i = luaL_checknumber(L, 1);
	lock_audio();
	reset_loop_instance(i);
	unlock_audio();
	return 0;
}

static void render_sources(AudioSample * buf, int len) {
	int i;
	
	for (i = 0; i < MAX_AUDIO_SOURCES; ++i) {
		if (play_instances[i].wave != NULL) {
			render_play_instance(i, buf, len);
		}
		if (loop_instances[i].wave != NULL) {
			render_loop_instance(i, buf, len);
		}
	}
}

static void sdl_audio_callback(void * userdata, Uint8 * stream, int len) {
	AudioSample * sample_buf;
	int num_samples;
	
	memset(stream, app_audio_spec.silence, len);
	sample_buf = (AudioSample *) stream;
	num_samples = len / sizeof(AudioSample);
	render_sources(sample_buf, num_samples);
}

void shutdown_audio() {
	SDL_CloseAudioDevice(dev);
}

void register_audio_functions(lua_State * L) {
	SDL_AudioSpec want;

	// Initialize for playing audio.

	memset(play_instances, 0, sizeof(PlayInstance) * MAX_AUDIO_SOURCES);
	memset(loop_instances, 0, sizeof(LoopInstance) * MAX_AUDIO_SOURCES);

	SDL_zero(want);
	want.freq      = 44100;
	want.format    = AUDIO_S32LSB; 
	want.channels  = 1;
	want.samples   = 4096;
	want.callback  = sdl_audio_callback;

	dev = SDL_OpenAudioDevice(NULL, 0, &want, &app_audio_spec, 0);
	if (dev == 0) fatal(SDL_GetError());

	SDL_PauseAudioDevice(dev, 0); 

	// Now register Lua functions.

	lua_register(L, "wave_from_file" , wave_from_file );
	lua_register(L, "destroy_wave"   , destroy_wave   );
	lua_register(L, "play_wave"      , play_wave      );
	lua_register(L, "loop_wave"      , loop_wave      );
	lua_register(L, "stop_loop"      , stop_loop      );
}
