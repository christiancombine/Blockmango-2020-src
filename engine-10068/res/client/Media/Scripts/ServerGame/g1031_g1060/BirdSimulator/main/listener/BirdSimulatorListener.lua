require "manager.ChestManager"
require "manager.StoreHouseManager"
require "manager.PersonalStoreManager"
require "manager.TaskManager"
require "manager.AreaInfoManager"
require "manager.NovicePackage"
require "manager.EggChestManager"
BirdSimulatorListener = {}

function BirdSimulatorListener:init()
    BirdSimulatorPersonalStoreBuyGoodsEvent.registerCallBack(self.onBirdSimulatorPersonalStoreBuyGoods)
    BirdSimulatorBagOperationEvent.registerCallBack(self.onBirdSimulatorBagOperation)
    BirdSimulatorFuseEvent.registerCallBack(self.onBirdSimulatorFuse)
    BirdSimulatorStoreOperationEvent.registerCallBack(self.onBirdSimulatorStoreOperation)
    BirdSimulatorLotteryEvent.registerCallBack(self.onBirdSimulatorLottery)
    BirdSimulatorOpenTreasureChestEvent.registerCallBack(self.onBirdSimulatorOpenTreasureChest)
    BirdSimulatorTaskEvent.registerCallBack(self.onBirdSimulatorTask)
    BirdSimulatorNestOperationEvent.registerCallBack(self.onBirdSimulatorNestOperation)
    BirdSimulatorFeedEvent .registerCallBack(self.onBirdSimulatorFeed)
    BirdSimulatorSetDressEvent.registerCallBack(self.onBirdSimulatorSetDress)
    BirdSimulator_MonsterDieEvent.registerCallBack(self.onBirdSimulator_MonsterDie)
    BirdSimulator_MonsterFindPlayerEvent.registerCallBack(self.onBirdSimulator_MonsterFindPlayer)
    BirdSimulatorOpenEggChestEvent.registerCallBack(self.onBirdSimulatorOpenEggChest)
    PlayerCheckinEvent.registerCallBack(self.onBirdSimulatorCheckin)
end

-- 玩家从个人商店购买物品事件
function BirdSimulatorListener.onBirdSimulatorPersonalStoreBuyGoods(rakssId, labelId, giftId)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return false
    end
    return PersonalStoreManager:OnPlayerBuyItem(player, labelId, giftId)
end

function BirdSimulatorListener.onBirdSimulatorOpenTreasureChest(rakssId, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return
    end

    return ChestManager:OnPlayerReceive(player, entityId)
end

function BirdSimulatorListener.onBirdSimulatorLottery(rakssId, npcId)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return false
    end
    return DrawLuckyManager:OnPlayerDrawLucky(player, npcId)
end

function BirdSimulatorListener.onBirdSimulatorTask(rakssId, npcId, taskId)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return false
    end
    return TaskManager:OnPlayerAcceptTask(player, npcId, taskId)
end

function BirdSimulatorListener.onBirdSimulatorBagOperation(rakssId, birdId, operationType)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return
    end
    if player.userBird == nil or player.userBirdBag == nil then
        return
    end
    -- operationType  字段说明 1 携带 2 取消携带 3 删除 4 扩充携带 5 扩容 6转换
    if operationType == 1 then
        if player.userBirdBag.curCarryBirdNum < player.userBirdBag.maxCarryBirdNum then
            player.userBird:carryBirdToWorld(birdId)
        else
            HostApi.sendCommonTip(rakssId, "can_carry_num_max")
        end
    elseif operationType == 2 then
        player.userBird:callBackBirdToBag(birdId)
    elseif operationType == 3 then
        if player.userBird:getBirdNum() <= 1 then
            HostApi.sendCommonTip(rakssId, "can_not_delete_last_bird")
            return
        end
        player.userBird:deleteBirdById(birdId)
    elseif operationType == 4 then
        player.userBirdBag:expandCarryBag(rakssId)
    elseif operationType == 5 then
        player.userBirdBag:expandCapacityBag(rakssId)
    elseif operationType == 6 then
        if player.curFruitNum > 0 then
            if player.userBirdBag.curCarryBirdNum == 0 then
                MsgSender.sendCenterTipsToTarget(player.rakssid, 2, Messages:notCarryBird())
            else
                player:toggleConvert()
                player:setBirdSimulatorPlayerInfo()
            end
        else
            MsgSender.sendCenterTipsToTarget(player.rakssid, 2, Messages:notEnoughFruit())
        end
    end
    player:setBirdSimulatorBag()
end

function BirdSimulatorListener.onBirdSimulatorFuse(rakssId, birds)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return
    end

    if #birds ~= 3 then
        return
    end

    local quality = {}
    for i, v in pairs(birds) do
        local birdInfo = player.userBird:getBirdInfoById(v)
        if birdInfo == nil then
            HostApi.sendCommonTip(rakssId, "fuse_wrong_happen")
            return
        end

        local bird = BirdConfig:getBirdInfoById(birdInfo.birdId)

        if birdInfo.entityId ~= 0 then
            HostApi.sendCommonTip(rakssId, "fuse_need_detach_bird")
            return
        end

        if bird == nil then
            HostApi.sendCommonTip(rakssId, "fuse_wrong_happen")
            return
        end

        if player.userBird:isHasThisBirdId(v) == false then
            HostApi.sendCommonTip(rakssId, "fuse_has_not_bird")
            return
        end

        if type(bird.quality) ~= "number" then
            HostApi.sendCommonTip(rakssId, "fuse_wrong_happen")
            return
        end

        table.insert(quality, bird.quality)
    end

    if quality[1] == quality[2] and quality[1] == quality[3] then
        for i, v in pairs(birds) do
            player.userBird:deleteBirdById(v)
        end

        local combineBird = BirdConfig:getCombineBirdIdByQuality(tonumber(quality[1]))
        local partIds = BirdConfig:getPartIdsById(combineBird.birdId)
        local index = player.userBird:getLastBirdIndex()
        local status = 0
        if player.userBirdBag ~= nil then
            if player.userBirdBag.curCarryBirdNum < player.userBirdBag.maxCarryBirdNum then
                status = 1
            end
        end
        player.userBird:createBird(index, combineBird.birdId, 1, 0, status, partIds)
        player:setBirdSimulatorBag()

        local birdInfo = BirdConfig:getBirdInfoById(combineBird.birdId)
        local bird = BirdInfo.new()
        bird.id = combineBird.birdId
        bird.level = 1
        bird.maxLevel = BirdConfig:getMaxLevel()
        bird.exp = 0
        bird.nextExp = BirdConfig:getExpByLevel(2)
        bird.quality = birdInfo.quality or 1
        bird.speed = birdInfo.flySpeed or 0
        bird.gather = birdInfo.collectValue or 0
        bird.attack = birdInfo.attack or 0
        bird.convert = birdInfo.convert or 0
        bird.convertTime = birdInfo.convertTime or 0
        bird.gatherTime = birdInfo.gatherTime or 0
        bird.isCarry = false
        bird.name = birdInfo.name or ""
        bird.icon = birdInfo.icon or ""
        bird.skill = birdInfo.skill or ""
        bird.talent = birdInfo.talent or ""

        bird.glassesId = partIds.glassesId
        bird.hatId = partIds.hatId
        bird.beakId = partIds.beakId
        bird.wingId = partIds.wingId
        bird.tailId = partIds.tailId
        bird.effectId = partIds.effectId
        bird.bodyId = partIds.bodyId
        HostApi.sendBirdLotteryResult(player.rakssid, combineBird.egg, bird)
    else
        HostApi.sendCommonTip(rakssId, "fuse_must_be_same_quality")
    end
end

-- operationType 1 是购买  二是使用
function BirdSimulatorListener.onBirdSimulatorStoreOperation(rakssId, storeId, itemId, operationType)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return false
    end
    return StoreHouseManager:OnPlayerBuyItem(player, storeId, itemId, operationType)
end

function BirdSimulatorListener.onBirdSimulatorNestOperation(userId, meta, vec3)

    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if player.userNest == nil then
        return
    end

    if player.userBirdBag == nil then
        return
    end

    local nestIndex = player.userNest.nestIndex
    local index = NestConfig:getNestIndexByVec3(nestIndex, vec3)
    if index <= 0 then
        return
    end

    local isUnlock = false
    if index <= player.userBirdBag.maxCarryBirdNum then
        isUnlock = true
    end

    HostApi.sendBirdNestOperation(player.rakssid, isUnlock, index)
end

function BirdSimulatorListener.onBirdSimulatorFeed(rakssid, birdId, foodId, num)
    if num <= 0 then
        return
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil or player.userBird == nil or player.userBirdBag == nil then
        return
    end

    if player.userBird:isLevelMaximum(birdId) then
        player:setBirdSimulatorBag()
        HostApi.sendCommonTip(rakssid, "bird_max_level")
        return
    end

    local maxLevel = BirdConfig:getMaxLevel()
    local maxExp = BirdConfig:getExpByLevel(maxLevel)
    local curExp = player.userBird:getCurExp(birdId)

    local foodExp = BirdConfig:getExpByFoodId(foodId)

    local foodNum = num

    if (maxExp - curExp) < (foodExp * num) then
        foodNum = math.ceil((maxExp - curExp) / foodExp)
    end

    if player.userBirdBag:isEnoughBagItems(foodId, foodNum) then
        player.userBird:feed(birdId, foodId, foodNum)
        player.userBirdBag:subBagItem(foodId, foodNum)
        player:setBirdSimulatorFood()
    else
        HostApi.sendCommonTip(rakssid, "food_not_enough")
    end

end

function BirdSimulatorListener.onBirdSimulatorSetDress(rakssid, birdId, dressType, itemId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if player.userBird == nil then
        return
    end
    player.userBird:setDress(birdId, itemId, dressType)
end

function BirdSimulatorListener.onBirdSimulator_MonsterDie(entityId)
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        if player.userMonster ~= nil then
            local monster = player.userMonster:getMonsterByEntityId(entityId)
            if monster ~= nil then
                monster.isLostTarget = true
                return
            end
        end
    end
    EngineWorld:removeEntity(entityId)
end

function BirdSimulatorListener.onBirdSimulator_MonsterFindPlayer(entityId, userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end

    if player.userMonster == nil then
        return false
    end

    for i, v in pairs(player.userMonster.monsters) do
        if v.entityId == entityId then
            local playerPos = player:getPosition()
            if v.minPos.x <= playerPos.x and v.minPos.y <= playerPos.y and v.minPos.z <= playerPos.z
            and v.maxPos.x >= playerPos.x and v.maxPos.y >= playerPos.y and v.maxPos.z >= playerPos.z then
                return true
            end
        end
    end

    return false
end

function BirdSimulatorListener.onBirdSimulatorOpenEggChest(rakssId, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return
    end

    return EggChestManager:OnPlayerReceive(player, entityId)
end

function BirdSimulatorListener.onBirdSimulatorCheckin(rakssId, type)
    -- type : 0 -> send checkIn information to player
    --        1 -> checkIn
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return
    end

    if type == 1 then
        player:doCheckIn()
    end

    player:sendCheckInData()
end

function BirdSimulatorListener:OnPlayerReady(player)
    AreaInfoManager:SyncAreaInfo(player)
end
