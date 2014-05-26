-- Test fonts.

local font1, err = open_font('fonts/I_dont_exist.ttf')
assert(not font1)
assert(err and type(err) == 'string')

font1, err = open_font('fonts/I_dont_exist.ttf', 24)
assert(not font1)
assert(err and type(err) == 'string')

font1, err = open_font(24, 'fonts/DroidSansMono.ttf')
assert(not font1)
assert(err and type(err) == 'string')

font1, err = open_font('fonts/DroidSansMono.ttf', 'not a number')
assert(not font1)
assert(err and type(err) == 'string')

--close_font() -- This opens a popup from the engine side.
--close_font('not userdata') -- This opens a popup from the engine side.

font1, err = open_font('fonts/DroidSansMono.ttf', 24)
assert(font1 and type(font1) == 'userdata')
assert(not err)

font2, err = open_font('fonts/DroidSansMono.ttf', 24)
assert(font2 and type(font2) == 'userdata')
assert(not err)
assert(font1 ~= font2)
close_font(font2)

-- Test textures.

local texture1, err = texture_from_file(1, 2)
assert(texture1 == nil and type(err) == 'string')

texture1, err = texture_from_file(1)
assert(texture1 == nil and type(err) == 'string')

texture1, err = texture_from_file('textures/I_dont_exist.png')
assert(texture1 == nil and type(err) == 'string')

texture1, w, h = texture_from_file('textures/Cara.png')
assert(type(texture1) == 'userdata' and type(w) == 'number' and type(h) == 'number')

local texture2 = texture_from_file('textures/Cara.png')
assert(type(texture2) == 'userdata' and texture1 ~= texture2)

err = destroy_texture(texture2)
assert(err == nil)

err = destroy_texture('Not a texture')
assert(type(err) == 'string')

err = render_texture(texture1)
assert(err == nil)
render_texture(texture1, 100, 100)
render_texture(texture1, 200, 200, 32, 32)
render_texture(texture1, 50, 50, 100, 100, 300, 300)
render_texture(texture1, 50, 50, 100, 100, 400, 300, 32, 32)

render_present()

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
	render_present()
end

