dofile('tests/fonts.lua')
dofile('tests/textures.lua')
dofile('tests/audio.lua')
dofile('tests/screen1.lua')


--[[

local testing123 = texture_from_font(f, "Press F for fullscreen")
local testing456 = texture_from_font(f, "Press ESC for windowed", 0, 0, 0, 255)

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
	msgbox('hi')
end

function on_keydown(key)
	if     (key ==  27) then windowed() 
	elseif (key == 102) then fullscreen() end
end

function on_update()
	render_texture(t)
	render_texture(t, 100, 100)
	render_texture(t, 200, 200, 32, 32)
	render_texture(t, 50, 50, 100, 100, 300, 300)
	render_texture(t, 50, 50, 100, 100, 400, 300, 32, 32)
	render_texture(testing123, 20, 20)
	render_texture(testing456, 20, 40)
	blendmode_blend()
	set_draw_color(255, 0, 0, 75)
	fill_rect(400, 220, 50, 75)
	blendmode_none()
	set_draw_color(255, 0, 0, 75)
	fill_rect(350, 220, 50, 75)
end
--]]

