local f = open_font('fonts/DroidSansMono.ttf', 28)

local t1 = texture_from_font(f, "Clické for next test.")
local t2 = texture_from_font(f, "Clické for next test.", 100, 255, 25)
local t3 = texture_from_font(f, "Clické for next test.", 100, 255, 25, 40)

local cara, w, h = texture_from_file('textures/Cara.png')

function on_update()
	render_texture(cara, 50, 50) 
	render_texture(t1, 50, 50) 
	render_texture(t2, 50, 80) 
	render_texture(t3, 50, 110) 
end

function on_touch()
	quit()
end

