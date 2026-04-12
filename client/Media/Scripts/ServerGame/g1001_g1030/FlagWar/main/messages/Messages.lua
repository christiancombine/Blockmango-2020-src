---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(20064)
end

function Messages:selectMapHint(name)
    return 20054, TextFormat:getTipType(name)
end

function Messages:msgSelectRole(seconds)
    return 20065, tostring(seconds)
end

function Messages:msgSelectRoleTime(seconds)
    return 20066, tostring(seconds)
end

function Messages:longWaitingHint()
    return 20053
end

function Messages:selectRoleAgain()
    return 20067
end

function Messages:selectRoleHint(name)
    return 20068, name
end

function Messages:enterTransferGate()
    return 20069
end

function Messages:quitTransferGate()
    return 20070
end

function Messages:transferGateHint(seconds)
    return 20071, tostring(seconds)
end

function Messages:enterBloodPool()
    return 20072
end

function Messages:quitBloodPool()
    return 20073
end

function Messages:msgGetFlag(name)
    return 20074, name
end

function Messages:msgHandedInFlag(name)
    return 20075, name
end

function Messages:msgGetFlagTarget()
    return 20076
end

function Messages:msgHandedInFlagTarget()
    return 20077
end

function Messages:msgScore(teams)
    local message = ""
    for id, team in pairs(teams) do
        if id ~= #teams then
            message = message .. team.color .. team.name .. " : " .. team.score .. TextFormat.colorWrite .. " - "
        else
            message = message .. team.color .. team.name .. " : " .. team.score
        end
    end
    return message
end

function Messages:msgTransferGateHint()
    return 1022001
end

function Messages:msgEnemyAreaHint()
    return 1022002
end

function Messages:msgGetFlagTeammateHint(name)
    return 1022003, name
end

function Messages:msgGetFlagEnemyHint(name)
    return 1022004, name
end

function Messages:msgGetFlagAfterHint()
    return 1022005
end

function Messages:msgNoFlagInStationHint()
    return 1022006
end

function Messages:msgHandFlagHasHint()
    return 1022007
end

function Messages:msgInEnemyFlagStationHint()
    return 1022008
end

function Messages:msgNoRoleHint()
    return 1022009
end

return Messages