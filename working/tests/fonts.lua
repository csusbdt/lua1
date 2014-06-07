assert(not pcall(open_font, 'fonts/DroidSansMono.ttf'))
assert(not pcall(open_font, 'fonts/DroidSansMono.ttf', 'not a number'))
assert(not pcall(open_font, 'fonts/I_dont_exist.ttf', 24))

local _, font = assert(pcall(open_font, 'fonts/DroidSansMono.ttf', 24))

assert(pcall(close_font, font))
assert(not pcall(close_font, font))

