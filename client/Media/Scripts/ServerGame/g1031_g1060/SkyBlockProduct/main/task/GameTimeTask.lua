--region *.lua

local CGameTimeTask = class("GameTimeTask", ITask)

function CGameTimeTask:onTick(ticks)
    GameMatch:onTick(ticks)
    self:foodAddHP(ticks)
	self:addItemsToWord(ticks)
    self:productItem(ticks)
    -- self:expireItem(ticks)
    self:updateNpc(ticks)
    self:updateSignPostName(ticks)
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

function CGameTimeTask:addItemsToWord(ticks)
	local players = PlayerManager:getPlayers()
	for i, player in pairs(players) do
		if player.isLife then
            for _, npc in pairs(player.products) do
                local id = player:updateNpcLevel(ticks, npc)
                local item = ProductResourceConfig:getItem(id)
                for i , k in pairs(item) do
                    if ticks % k.time == 0 then
                        if player:checkIsHAvePri(PrivilegeConfig.priType.npc_eff_double) then
                            k.num = k.num * 2
                        end

                        for j = 1, k.num do
                            local product_item = {}
                            product_item.item_id = k.itemId
                            product_item.item_num = 1
                            --product_item.expire_time = os.time() + GameConfig.maxTime
                            product_item.entityId = 0
                            player:insertItemToNpcPreList(npc, product_item, ProductLimitConfig.Online)
                        end
                        player:updateOreNum(npc)
                    end
                end
            end

		end
	end
end

function CGameTimeTask:getPreProductListItemByRandom(pre_product_list)
    local result_item = nil
    if pre_product_list then
        local randomPos = HostApi.random("getPreProductListItem", 1, #pre_product_list + 1)
        result_item = pre_product_list[randomPos]
    end
    return result_item
end

function CGameTimeTask:productItem(ticks)
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        if player.cur_product_num < GameConfig.maxNum then-- GameConfig.max_product_num then
            if DbUtil:CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
                for _, npc in pairs(player.products) do
                    local item = self:getPreProductListItemByRandom(npc.pre_product_list)
                    if item then
                        if item.item_num > GamePlayer.DropNum.FirstStage then
                            local n = math.floor(item.item_num / GamePlayer.DropNum.FirstStage)
                            for i = 1, n do
                                self:addNpcItemToWordOnline(player, npc, item , 64)
                            end
                        else
                            if item.item_num > GamePlayer.DropNum.SecondStage then
                                for i = 1, GamePlayer.DropNum.SecondStage do
                                    self:addNpcItemToWordOnline(player, npc, item , 1)
                                end
                            else
                                self:addNpcItemToWordOnline(player, npc, item , 1)
                            end
                        end
                    end
                end
            end
        end
    end
end

function CGameTimeTask:addNpcItemToWordOnline(player, npc, item ,itemNum)
    -- 1. spawn the item
    local motion = VectorUtil.newVector3(0, 0, 0)
    local Vector3i = GameConfig:getOriginPosByManorIndex(player.manorIndex)
    local radius = GameConfig.randomRadius
    local pos = player:getRandomPos(Vector3i, radius)
    if pos then
        local entityId = EngineWorld:addEntityItem(item.item_id, itemNum, 0, item.life, pos, motion, false, true)
        table.insert(player.world_products, entityId)
        player.cur_product_num = player.cur_product_num + 1

        -- 2. remove item from pre_product_list
        player:removeItemFromNpcList(npc.pre_product_list, item.item_id, itemNum)

        -- 3. add item to had_product_list
        local product_item = {}
        product_item.item_id = item.item_id
        product_item.item_num = item.item_num
        product_item.entityId = entityId
        player:insertItemToNpcHadList(npc, product_item, itemNum)
    end
    player:updateOreNum(npc)
end

--function CGameTimeTask:expireItem(ticks)
--    local players = PlayerManager:getPlayers()
--    for i, player in pairs(players) do
--        if DbUtil:CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
--            for _, npc in pairs(player.products) do
--                --for key, product in pairs(npc.had_product_list) do
--                --    local cur_time = os.time()
--                --    if cur_time > product.expire_time then
--                --        local entity = EngineWorld:getEntity(product.entityId)
--                --        if entity ~= nil then
--                --            entity:setDead()
--                --            player:removeItemFromNpcList(npc.had_product_list, key)
--                --            player.cur_product_num = player.cur_product_num - 1
--                --        end
--                --        break
--                --    end
--                --end
--                --for key, product in pairs(npc.pre_product_list) do
--                --    local cur_time = os.time()
--                --    if cur_time > product.expire_time then
--                --        player:removeItemFromNpcList(npc.pre_product_list, key)
--                --        break
--                --    end
--                --end
--            end
--        end
--    end
--end

function CGameTimeTask:updateNpc()
    local isUpdateAll = false
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isUpdateAllNpc == true then
            isUpdateAll = true
            HostApi.log("npc.is_laterPlayerReady = true")
            player.isUpdateAllNpc = false
        end
    end
    for _, player in pairs(players) do
        for index, npcs in pairs(GameConfig.ProductNpc) do
            if tonumber(index) == player.manorIndex then
                for entityId, npc in pairs(npcs) do
                    if npc then
                        local info = player:getUpdateNpcInfo(npc.id)
                        local curInterval = info.curInterval or 0
                        local curLevel = info.level or -2
                        local maxLv = ProductConfig:getMaxLevel(npc.id)
                        if curLevel >= maxLv then
                            curLevel = maxLv
                        end
                        if isUpdateAll then
                            npc.is_laterPlayerReady = true
                        end
                        npc:onUpdate(curInterval, curLevel)
                    end
                end
            end
        end
    end
    isUpdateAll = false
end

function CGameTimeTask:updateSignPostName()
    local isUpdateAll = false
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isUpdateAllName == true then
            isUpdateAll = true
            HostApi.log("player.isUpdateAllName = true")
            player.isUpdateAllName = false
        end
    end
    if isUpdateAll then
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            local signPost_pos = GameConfig:getOriginPosByManorIndex(player.manorIndex)
            if signPost_pos then
                local namePos = VectorUtil.newVector3(signPost_pos.x, signPost_pos.y + tonumber(GameConfig.SignPostName[1] or 3), signPost_pos.z)
                HostApi.deleteWallText(0, namePos)
                HostApi.addWallText(0, 2, 0, player.name, namePos, tonumber(GameConfig.SignPostName[2] or 5), 90, 0,
                    {   tonumber(GameConfig.SignPostName[3] or 1),
                        tonumber(GameConfig.SignPostName[4] or 1),
                        tonumber(GameConfig.SignPostName[5] or 1), 1})
            end
        end
    end
    isUpdateAll = false
end


GameTimeTask = CGameTimeTask.new(1)

return GameTimeTask

--endregion
