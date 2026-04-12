---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1027001)
end

function Messages:startMessage(messageId)
    return messageId
end

return Messages