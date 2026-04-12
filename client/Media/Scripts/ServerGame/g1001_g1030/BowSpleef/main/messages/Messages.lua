--region *.lua
require "config.GameConfig"

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(34);
end

function Messages:msgGameData(players, seconds, score)
    return 35, players .. TextFormat.argsSplit .. seconds .. TextFormat.argsSplit .. score;
end

return Messages


--endregion
