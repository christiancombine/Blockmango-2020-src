---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1049001)
end

function Messages:msgCannotBackHome()
    return 1049002
end

function Messages:msgResetMapCountDown(seconds)
    return 1049003, seconds
end

function Messages:msgGetGemSuccess(name)
    return 1049004, name
end

function Messages:msgTransmitting()
    return 1049005
end

function Messages:msgUIButtonCountDown(seconds)
    return 1049006, seconds
end

return Messages