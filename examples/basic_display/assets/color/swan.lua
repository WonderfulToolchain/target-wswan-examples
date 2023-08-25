local process = require("wf.api.v1.process")
local superfamiconv = require("wf.api.v1.process.tools.superfamiconv")

for i, file in pairs(process.inputs(".png")) do
	local tilemap = superfamiconv.convert_tilemap(
		file,
		superfamiconv.config()
			:mode("wsc"):bpp(4)
			:color_zero("#ff00fd")
			:tile_base(512):palette_base(12)
	)
	process.emit_symbol("gfx_color_" .. process.symbol(file), tilemap)
end
