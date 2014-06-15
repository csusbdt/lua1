local f = open_font('fonts/DroidSansMono.ttf', 28)
local t1, t2, t3
local cara, w, h

function on_render_targets_reset()
	set_draw_color(0, 0, 0, 255)
	fill_rect(0, 0, app_width, app_height)
	render_texture(cara, 50, 50) 
	render_texture(t1, 50, 50) 
	render_texture(t2, 50, 80) 
	render_texture(t3, 50, 110) 
	render()
end

function on_device_reset()
	t1 = texture_from_font(f, "Clické for next test.")
	t2 = texture_from_font(f, "Clické for next test.", 100, 255, 25)
	t3 = texture_from_font(f, "Clické for next test.", 100, 255, 25, 40)
	cara, w, h = texture_from_file('textures/Cara.png')
	on_render_targets_reset()
end

function on_update()
end

function on_touch()
	dofile('tests/screen3.lua')
end

on_device_reset()

