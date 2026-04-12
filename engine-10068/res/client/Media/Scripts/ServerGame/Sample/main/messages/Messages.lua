---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---
Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(43)
end

function Messages:customTimeHint(seconds)
    return 159, tostring(seconds)
end

return Messages