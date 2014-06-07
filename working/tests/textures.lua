local cara, w, h = texture_from_file('textures/Cara.png')
assert(type(cara) == 'userdata' and type(w) == 'number' and type(h) == 'number')

assert(not pcall(texture_from_file, 1, 2))
assert(not pcall(texture_from_file, 1))
assert(not pcall(texture_from_file, 'textures/I_dont_exist.png'))
assert(not pcall(texture_from_file, 'waves/Overworld.wav'))

-- Demonstrate that loading the same image file results in waste.
cara2 = texture_from_file('textures/Cara.png')
assert(type(cara2) == 'userdata' and cara ~= cara2)

assert(not pcall(destroy_texture, 1, 2))
assert(not pcall(destroy_texture, 1))
assert(pcall(destroy_texture, cara2))
assert(not pcall(destroy_texture, cara2))

local _, f = assert(pcall(open_font, 'fonts/DroidSansMono.ttf', 44))
assert(not pcall(texture_from_font, 1))
assert(not pcall(texture_from_font, 1, 2))
assert(not pcall(texture_from_font, f, 2))
assert(not pcall(texture_from_font, f, "x", "a", "b", "c"))
assert(not pcall(texture_from_font, f, "x", 3, 4, 5, 6, 7))
assert(not pcall(texture_from_font, f, "x", 3, 4, 5, "x"))
assert(not pcall(texture_from_font, f, ""))

assert(pcall(render_texture, cara))
assert(not pcall(render_texture))
assert(not pcall(render_texture, cara, 2))
assert(pcall(render_texture, cara, 2, 3))
assert(not pcall(render_texture, cara, 2, 3, 4))
assert(pcall(render_texture, cara, 2, 3, 4, 5))
assert(not pcall(render_texture, cara, 2, 3, 4, 5, 6))
assert(pcall(render_texture, cara, 2, 3, 4, 5, 6, 7))
assert(not pcall(render_texture, cara, 2, 3, 4, 5, 6, 7, 8))
assert(pcall(render_texture, cara, 2, 3, 4, 5, 6, 7, 8, 9))

assert(not pcall(texture_alpha_mod, 1, 2))
assert(not pcall(texture_alpha_mod, 1))
assert(pcall(texture_alpha_mod, cara, 2))
assert(pcall(destroy_texture, cara))
assert(not pcall(render_texture, cara))
assert(not pcall(texture_alpa_mod, cara, 100))

