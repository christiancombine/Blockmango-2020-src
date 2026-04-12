---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1050001)
end

function Messages:msgNotEnoughMoneyTip()
    return 1050002
end

function Messages:msgUpdateSuccessTip()
    return 1050003
end

function Messages:msgUpdateMaxLevelTip()
    return 1050004
end

-- function Messages:msgUIButtonCountDown(seconds)
--     return 1050005, seconds
-- end

function Messages:msgTransmitting()
    return 1050005
end

return Messages