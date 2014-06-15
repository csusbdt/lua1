local f = open_font("fonts/DroidSansMono.ttf", 24)

local full, wind

function on_render_targets_reset()
	set_draw_color(0, 0, 0, 255)
	fill_rect(0, 0, app_width, app_height)
	render_texture(full, 100, 100)
	render_texture(wind, 100, 150)
	render()
end

function on_device_reset()
	full = texture_from_font(f, "Press F for fullscreen")
	wind = texture_from_font(f, "Press ESC for windowed")
	on_render_targets_reset()
end

function on_touch(x, y)
	dofile('tests/screen4.lua')
end

function on_keydown(key)
	if     (key ==  27) then windowed() 
	elseif (key == 102) then fullscreen() end
end

function on_update()
end

on_device_reset()

