--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

local CGameTimeTask = class("GameTimeTask", ITask)

function CGameTimeTask:onTick(ticks)
	GameMatch:onTick(ticks)
    self:resetMap(ticks)
    self:foodAddHP(ticks)
end

function CGameTimeTask:resetMap(ticks)
    if ticks % GameConfig.map_resetCD == 0 then
        GameConfig.map_initTip = GameConfig.map_initTip + GameConfig.map_resetCD
        local players = PlayerManager:getPlayers()
        for i, player in pairs(players) do
            local pos = player:getPosition()
            local tab = {
                {pos.x - 1, pos.z - 1},
                {pos.x - 1, pos.z + 1},
                {pos.x + 1, pos.z - 1},
                {pos.x + 1, pos.z + 1}
            }
            for _,v in pairs(tab) do
                local edge_pos = VectorUtil.newVector3(v[1], pos.y, v[2])
                -- HostApi.log("edge_pos " .. edge_pos.x.. " " .. edge_pos.y .. " " .. edge_pos.z)
                local isInArea = GameConfig:isInArea(edge_pos, GameConfig.canBreakArea, true)
                if isInArea then
                    player:teleInitPos()
                    break
                end
            end
        end
        HostApi:resetMap()
    end
    if ticks % GameConfig.map_initTip == 0 then
        GameConfig.map_isReset = true
    end
    if GameConfig.map_isReset and GameConfig.map_tipCD > 0 then
        MsgSender.sendCenterTips(1, Messages:msgResetMapCountDown(GameConfig.map_tipCD))
        GameConfig.map_tipCD = GameConfig.map_tipCD - 1
        if GameConfig.map_tipCD < 1 then
            GameConfig.map_isReset = false
            GameConfig.map_tipCD = GameConfig.map_resetTip
        end
    end
end

function CGameTimeTask:foodAddHP(ticks)
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        if player.isLife then
            player.foodLevel = player:getFoodLevel()
            player.hp = player:getHealth()
            player.maxHp = player:getMaxHealth()
            if player.foodLevel <= tonumber(GameConfig.maxFoodAddHp[3] or 16) or player.hp >= player.maxHp then
                player.isMaxFood = false
            end
            if player.hp < player.maxHp and  player.foodLevel >= 20 then
                player.isMaxFood = true
            end
            if player.isMaxFood then
                if ticks % tonumber(GameConfig.maxFoodAddHp[4] or 1) == 0 and player.foodLevel >= tonumber(GameConfig.maxFoodAddHp[3] or 16) + 1 then
                    player.foodLevel =  player.foodLevel - tonumber(GameConfig.maxFoodAddHp[2] or 1)
                    player:setFoodLevel(player.foodLevel)
                    player:addHealth(tonumber(GameConfig.maxFoodAddHp[1] or 1))
                end
            end
            if  player.foodLevel == 0 and ticks % GameConfig.hungerInterval == 0 then
                player:subHealth(GameConfig.hungerValue)
            end
        end
    end
end

GameTimeTask = CGameTimeTask.new(1)

return GameTimeTask

--endregion
