---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---
Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(10042001)
end

function Messages:msgWaitMatching()
    return 10042002
end

return Messages