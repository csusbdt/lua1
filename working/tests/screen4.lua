local cara, w, h = texture_from_file('textures/Cara.png')
local f = open_font('fonts/DroidSansMono.ttf', 28)
local t1 = texture_from_font(f, "Clické for next test.")
local t2 = texture_from_font(f, "Clické for next test.", 100, 255, 25)
local t3 = texture_from_font(f, "Clické for next test.", 100, 255, 25, 40)

function on_update()
	render_texture(cara, 50, 50) 
	render_texture(t1, 50, 50) 
	render_texture(t2, 50, 80) 
	render_texture(t3, 50, 110) 
end

function on_touch()
	quit()
end

function on_update()
	render_texture(cara)
	render_texture(cara, 100, 100)
	render_texture(cara, 200, 200, 32, 32)
	render_texture(cara, 50, 50, 100, 100, 300, 300)
	render_texture(cara, 50, 50, 100, 100, 400, 300, 32, 32)
	render_texture(t1, 20, 20)
	render_texture(t2, 20, 40)
	blendmode_blend()
	set_draw_color(255, 0, 0, 75)
	fill_rect(400, 220, 50, 75)
	blendmode_none()
	set_draw_color(255, 0, 0, 75)
	fill_rect(350, 220, 50, 75)
end

