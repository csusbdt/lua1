--[[
assert(not pcall(open_font, 'fonts/DroidSansMono.ttf'))
assert(not pcall(open_font, 'fonts/DroidSansMono.ttf', 'not a number'))
assert(not pcall(open_font, 'fonts/I_dont_exist.ttf', 24))

local _, font = assert(pcall(open_font, 'fonts/DroidSansMono.ttf', 24))
--]]
local f = open_font('fonts/DroidSansMono.ttf', 24)

assert(not pcall(texture_from_file, 1, 2))
assert(not pcall(texture_from_file, 1))
assert(not pcall(texture_from_file, 'textures/I_dont_exist.png'))
assert(not pcall(texture_from_file, 'waves/Overworld.wav'))

local _, cara, w, h = assert(pcall(texture_from_file, 'textures/Cara.png'))
assert(type(cara) == 'userdata' and type(w) == 'number' and type(h) == 'number')

-- Demonstrate that loading the same iamge file results in waste.
cara2 = texture_from_file('textures/Cara.png')
assert(type(cara2) == 'userdata' and cara ~= cara2)

assert(not pcall(destroy_texture, 1, 2))
assert(not pcall(destroy_texture, 1))
assert(pcall(destroy_texture, cara2))
assert(not pcall(destroy_texture, t2))

assert(not pcall(texture_from_font, 1))
assert(not pcall(texture_from_font, 1, 2))
assert(not pcall(texture_from_font, f, 2))
assert(not pcall(texture_from_font, f, "x", "a", "b", "c"))
assert(not pcall(texture_from_font, f, "x", 3, 4, 5, 6, 7))
assert(not pcall(texture_from_font, f, "x", 3, 4, 5, "x"))
assert(not pcall(texture_from_font, f, ""))

local _, t1 = assert(pcall(texture_from_font, f, "Click for next test."))
local _, t2 = assert(pcall(texture_from_font, f, "Click for next test.", 100, 255, 25))
local _, t3 = assert(pcall(texture_from_font, f, "Click for next test.", 100, 255, 25, 40))

function on_update()
	render_texture(cara, 50, 50) 
	render_texture(t1, 50, 50) 
	render_texture(t2, 50, 80) 
	render_texture(t3, 50, 110) 
end

