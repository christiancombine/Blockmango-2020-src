---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1031001)
end

function Messages:welcomeManor(name)
    return 1031002, name
end

return Messages