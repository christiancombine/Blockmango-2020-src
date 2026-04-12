Messages = {}

function Messages:msgWaitTeleportTip(time)
    return 1054001, tostring((time))
end

function Messages:msgGameStartCountDownTip(time)
    return 1054002, tostring((time))
end

function Messages:msgOutOfWorldTip(name)
    return 1054003, name
end

function Messages:msgTntRain(name, island)
    return 1054004, name .. TextFormat.argsSplit .. tostring(island)
end

function Messages:msgArrowRain(name, island)
    return 1054005, name .. TextFormat.argsSplit .. tostring(island)
end

function Messages:msgWaterRain(name, island)
    return 1054006, name .. TextFormat.argsSplit .. tostring(island)
end

function Messages:msgTransfer()
    return 1054007
end

function Messages:msgInterchange()
    return 1054008
end

function Messages:msgBeInterchange()
    return 1054009
end

function Messages:msgFrozen()
    return 1054010
end

function Messages:msgBeFrozen()
    return 1054011
end

function Messages:msgInvisibility()
    return 1054012
end

function Messages:msgGoldApple()
    return 1054013
end

function Messages:msgQuicklyBuild()
    return 1054014
end

function Messages:msgGetProperty()
    return 1054015
end

function Messages:msgThunder(name)
    return 1054016, name
end

function Messages:msgIslandFrozen(name, island)
    return 1054017, name .. TextFormat.argsSplit .. tostring(island)
end

function Messages:msgHotPotatoTelePos()
    return 1054019
end

function Messages:msgGameStage(index)
    if index == 1 then
        return 1054022
    elseif index == 2 then
        return 1054023
    elseif index == 3 then
        return 1054024
    elseif index == 4 then
        return 1054025
    end
    return nil
end

function Messages:msgOutOfArea()
    return 1054026
end

function Messages:msgNotEnoughTimeEnderDragon()
    return 1054027
end

function Messages:msgNotEnoughTimeWithered()
    return 1054028
end

function Messages:msgTeleportFailedCantFindTarget()
    return 1054029
end

function Messages:msgKillTip(killer, dier)
    return 1054030, killer .. TextFormat.argsSplit .. dier
end

function Messages:msgHotPotatoInvFull()
    return 1054032
end

function Messages:msgTntRainToOwn(island)
    return 1054033, island
end

function Messages:msgArrowRainToOwn(island)
    return 1054034, island
end

function Messages:msgWaterRainToOwn(island)
    return 1054035, island
end

function Messages:msgIslandFrozenToOwn(island)
    return 1054036, island
end

function Messages:msgThunderToOwn()
    return 1054037
end

function Messages:msgFailedTip(name)
    return 1054038, name
end

return Messages