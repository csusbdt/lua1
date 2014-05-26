-- Test audio.

local wave1, err = wave_from_file('waves/I_dont_exist.wav')
assert(not wave);
assert(err and type(err) == 'string')

wave1, err = wave_from_file('I_dont_exist/Overworld.wav')
assert(not wave);
assert(err and type(err) == 'string')

wave1, err = wave_from_file('waves/Overworld.wav')
assert(wave1);
assert(not err);

local wave1_i, err = loop_wave(wave1)
assert(wave1_i)
assert(not err)

local wave2_i, err = loop_wave(nil)
assert(not wave2_i)
assert(err)

-- Test on touch event.

function on_touch(x, y)
	print('x = ' .. x .. '  y = ' .. y)
end

