require "config.BonusConfig"
DrawLuckyPool = class("DrawLuckyPool")

function DrawLuckyPool:ctor(index, config)
    self.id = config.id
    self.isKey = config.isKey == 1 and true or false
    self.weight = config.weight
    self.index = index
    self.minRange = 0
    self.maxRange = 0
    self.itemAndWeights = BonusConfig:GetBonusTeamsByPoolId(self.id)

    self.allweight = 0
    for _, info in ipairs(self.itemAndWeights) do
        self.allweight = self.allweight + info.weight
    end
end

function DrawLuckyPool:GetWeight(index)
    local num = self.weight[index]
    if type(num) == "number" then
        return num
    end
    return 0
end

--  刷新改 奖励的 区间
function DrawLuckyPool:RefreshRange(player, max, index)
    self.minRange = max
    self.maxRange =  self:GetWeight(index) + max
    return self.maxRange
end

-- 检测一个值在不在这个区间
function DrawLuckyPool:CheckRangeVal(val)
    return val >= self.minRange and val < self.maxRange
end

function DrawLuckyPool:GetMaxCount()
    return #self.weight
end

-- 当随机到这个奖励池的时候调用
function DrawLuckyPool:OnRandomPool(player, actorBodyId)
    local randNumber = HostApi.random("RandomPool", 1, self.allweight)
    local weight = 0
    local teamId = -1
    local callback = function()
        local allItems = BonusConfig:GetItemByTeamId(teamId)
        local items = {}
        for _, info in pairs(allItems) do
            local birdInfo = BirdConfig:getBirdInfoById(info.id) 
            if type(birdInfo) == "table" then

                local id = player.userBird:getLastBirdIndex()
                local partIds = BirdConfig:getPartIdsById(info.id)
                local status = 0
                if player.userBirdBag ~= nil then
                    if player.userBirdBag.curCarryBirdNum < player.userBirdBag.maxCarryBirdNum then
                        status = 1
                    end
                end
                player.userBird:createBird(id, info.id, 1, 0, status, partIds)
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

                if actorBodyId ~= nil then
                    HostApi.sendBirdLotteryResult(player.rakssid, actorBodyId, bird)
                else
                    local combineBird = BirdConfig:getCombineBirdInfoById(info.id)
                    if combineBird ~= nil then
                        HostApi.sendBirdLotteryResult(player.rakssid, combineBird.egg, bird)
                    end
                end
            end
        end
    end
    local info = self.itemAndWeights[1]
    if weight >= 0 and weight < info.weight then
        teamId = info.id
        callback()
        return
    end
    for _, info in ipairs(self.itemAndWeights) do
        if randNumber >= info.weight and randNumber < weight then
            teamId = info.id
            break
        end
        weight = weight + info.weight
    end
    callback()
end
