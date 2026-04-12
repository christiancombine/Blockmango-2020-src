--region *.lua
require "config.GameConfig"

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(55);
end

function Messages:msgGameData(teamMsg, score)
    return 56, teamMsg .. TextFormat.argsSplit .. tostring(score);
end

return Messages


--endregion
