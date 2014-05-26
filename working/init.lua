-- Test fonts.

local f, f2, t, t2, w, w2, wi, w2i

f, err = open_font('fonts/I_dont_exist.ttf')
assert(not f and type(err) == 'string')

f, err = open_font('fonts/I_dont_exist.ttf', 24)
assert(not f and type(err) == 'string')

f, err = open_font(24, 'fonts/DroidSansMono.ttf')
assert(not f and type(err) == 'string')

f, err = open_font('fonts/DroidSansMono.ttf', 'not a number')
assert(not f and type(err) == 'string')

--close_font() -- This opens a popup from the engine side.
--close_font('not userdata') -- This opens a popup from the engine side.

f, err = open_font('fonts/DroidSansMono.ttf', 24)
assert(type(f) == 'userdata' and not err)

f2, err = open_font('fonts/DroidSansMono.ttf', 24)
assert(type(f2) == 'userdata')
assert(f ~= f2)
close_font(f2)

-- Test textures.

t, err = texture_from_file(1, 2)
assert(not t and type(err) == 'string')

t, err = texture_from_file(1)
assert(not t and type(err) == 'string')

t, err = texture_from_file('textures/I_dont_exist.png')
assert(not t and type(err) == 'string')

t, w, h = texture_from_file('textures/Cara.png')
assert(type(t) == 'userdata' and type(w) == 'number' and type(h) == 'number')

t2 = texture_from_file('textures/Cara.png')
assert(type(t2) == 'userdata' and t~= t2)

err = destroy_texture(t2)
assert(err == nil)

err = destroy_texture('Not a texture')
assert(type(err) == 'string')

err = render_texture(t)
assert(err == nil)
render_texture(t, 100, 100)
render_texture(t, 200, 200, 32, 32)
render_texture(t, 50, 50, 100, 100, 300, 300)
render_texture(t, 50, 50, 100, 100, 400, 300, 32, 32)

render_texture(
	texture_from_font(f, "Testing 1 2 3 ..."),
	20, 20
)

-- Test audio.

local w, err = wave_from_file('waves/I_dont_exist.wav')
assert(not w and type(err) == 'string')

w, err = wave_from_file('I_dont_exist/Overworld.wav')
assert(not w and type(err) == 'string')

w, err = wave_from_file('waves/Overworld.wav')
assert(w and not err);

wi, err = loop_wave(w)
assert(wi and not err)

w2i, err = loop_wave(nil)
assert(not w2i and err)

-- Test on touch event.

function on_touch(x, y)
	print('x = ' .. x .. '  y = ' .. y)
	render_present()
end

render_present()

