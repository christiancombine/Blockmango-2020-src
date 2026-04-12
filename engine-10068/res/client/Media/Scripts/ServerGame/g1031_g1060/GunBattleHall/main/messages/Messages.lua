---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1032001)
end

function Messages:msgHaveSameGunTip()
    return 1032002
end

function Messages:msgGunForgingFailureTip()
    return 1032003
end

function Messages:msgGunForgingSuccessTip()
    return 1032004
end

function Messages:msgNotOperateTip()
    return 1032005
end

function Messages:msgSuccessBuySPTip()
    return 1032006
end

function Messages:msgPurchaseFailedTip()
    return 1032007
end

function Messages:msgSuccessBuySPActorTip()
    return 1032008
end

function Messages:msgFailedBuySPActorTip()
    return 1032009
end

function Messages:msgNotOwningTheGunTip()
    return 1032010
end

function Messages:msgWaitMatching()
    return 1032011
end

return Messages