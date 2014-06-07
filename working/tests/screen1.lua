local f = open_font('fonts/DroidSansMono.ttf', 36)

local s = "Clické for next test."

local w, h = font_texture_size(f, s)

local t1 = texture_from_font(f, s)
local t2 = texture_from_font(f, s, 100, 255, 25)
local t3 = texture_from_font(f, s, 100, 255, 25, 40)

local cara, w, h = texture_from_file('textures/Cara.png')

texture_blendmode_blend(t2)
texture_blendmode_blend(t3)

function on_update()
	render_texture(cara, 50, 50) 
	render_texture(t1, 50, 50) 
	render_texture(t2, 50, 80) 
	render_texture(t3, 50, 110) 
end

function on_touch()
	dofile('tests/screen2.lua')
end

