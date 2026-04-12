---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(155)
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
    if role == GamePlayer.ROLE_HIDE then
        return TextFormat.colorGreen .. "[" .. TextFormat:getTipType(156) .. "]"
    elseif role == GamePlayer.ROLE_SEEK then
        return TextFormat.colorRed .. "[" .. TextFormat:getTipType(157) .. "]"
    else
        return TextFormat.colorGreen .. "[" .. TextFormat:getTipType(156) .. "]"
    end
end

function Messages:assignRoleTimeHint(seconds)
    return 49, tostring(seconds)
end

function Messages:becomeHideHint()
    return 158
end

function Messages:changeAndHideHint(seconds)
    return 159, tostring(seconds)
end

function Messages:waitChangeAndHideHint(seconds)
    return 160, tostring(seconds)
end

function Messages:changeActorByNoStart()
    return 161
end

function Messages:changeActorByNoTimes(times)
    return 162, tostring(times)
end

function Messages:changeActorByNoHide()
    return 163
end

function Messages:msgLastModelName(name)
    return 164, "[" .. TextFormat:getTipType(name) .. "]"
end

function Messages:findFalse()
    return 20001
end

function Messages:findTrue(name)
    return 20002, "[" .. TextFormat:getTipType(name) .. "]"
end

function Messages:finded()
    return 20003
end

function Messages:lastHide(num)
    return 20004, tonumber(num)
end

function Messages:findFalseLast45Second()
    return 20050
end

function Messages:last45Second(seconds)
    return 20051, tostring(seconds)
end

return Messages