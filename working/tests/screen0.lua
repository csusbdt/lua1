local name_text = "CECILIA"
--local name_font = open_font('fonts/CaviarDreams.ttf', 60)
local name_font = open_font('fonts/DroidSansMono.ttf', 46)

local main_texture
local main_texture_w
local main_texture_h

local name_texture
local name_texture_w
local name_texture_h

--local w, h = font_texture_size(f, s)

function load_textures()
	main_texture, main_texture_w, main_texture_h 
		= texture_from_file('textures/UI-main.png')
	name_texture, name_texture_w, name_texture_h 
		= texture_from_font(name_font, name_text, 255, 255, 255, 255)
end

local function draw()
	set_draw_color(0, 0, 0, 255)
	render_clear()
	local x = 0
	local y = 400
	render_texture(main_texture, x, y)
	x = x + 956 - name_texture_w / 2
	y = y + 104 - name_texture_h / 2
	render_texture(name_texture, x, y) 
	render_texture(name_texture, 0, 0) 
	render()
end

function on_render_targets_reset()
	draw()
end

function on_render_device_reset()
	load_textures()
	draw()
end

function on_update()
end

function on_touch()
	if not app_fullscreen then 
		fullscreen()
		app_fullscreen = true
	else
		windowed()
		app_fullscreen = false
		dofile('tests/screen1.lua')
	end
end

load_textures()
--set_window_size(2560 / 3.2, 1440 / 3.2)
draw()
set_window_position(10, 10)

