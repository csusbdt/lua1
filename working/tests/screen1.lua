local f = open_font('fonts/DroidSansMono.ttf', 26)

local s = "Clické for next test."

local w, h = font_texture_size(f, s)

local t1 = texture_from_font(f, s)
local t2 = texture_from_font(f, s, 100, 255, 25)
local t3 = texture_from_font(f, s, 100, 255, 25, 40)
local w, h = font_texture_size(f, s)

local cara, cw, ch = texture_from_file('textures/Cara.png')
local jpg, jw, jh = texture_from_file('textures/nature-wallpaper-1.jpg')

--texture_alpha_mod(cara, 100)
texture_alpha_mod(t1, 100)
texture_alpha_mod(t2, 100)
texture_alpha_mod(t3, 100)

function on_update()
	local y = 50
	render_texture(jpg) 
	render_texture(cara, 70, 50) 
	render_texture(t1, 50, y); y = y + h
	render_texture(t2, 50, y); y = y + h
	render_texture(t3, 50, y) 
end

function on_touch()
	dofile('tests/screen2.lua')
end

