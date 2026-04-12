require "manager.ConsumeDiamondsManager"
CustomListener = {}

function CustomListener:init()
    ConsumeDiamondsEvent.registerCallBack(self.onConsumeDiamonds)
    WatchAdvertFinishedEvent.registerCallBack(self.onWatchAdvertFinished)
    CommonReceiveRewardEvent:registerCallBack(self.onCommonReceiveReward)
end

function CustomListener.onConsumeDiamonds(rakssid, isSuccess, message, extra, payId)
    if isSuccess == false then
        return ConsumeDiamondsManager.OnConsumeFail(rakssid, message, extra, payId)
    end

    return ConsumeDiamondsManager.OnConsumeSuccess(rakssid, message, extra, payId)
end

function CustomListener.onWatchAdvertFinished(rakssid, type, params, code)
    if code ~= 1 then
        return
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end

    if type == 104101 then
        local operateType = DataBuilder.new():fromData(params):getNumberParam("operateType")
        local param1 = DataBuilder.new():fromData(params):getNumberParam("param1")
        local param2 = DataBuilder.new():fromData(params):getNumberParam("param2")
        VideoAdHelper:doGetMoneyAdReward(player, operateType, param1, param2)
        return
    end

    if type == 104102 then
        VideoAdHelper:doGetEggTicketAdReward(player, params)
        return
    end

    if type == 104103 then
        VideoAdHelper:doGetChestCDAdReward(player, params)
        return
    end

    if type == 104104 then
        VideoAdHelper:doGetVipChestAdReward(player)
        return
    end

    if type == 104105 then
        VideoAdHelper:doGetFeedAdReward(player, params)
        return
    end
end

function CustomListener.onCommonReceiveReward(player, propsType, propsId, propsCount, expireTime)
    if propsType == "Item" then
        player:addProp(propsId, propsCount)
    end

    if propsType == "Privilege" then
        player:addPrivilegeId(propsId)
    end

    if propsType == "Dress" then
        local partInfo = BirdConfig:getPartInfoById(propsId)
        if partInfo ~= nil then
            player.userBird:addDress(partInfo.id, partInfo.type, partInfo.bodyId)
            player:setBirdSimulatorBirdDress()
            player:setBirdSimulatorBag()
        end
    end

    if propsType == "Bird" then
        local birdInfo = BirdConfig:getBirdInfoById(propsId)
        if type(birdInfo) == "table" then
            local id = player.userBird:getLastBirdIndex()
            local partIds = BirdConfig:getPartIdsById(propsId)
            local status = 0
            if player.userBirdBag ~= nil then
                if player.userBirdBag.curCarryBirdNum < player.userBirdBag.maxCarryBirdNum then
                    status = 1
                end
            end
            player.userBird:createBird(id, propsId, 1, 0, status, partIds)
            player:setBirdSimulatorBag()

            local bird = BirdInfo.new()
            bird.id = id
            bird.level = 1
            bird.maxLevel = BirdConfig:getMaxLevel()
            bird.exp = 0
            bird.nextExp = BirdConfig:getExpByLevel(2)
            bird.quality = birdInfo.quality
            bird.speed = birdInfo.flySpeed
            bird.gather = birdInfo.collectValue
            bird.attack = birdInfo.attack
            bird.convert = birdInfo.convert
            bird.convertTime = birdInfo.convertCD
            bird.gatherTime = birdInfo.collectCD
            bird.isCarry = false
            bird.name = birdInfo.name
            bird.icon = birdInfo.icon
            bird.skill = birdInfo.skill
            bird.talent = birdInfo.talent

            bird.glassesId = partIds.glassesId
            bird.hatId = partIds.hatId
            bird.beakId = partIds.beakId
            bird.wingId = partIds.wingId
            bird.tailId = partIds.tailId
            bird.effectId = partIds.effectId
            bird.bodyId = partIds.bodyId

            local combineBird = BirdConfig:getCombineBirdInfoById(propsId)
            if combineBird ~= nil then
                HostApi.sendBirdLotteryResult(player.rakssid, combineBird.egg, bird)
            end
        end
    end
end

return CustomListener
