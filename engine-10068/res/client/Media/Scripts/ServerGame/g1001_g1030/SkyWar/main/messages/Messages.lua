--region *.lua
require "config.GameConfig"

Messages = {}

function Messages:gamename()
	return TextFormat:getTipType(57);
end

return Messages


--endregion
