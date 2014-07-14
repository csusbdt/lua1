local name_text = "CECILIA"
local name_font = open_font('fonts/CaviarDreams.ttf', 36)

local main_texture
local main_texture_w
local main_texture_h

local diamond_texture
local diamond_texture_w
local diamond_texture_h

local next_texture
local next_texture_w
local next_texture_h

local name_texture
local name_texture_w
local name_texture_h

local function load_textures()
	main_texture, main_texture_w, main_texture_h 
		= texture_from_file('textures/UI-main.png')
	diamond_texture, diamond_texture_w, diamond_texture_h 
		= texture_from_file('textures/UI-diamond-black.png')
	next_texture, next_texture_w, next_texture_h 
		= texture_from_file('textures/UI-button-next.png')
	name_texture, name_texture_w, name_texture_h 
		= texture_from_font(name_font, name_text, 255, 255, 255, 255)
end

local function draw()
	set_draw_color(125, 125, 125, 255)
	render_clear()
	render_texture(main_texture, 0, 312)
	render_texture(next_texture, design_width - next_texture_w * 2, 312 + 132)
	render_texture(diamond_texture, 400 - diamond_texture_w/2, 300 - diamond_texture_h/2) 
	render_texture(diamond_texture, 800 - diamond_texture_w, 600 - diamond_texture_h) 
	render_texture(diamond_texture, 1024 - diamond_texture_w, 768 - diamond_texture_h) 
	render_texture(name_texture, 385 - name_texture_w / 2, 353 - name_texture_h / 2) 
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
draw()
set_window_position(10, 10)

