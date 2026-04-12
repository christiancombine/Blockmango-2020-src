---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1048001)
end

function Messages:msgBlockPlaceTip()
    return 1048002
end

function Messages:msgExtendAreaTip()
    return 1048003
end

function Messages:msgNotEnoughMoneyTip()
    return 1048004
end

function Messages:msgIsUsingFurnace()
    return 1048005
end

function Messages:msgForbidSetHome()
    return 1048006
end

function Messages:msgCannotBackHome()
    return 1048007
end

function Messages:msgCannotOperatorVistor()
    return 1048008
end

function Messages:msgUIButtonCountDown(seconds)
    return 1048009, seconds
end

function Messages:msgSetHomeSuccess()
    return 1048010
end

function Messages:msgBackHomeSuccess()
    return 1048011
end

function Messages:msgSetHomeFailed()
    return 1048012
end

function Messages:msgBoxLimit()
    return 1048013
end

function Messages:msgFurnaceLimit()
    return 1048014
end

function Messages:msgSignPostTextLimit(signPostNum)
    return 1048015, signPostNum
end

function Messages:msgTransmitting()
    return 1048016
end

function Messages:msgGoParty()
    return 1048017
end

function Messages:msgEnlargeBlockPlaceTip()
    return 1048018
end

function Messages:msgNotEnlargeBlockPlaceTip()
    return 1048019
end

function Messages:msgActorTextLimit(actortNum)
    return 1048020, actortNum
end

return Messages