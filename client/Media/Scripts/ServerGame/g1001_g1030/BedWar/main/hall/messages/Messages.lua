---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1046001)
end

function Messages:msgTimeMax()
    return 1046002
end
return Messages