--region *.lua

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(44);
end

function Messages:gameOverHint(seconds)
    return 28, tostring(seconds);
end

function Messages:gameResetTimeHint()
    return 29;
end

function Messages:becomeRole(role)
    return 45, self:getRoleName(role);
end

function Messages:getRoleName(role)
    if role == GamePlayer.ROLE_CIVILIAN then
        return TextFormat.colorGreen .. "[" .. TextFormat:getTipType(46) .. "]";
    elseif role == GamePlayer.ROLE_MURDERER then
        return TextFormat.colorRed .. "[" .. TextFormat:getTipType(47) .. "]";
    elseif role == GamePlayer.ROLE_SHERIFF then
        return TextFormat.colorGreen .. "[" .. TextFormat:getTipType(48) .. "]";
    else
        return TextFormat.colorGreen .. "[" .. TextFormat:getTipType(46) .. "]";
    end
end

function Messages:assignRoleTimeHint(seconds)
    return 49, tostring(seconds);
end

function Messages:sheriffKilled()
    return 50;
end

function Messages:civilianKilled(left)
    return 52, tostring(left);
end

function Messages:murdererKilled()
    return 51;
end

function Messages:zeroOfCivilian()
    return 53;
end

function Messages:msgGameData(civilians, murderers, hp)
    return 54, civilians..TextFormat.argsSplit..murderers..TextFormat.argsSplit..hp;
end

return Messages

--endregion
