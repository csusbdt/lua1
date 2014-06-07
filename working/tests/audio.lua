assert(not pcall(wave_from_file))
assert(not pcall(wave_from_file, 2))
assert(not pcall(wave_from_file, "2", 3))

local _, w = assert(pcall(wave_from_file, "waves/door.wav"))

assert(not pcall(play_wave, 1))
assert(not pcall(play_wave, w, 2))
assert(pcall(play_wave, w))

assert(not pcall(destroy_wave))
assert(not pcall(destroy_wave, 1))

local _, l = assert(pcall(loop_wave, w))

assert(not pcall(destroy_wave, 1))
assert(not pcall(destroy_wave, w, 2))
assert(pcall(destroy_wave, w))
assert(not pcall(destroy_wave, w))
assert(not pcall(play_wave, w))

assert(pcall(stop_loop, l))
assert(not pcall(loop_wave, w))

local _, m = assert(pcall(wave_from_file, "waves/Overworld.wav"))

