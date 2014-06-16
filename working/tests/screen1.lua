local s = "Clické for next test."
local f = open_font('fonts/DroidSansMono.ttf', 26)
local w, h = font_texture_size(f, s)
local t1, t2, t3
local cara, cw, ch
local jpg, jw, jh

function on_render_targets_reset()
	render_texture(jpg) 
	render_texture(cara, 70, 50) 
	local y = 50
	render_texture(t1, 50, y); y = y + h
	render_texture(t2, 50, y); y = y + h
	render_texture(t3, 50, y) 
	render()
end

function on_render_device_reset()
	t1 = texture_from_font(f, s)
	t2 = texture_from_font(f, s, 100, 255, 25)
	t3 = texture_from_font(f, s, 100, 255, 25, 40)
	cara, cw, ch = texture_from_file('textures/Cara.png')
	jpg, jw, jh = texture_from_file('textures/nature-wallpaper-1.jpg')
	texture_alpha_mod(t1, 100)
	texture_alpha_mod(t2, 100)
	texture_alpha_mod(t3, 100)
	on_render_targets_reset()
end

function on_update()
end

function on_touch()
	dofile('tests/screen2.lua')
end

on_render_device_reset()

