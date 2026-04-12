---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---
Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(20052)
end

function Messages:longWaitingHint()
    return 20053
end

function Messages:selectMapHint(name)
    return 20054, TextFormat:getTipType(name)
end

function Messages:upgradePlayerHint(name)
    return 20055
end

function Messages:gameWin()
    return 20056
end

function Messages:gameLose()
    return 20057
end

function Messages:hasBuy()
    return 20058
end

function Messages:gamePrepareTimeHint(second)
    return 20062, tostring(second)
end

function Messages:autoRespawnHint()
    return 20063
end

return Messages