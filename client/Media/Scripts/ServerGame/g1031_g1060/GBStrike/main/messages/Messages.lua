---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1033001)
end

function Messages:msgStandToBuyTip()
    return 1033002
end

function Messages:msgPurchaseFailedTip()
    return 1033003
end

function Messages:msgNotOperateTip()
    return 1033004
end

function Messages:msgLackOfMoney()
    return 1033005
end

function Messages:msgWaitMatching()
    return 1032011
end

return Messages