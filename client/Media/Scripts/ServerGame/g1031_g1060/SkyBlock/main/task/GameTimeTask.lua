--region *.lua

local CGameTimeTask = class("GameTimeTask", ITask)

function CGameTimeTask:onTick(ticks)
    GameMatch:onTick(ticks)
    self:foodAddHP(ticks)
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
