--region *.lua

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(31);
end

function Messages:breakBedTip(destroy, team)
    return 1008024, destroy .. TextFormat.argsSplit .. team .. TextFormat.argsSplit .. team;
end

function Messages:getStoreName(type)
    local name = "";
    if BaseMain:isChina() then
        if type == 1 then
            name = name .. "防具"
        elseif type == 2 then
            name = name .. "武器"
        elseif type == 3 then
            name = name .. "方块"
        elseif type == 4 then
            name = name .. "食物"
        else
            name = name .. "其他"
        end
    else
        if type == 1 then
            name = name .. "Armor"
        elseif type == 2 then
            name = name .. "Arms"
        elseif type == 3 then
            name = name .. "Block"
        elseif type == 4 then
            name = name .. "Food"
        else
            name = name .. "Other"
        end
    end
    return name
end

function Messages:msgUpgrade(name, goods)
    return 70, name .. TextFormat.argsSplit .. goods;
end

function Messages:msgStore()
    return TextFormat:getTipType(68);
end

function Messages:msgIronMoney()
    return 1008001
end

function Messages:msgPublicMoney()
    return TextFormat:getTipType(69);
end

function Messages:msgBlockPlaceTip()
    return 1008002
end

--function Messages:msgOutOfWorldTip(dier)
--    return 1008003, dier
--end

function Messages:msgGameTip()
    return 1008004
end

function Messages:msgShopLimit()
    return 1008006
end

function Messages:msgShopBuySuccess()
    return 1008007
end

function Messages:msgShopBuyHotSpringSuc(name)
    return 1008010, name
end

function Messages:msgHpSpringMax()
    return 1008011
end

function Messages:msgBuyLimit()
    return 1008012
end

function Messages:msgIronMax()
    return 1008013
end

function Messages:msgHotSpringLvUp(name)
    return 1008014, name
end

function Messages:msgEnMax()
    return 1008015
end

function Messages:equipLowerLevel()
    return 1008016
end

function Messages:msgNotUseRescue()
    return 1008017
end

function Messages:msgNotBreakBed()
    return 1008018
end

function Messages:msgEnLvUp(name)
    return 1008019, name
end

function Messages:msqMaxTool()
    return 1008020
end

function Messages:msgBedHasBreak()
    return 1008022
end

function Messages:msgTeamFail(team)
    return 1008023, team
end

function Messages:msgKillByVoid(killer, deceased)
    return 1008027, killer .. TextFormat.argsSplit .. deceased
end

function Messages:msgToolNotBreakBed()
    return 1008028
end

function Messages:msgShopLackOfMoney()
    return 1008029
end

function Messages:msgFullInventory()
    return 1008030
end

function Messages:msgOutOfArea()
    return 1008033
end

function Messages:adjustTeamMsg(player)
    return 1008038, GameMatch.Teams[player.teamId]:getDisplayName(player:getLanguage())
end

function Messages:msgResourceLevelUp(itemId)
    if itemId == 264 then
        return 1008031
    elseif itemId == 388 then
        return 1008032
    end
end

function Messages:msgGameStageChange(gameStage)
    if gameStage == 2 then
        return 1008031, 2
    elseif gameStage == 3 then
        return 1008032, 2
    elseif gameStage == 4 then
        return 1008031, 3
    elseif gameStage == 5 then
        return 1008032, 3
    end
end

function Messages:msgSummonsNumLimit()
    return 1008043
end

return Messages


--endregion
