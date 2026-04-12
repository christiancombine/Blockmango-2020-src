--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "config.GameConfig"

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(76);
end

function Messages:msgGameData(teamMsg, score)
    return 56, teamMsg .. TextFormat.argsSplit .. tostring(score);
end

return Messages


--endregion
