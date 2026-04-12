---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(10029001)
end

function Messages:msgWallPrepareCollapsedTip()
    return 10029002
end

function Messages:msgCollectTip()
    return 10029003
end

function Messages:msgWallHasCollapsedTip()
    return 10029004
end

function Messages:msgTowerCollapsedTip()
    return 10029005
end

function Messages:msgTowerRebuildTip()
    return 10029006
end

function Messages:msgBasementUnderAttackTip()
    return 10029007
end

function Messages:msgBasementCollapsedTip()
    return 10029008
end

function Messages:msgKillTip(killer, dier)
    return 10029009, killer .. TextFormat.argsSplit .. dier
end

function Messages:msgGameOver()
    return 10029010
end

function Messages:msgOnly10SecondsWallCollapsedTip()
    return 10029011
end

function Messages:msgDontBuyOcc()
    return 10029012
end

return Messages