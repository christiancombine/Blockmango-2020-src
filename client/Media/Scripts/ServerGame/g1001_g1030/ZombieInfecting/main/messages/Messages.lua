--region *.lua

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(77)
end

function Messages:gameOverHint(seconds)
    return 28, tostring(seconds)
end

function Messages:gameResetTimeHint()
    return 29;
end

function Messages:becomeRole(role)
    return 45, self:getRoleName(role)
end

function Messages:roleSelected(role)
    if role == GamePlayer.ROLE_SOLDIER then
        return 78
    elseif role == GamePlayer.ROLE_ZOMBIE then
        return 79
    end
end

function Messages:getRoleName(role)
    if role == GamePlayer.ROLE_ZOMBIE then
        return TextFormat.colorRed .. "[" .. TextFormat:getTipType(81) .. "]"
    elseif role == GamePlayer.ROLE_SOLDIER then
        return TextFormat.colorGreen .. "[" .. TextFormat:getTipType(80) .. "]"
    else
        return ""
    end
end

function Messages:assignRoleTimeHint(seconds)
    return 49, tostring(seconds)
end

function Messages:msgGameData(soldiers, zombies, score)
    return 84, soldiers .. TextFormat.argsSplit .. zombies .. TextFormat.argsSplit .. score
end

function Messages:convertToZombie()
    return 85
end

function Messages:dontBuyGoodsByZombie()
    return 89
end

function Messages:convertHunterMsgToSoliderStart()
    return 130
end

function Messages:convertHunterMsgToZombieStart()
    return 131
end

function Messages:convertHunterMsgToSoliderEnd()
    return 132
end

function Messages:convertHunterMsgToZombieEnd()
    return 133
end

return Messages

--endregion
