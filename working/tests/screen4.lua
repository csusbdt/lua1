local f = open_font('fonts/DroidSansMono.ttf', 28)
local cara, w, h
local t1, t2, t3

function on_render_targets_reset()
	set_draw_color(0, 0, 0, 255)
	render_clear()
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
	render()
end

function on_render_device_reset()
	cara, w, h = texture_from_file('textures/Cara.png')
	t1 = texture_from_font(f, "Clické for next test.")
	t2 = texture_from_font(f, "Clické for next test.", 100, 255, 25)
	t3 = texture_from_font(f, "Clické for next test.", 100, 255, 25, 40)
	on_render_targets_reset()
end

function on_update()
end

function on_touch()
	quit()
end

on_render_device_reset()

