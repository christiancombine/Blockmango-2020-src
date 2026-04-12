--region *.lua
require "config.GameConfig"

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(59);
end

function Messages:msgPvpEnableAfter(time, unit, ifHalf)
    return 60, IMessages:msgTimeTick(time, unit, ifHalf);
end

function Messages:msgWinnerInfo(name)
    return 61, name;
end

function Messages:gamePrepareHint(seconds)
    return 62, tostring(seconds);
end

function Messages:gameStartHint()
    return 63;
end

function Messages:gameDataHint(f1, f2, f3, f4, me, second)
    return 67,  f4..TextFormat.argsSplit..
                f3..TextFormat.argsSplit..
                f2..TextFormat.argsSplit..
                f1..TextFormat.argsSplit..
                self:getMeFloor(me)..TextFormat.argsSplit..
                second;
end

function Messages:becomeRole(role)
    return 64, self:getRoleName(role);
end

function Messages:getRoleName(role)
    if role == GamePlayer.ROLE_DESTROYER then
        return TextFormat:getTipType(66);
    else
        return TextFormat:getTipType(65);
    end
end

function Messages:getMeFloor(floor)
    local me = "0F"
    if floor == 1 then
        return "1F"
    end
    if floor == 2 then
        return "2F"
    end
    if floor == 3 then
        return "3F"
    end
    if floor == 4 then
        return "4F"
    end
    return me
end

return Messages
--endregion
