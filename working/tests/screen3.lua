local f = open_font("fonts/DroidSansMono.ttf", 24)
local full = texture_from_font(f, "Press F for fullscreen")
local wind = texture_from_font(f, "Press ESC for windowed")

function on_touch(x, y)
	dofile('tests/screen4.lua')
end

function on_keydown(key)
	if     (key ==  27) then windowed() 
	elseif (key == 102) then fullscreen() end
end

function on_update()
	render_texture(full, 100, 100)
	render_texture(wind, 100, 150)
end

