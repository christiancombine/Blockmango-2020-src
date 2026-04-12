VideoAdHelper = {}

function VideoAdHelper:checkShowAd(player)
    VideoAdHelper:checkMoneyAd(player)
    VideoAdHelper:checkEggTicketAd(player)
    VideoAdHelper:checkChestCDAd(player)
    VideoAdHelper:checkVipChestAd(player)
    VideoAdHelper:checkFeedAd(player)
end

function VideoAdHelper:checkMoneyAd(player)
    local params = {
        isShow = (os.time() - player.moneyAdTime) >= GameConfig.moneyAdRefreshTime,
        moneyAdReward = GameConfig.moneyAdReward,
    }

    if not params.isShow then
        local delay = GameConfig.moneyAdRefreshTime - (os.time() - player.moneyAdTime)
        LuaTimer:schedule(function(userId)
            local c_player = PlayerManager:getPlayerByUserId(userId)
            if not c_player then
                return
            end

            local c_params = {
                isShow = true,
                moneyAdReward = GameConfig.moneyAdReward,
            }
            local c_data = DataBuilder.new():fromTable(c_params):getData()
            PacketSender:sendServerCommonData(c_player.rakssid, "BirdSimulatorData", "moneyAd", c_data)
        end, delay * 1000, nil, player.userId)
    end

    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(player.rakssid, "BirdSimulatorData", "moneyAd", data)
end

function VideoAdHelper:checkEggTicketAd(player)
    local params = {
        isShow = (os.time() - player.eggTicketAdTime) >= GameConfig.eggTicketAdRefreshTime,
        eggTicketAdReward = GameConfig.eggTicketAdReward,
    }

    if not params.isShow then
        local delay = GameConfig.eggTicketAdRefreshTime - (os.time() - player.eggTicketAdTime)
        LuaTimer:schedule(function(userId)
            local c_player = PlayerManager:getPlayerByUserId(userId)
            if not c_player then
                return
            end

            local c_params = {
                isShow = true,
                eggTicketAdReward = GameConfig.eggTicketAdReward,
            }
            local c_data = DataBuilder.new():fromTable(c_params):getData()
            PacketSender:sendServerCommonData(c_player.rakssid, "BirdSimulatorData", "eggTicketAd", c_data)
        end, delay * 1000, nil, player.userId)
    end

    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(player.rakssid, "BirdSimulatorData", "eggTicketAd", data)
end

function VideoAdHelper:checkChestCDAd(player)
    local params = {
        isShow = (os.time() - player.chestCDAdTime) >= GameConfig.chestCDAdRefreshTime
    }

    if not params.isShow then
        local delay = GameConfig.chestCDAdRefreshTime - (os.time() - player.chestCDAdTime)
        LuaTimer:schedule(function(userId)
            local c_player = PlayerManager:getPlayerByUserId(userId)
            if not c_player then
                return
            end

            local c_params = {
                isShow = true
            }
            local c_data = DataBuilder.new():fromTable(c_params):getData()
            PacketSender:sendServerCommonData(c_player.rakssid, "BirdSimulatorData", "chestCDAd", c_data)
        end, delay * 1000, nil, player.userId)
    end

    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(player.rakssid, "BirdSimulatorData", "chestCDAd", data)
end

function VideoAdHelper:checkVipChestAd(player)
    local params = {
        isShow = false
    }

    local watchAdTimeSection = VideoAdHelper:getVipChestTimeSection(player.vipChestAdTime)
    local curTimeSection = VideoAdHelper:getVipChestTimeSection(os.time())

    if curTimeSection > watchAdTimeSection then
        params.isShow = true
    end

    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(player.rakssid, "BirdSimulatorData", "vipChestAd", data)
end

function VideoAdHelper:checkFeedAd(player)
    local params = {
        isShow = (os.time() - player.feedAdTime) >= GameConfig.feedAdRefreshTime,
        feedAdReward = GameConfig.feedAdReward,
    }

    if not params.isShow then
        local delay = GameConfig.feedAdRefreshTime - (os.time() - player.feedAdTime)
        LuaTimer:schedule(function(userId)
            local c_player = PlayerManager:getPlayerByUserId(userId)
            if not c_player then
                return
            end

            local c_params = {
                isShow = true,
                feedAdReward = GameConfig.feedAdReward,
            }
            local c_data = DataBuilder.new():fromTable(c_params):getData()
            PacketSender:sendServerCommonData(c_player.rakssid, "BirdSimulatorData", "feedAd", c_data)
        end, delay * 1000, nil, player.userId)
    end

    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(player.rakssid, "BirdSimulatorData", "feedAd", data)
end

function VideoAdHelper:doGetMoneyAdReward(player, operateType, param1, param2)
    if operateType == 1 then
        --抽蛋
        local entityId = tonumber(param1)
        local eggNpc = DrawLuckyManager.NpcList[entityId]
        if not eggNpc then
            return
        end
        if eggNpc.moneyType ~= CurrencyType.Money then
            return
        end
        if player.userBirdBag == nil then
            return
        end
        if player.userBirdBag:isCanAddNumToCapacity(1) == false then
            HostApi.sendCommonTip(player.rakssid, "bird_capacity_full")
            return
        end
        local price = eggNpc.price
        local deltaPrice = price - GameConfig.moneyAdReward
        if deltaPrice < 0 then
            deltaPrice = 0
        end
        if player.money < deltaPrice then
            return
        end
        player:subMoney(deltaPrice)
        eggNpc:OnDeductionSuccess(nil, player.rakssid)
    elseif operateType == 2 then
        -- 商店物品
        local storeId = tonumber(param1)
        local itemId = tonumber(param2)
        local item = StoreHouseItems:getItemInfo(itemId)
        if not item then
            return
        end
        local currencyType = item.currencyType
        local price = item.price
        local itemType = item.ItemType
        local actorName = item.actorName
        local actorId = item.actorId
        if currencyType ~= CurrencyType.Money then
            return
        end
        local deltaPrice = price - GameConfig.moneyAdReward
        if deltaPrice < 0 then
            deltaPrice = 0
        end
        if player.money < deltaPrice then
            return
        end
        player:subMoney(deltaPrice)
        if itemType == 1 then       --采集器
            player:SetCollector(itemId, actorName, actorId)
        elseif itemType == 2 then   --存储灌
            player:SetStorageBag(itemId, actorName, actorId)
        elseif itemType == 3 then   --装饰
            player:SetDecoration(itemId, actorName, actorId)
        elseif itemType == 4 then   --翅膀
            player:SetWing(itemId, actorName, actorId)
        end
        StoreHouseManager:RefreshStoreInfo(player)
    elseif operateType == 3 then
        -- 小鸟携带扩容
        if not player.userBird or not player.userBirdBag then
            return
        end
        if player.userBirdBag.maxCarryBirdNum > BirdBagConfig:getMaxCarryBag() then
            HostApi.sendCommonTip(player.rakssid, "bird_carry_already_max_level")
            return
        end
        if player.userBirdBag.maxCarryBirdNum >= player.userBirdBag.maxCapacityBirdNum then
            HostApi.sendCommonTip(player.rakssid, "update_maxCapacity_first")
            return
        end
        local price = BirdBagConfig:getNextExpandCarryPrice(player.userBirdBag.maxCarryBirdNum)
        local moneyType = BirdBagConfig:getNextExpandCarryMoneyType(player.userBirdBag.maxCarryBirdNum)
        if moneyType ~= CurrencyType.Money then
            return
        end
        local deltaPrice = price - GameConfig.moneyAdReward
        if deltaPrice < 0 then
            deltaPrice = 0
        end
        if player.money < deltaPrice then
            return
        end
        player:subMoney(deltaPrice)
        player.userBirdBag:addMaxCarryBirdNum(1)
        player:setBirdSimulatorBag()
        -- 开启鸟巢格子
        if player.userNest ~= nil then
            local vec3 = NestConfig:getNestPosByIndex(player.userNest.nestIndex, player.userBirdBag.maxCarryBirdNum)
            if vec3 ~= nil then
                EngineWorld:setBlock(vec3, 96, 2)
            end
        end
        -- 同步门能否走过
        for i, v in pairs(FieldConfig.door) do
            if tonumber(v.type) == 0 then
                if tonumber(v.entityId) ~= 0 and tonumber(player.userBirdBag.maxCarryBirdNum) >= tonumber(v.requirement) then
                    EngineWorld:getWorld():updateSessionNpc(v.entityId, player.rakssid, "", v.actorName, v.bodyName, v.bodyId2, "", "", 0, false)
                end
            elseif tonumber(v.type) == 1 then
                -- vip
                if player.isVipArea and tonumber(player.userBirdBag.maxCarryBirdNum) >= tonumber(v.requirement) then
                    EngineWorld:getWorld():updateSessionNpc(v.entityId, player.rakssid, "", v.actorName, v.bodyName, v.bodyId2, "", "", 0, false)
                end
            end
        end
    elseif operateType == 4 then
        -- 小鸟上限扩容
        if not player.userBird or not player.userBirdBag then
            return
        end

        if player.userBirdBag.maxCapacityBirdNum >= BirdBagConfig:getMaxCapacityBag() then
            HostApi.sendCommonTip(player.rakssid, "bird_capacity_already_max_level")
            return
        end

        local price = BirdBagConfig:getNextExpandCapacityPrice(player.userBirdBag.maxCapacityBirdNum)
        local moneyType = BirdBagConfig:getNextExpandCapacityMoneyType(player.userBirdBag.maxCapacityBirdNum)
        if moneyType ~= CurrencyType.Money then
            return
        end
        local deltaPrice = price - GameConfig.moneyAdReward
        if deltaPrice < 0 then
            deltaPrice = 0
        end
        if player.money < deltaPrice then
            return
        end
        player:subMoney(deltaPrice)
        local num = BirdBagConfig:getNextExpandCapacityNum(player.userBirdBag.maxCapacityBirdNum)
        player.userBirdBag:setMaxCapacityBirdNum(num)
        player:setBirdSimulatorBag()
    end

    player.moneyAdTime = os.time()
    VideoAdHelper:checkMoneyAd(player)

    PacketSender:sendDataReport(player.rakssid, "reward_ad_give", DBManager:getGameType())
end

function VideoAdHelper:doGetEggTicketAdReward(player, params)
    local entityId = tonumber(params)
    local eggNpc = DrawLuckyManager.NpcList[entityId]
    if not eggNpc then
        return
    end
    if eggNpc.moneyType ~= CurrencyType.EggTicket then
        return
    end
    if player.userBirdBag == nil then
        return
    end
    if player.userBirdBag:isCanAddNumToCapacity(1) == false then
        HostApi.sendCommonTip(player.rakssid, "bird_capacity_full")
        return
    end
    local price = eggNpc.price
    local deltaPrice = price - GameConfig.eggTicketAdReward
    if deltaPrice < 0 then
        deltaPrice = 0
    end
    if player:GetTicketNum() < deltaPrice then
        return
    end
    player:SubEggTicket(deltaPrice)
    eggNpc:OnDeductionSuccess(nil, player.rakssid)

    player.eggTicketAdTime = os.time()
    VideoAdHelper:checkEggTicketAd(player)

    PacketSender:sendDataReport(player.rakssid, "reward_ad_give", DBManager:getGameType())
end

function VideoAdHelper:doGetChestCDAdReward(player, params)
    local entityId = tonumber(params)
    local chest = ChestManager.chestDatas[entityId]
    if not chest then
        return
    end

    local randNum = HostApi.random("chest", chest.minNum, chest.maxNum)
    local curCarryBirdNum = 0
    if player.userBirdBag ~= nil then
        curCarryBirdNum = player.userBirdBag.curCarryBirdNum
    end
    player:addProp(chest.itemId, randNum + chest.birdRation * curCarryBirdNum)
    if chest.itemId ~= 9000001 then
        local birdGain = BirdGain.new()
        birdGain.itemId = chest.itemId
        birdGain.num = randNum
        birdGain.icon = chest.icon
        birdGain.name = chest.itemName
        HostApi.sendBirdGain(player.rakssid, {birdGain})
    end

    TaskHelper.dispatch(TaskType.HaveChestEvent, player.userId, 1)
    TaskHelper.dispatch(TaskType.GetItemByOpenChestEvent, player.userId, chest.itemId, randNum)
    local endTime = os.time() + chest.refreshCD
    player:setChestCountDown(chest.chestId, endTime)
    chest:RefreshChesetItems(player, chest.refreshCD)

    player.chestCDAdTime = os.time()
    VideoAdHelper:checkChestCDAd(player)

    PacketSender:sendDataReport(player.rakssid, "reward_ad_give", DBManager:getGameType())
end

function VideoAdHelper:doGetVipChestAdReward(player)
    local chest = {}
    for _, v in pairs(ChestManager.chestDatas) do
        if v:isVipChest() and v.itemId == 9000001 then
            chest = v
        end
    end

    if not next(chest) then
        return
    end

    local randNum = HostApi.random("chest", chest.minNum, chest.maxNum)
    local curCarryBirdNum = 0
    if player.userBirdBag ~= nil then
        curCarryBirdNum = player.userBirdBag.curCarryBirdNum
    end
    player:addProp(chest.itemId, randNum + chest.birdRation * curCarryBirdNum)
    TaskHelper.dispatch(TaskType.HaveChestEvent, player.userId, 1)
    TaskHelper.dispatch(TaskType.GetItemByOpenChestEvent, player.userId, chest.itemId, randNum)

    player.vipChestAdTime = os.time()
    VideoAdHelper:checkVipChestAd(player)

    PacketSender:sendDataReport(player.rakssid, "reward_ad_give", DBManager:getGameType())
end

function VideoAdHelper:doGetFeedAdReward(player, params)
    local birdId = tonumber(params)
    if not player.userBird then
        return
    end

    local bird = player.userBird:getBirdById(birdId)
    if not bird then
        return
    end

    local maxLevel = BirdConfig:getMaxLevel()
    local maxExp = BirdConfig:getExpByLevel(maxLevel)

    if bird.level >= maxLevel then
        return
    end

    if tonumber(bird.exp) >= tonumber(maxExp) then
        player:setBirdSimulatorBag()
        HostApi.sendCommonTip(rakssid, "bird_max_level")
        return
    end

    player.feedAdTime = os.time()
    VideoAdHelper:checkFeedAd(player)

    local levelExp = BirdConfig:getExpByLevel(bird.level + 1)
    local exp = math.ceil(levelExp * GameConfig.feedAdReward / 100)
    if exp < 10 then
        exp = 10
    end
    if exp > 200 then
        exp = 200
    end
    player.userBird:addExp(birdId, exp)
    HostApi.notifyGetGoods(player.rakssid, "set:bird_ad.json image:exp", exp)

    PacketSender:sendDataReport(player.rakssid, "reward_ad_give", DBManager:getGameType())
end

function VideoAdHelper:getVipChestTimeSection(vTime)
    local time = {}
    local year = tonumber(os.date("%Y", os.time()))
    local month = tonumber(os.date("%m", os.time()))
    local day = tonumber(os.date("%d", os.time()))
    for _, v in pairs(GameConfig.vipChestADRefreshTime) do
        local hour = tonumber(StringUtil.split(v, ":")[1])
        local min = tonumber(StringUtil.split(v, ":")[2])
        local refreshTime = os.time({ year = year, month = month, day = day, hour = hour, min = min, sec = 0})
        table.insert(time, refreshTime)
    end

    table.sort(time, function (a , b)
        return a < b
    end)

    local timeSection = 0
    for _, v in pairs(time) do
        if vTime >= v then
            timeSection = v
        end
    end

    return timeSection
end

return VideoAdHelper