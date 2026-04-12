---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(144)
end

function Messages:safeAreaTimeTip(seconds)
    return 145, tostring(seconds)
end

function Messages:prepareSafeAreaTip()
    return 146
end

function Messages:poisonTimeTip(seconds)
    return 147, tostring(seconds)
end

function Messages:preparePoisonAreaTip()
    return 148
end

function Messages:airSupportTimeTip(seconds)
    return 149, tostring(seconds)
end

function Messages:prepareAirSupportTip()
    return 150
end

function Messages:poisonAttackTip()
    return 151
end

return Messages