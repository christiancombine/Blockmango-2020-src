
Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1024001)
end

function Messages:showDeep(deep)
    return 137, deep
end

function Messages:showDeepAndHp(deep, hp)
    return 138, deep .. TextFormat.argsSplit .. hp
end

function Messages:assignTeamTimeHint(seconds)
    return 49, tostring(seconds)
end

function Messages:gameStart()
    return 1024002
end

function Messages:enterMine()
    return 1024003
end

function Messages:goToFindJewel()
    return 1024004
end

function Messages:placeStatue()
    return 1024005
end

function Messages:playerGetJewel(teamName, playerName)
    return 1024006, teamName .. TextFormat.argsSplit .. "[" .. playerName .. "]"
end

function Messages:teamSourceFullWithJewel(teamName)
    return 1024007, teamName
end

function Messages:playerWin(teamName, playerName)
    return 1024008, "[" .. playerName .. "]" .. TextFormat.argsSplit .. teamName
end

function Messages:jewelRecreate()
    return 1024009
end

function Messages:teamSourceFull(teamName)
    return 1024010, teamName
end

function Messages:JewelDiscover()
    return 1024011
end

return Messages