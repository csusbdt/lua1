black = { r = 0, g = 0, b = 0 }

function on_window_size_changed()
	if on_render_targets_reset then on_render_targets_reset() end
end

dofile('tests/fonts.lua')
dofile('tests/textures.lua')
dofile('tests/audio.lua')
dofile('tests/screen.lua')

