---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1037001)
end

function Messages:msgNotOperateTip()
    return 1037003
end

function Messages:msgHaveItemTip(num,coin)
    return 1037002 , num .. TextFormat.argsSplit .. coin
end

return Messages