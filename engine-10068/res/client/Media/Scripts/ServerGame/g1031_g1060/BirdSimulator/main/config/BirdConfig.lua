BirdConfig = {}
BirdConfig.birds = {}
BirdConfig.combine = {}
BirdConfig.rate = {}
BirdConfig.level = {}
BirdConfig.property = {}
BirdConfig.gifts = {}
BirdConfig.parts = {}
BirdConfig.foods = {}

function BirdConfig:initBirds(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.name = v.name
        data.icon = v.icon
        data.talent = v.talent
        data.content = v.content
        data.quality = tonumber(v.quality)
        data.color = tonumber(v.color)
        data.actorName = v.actorName
        data.actorBody = v.actorBody
        data.actorBodyId = v.actorBodyId
        data.skill = ""
        data.glassesId = ""
        data.hatId = ""
        data.beakId = ""
        data.wingId = ""
        data.tailId = ""
        data.effectId = ""

        if v.glassesId ~= "@@@" then
            data.glassesId = v.glassesId
        end

        if v.hatId ~= "@@@" then
            data.hatId = v.hatId
        end

        if v.beakId ~= "@@@" then
            data.beakId = v.beakId
        end

        if v.wingId ~= "@@@" then
            data.wingId = v.wingId
        end

        if v.tailId ~= "@@@" then
            data.tailId = v.tailId
        end

        if v.effectId ~= "@@@" then
            data.effectId = v.effectId
        end

        if v.skill ~= "@@@" then
            data.skill = v.skill
        end

        data.action = StringUtil.split(v.action, "#")
        data.chance = tonumber(v.chance)
        data.coinId = tonumber(v.coinId)
        data.collectValue = tonumber(v.collectValue)
        data.collectCD = tonumber(v.collectCD)
        data.attack = tonumber(v.attack)
        data.attackCD = tonumber(v.attackCD)
        data.convert = tonumber(v.convert)
        data.convertCD = tonumber(v.convertCD)
        data.flySpeed = tonumber(v.flySpeed)
        data.collectLayer = tonumber(v.collectLayer)
        data.collectValueGrow = tonumber(v.collectValueGrow)
        data.collectCDGrow = tonumber(v.collectCDGrow)
        data.attackGrow = tonumber(v.attackGrow)
        data.attackCDGrow = tonumber(v.attackCDGrow)
        data.convertGrow = tonumber(v.convertGrow)
        data.convertCDGrow = tonumber(v.convertCDGrow)
        data.flySpeedGrow = tonumber(v.flySpeedGrow)
        data.extraColorCollect = StringUtil.split(v.extraColorCollect, "#")
        self.birds[data.id] = data
    end
end

function BirdConfig:initCombine(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.birdId = tonumber(v.birdId)
        data.quality = tonumber(v.quality)
        data.weight = tonumber(v.weight)
        data.egg = v.egg
        self.combine[data.id] = data
    end
end

function BirdConfig:initGifts(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.icon = v.icon
        data.coinId = tonumber(v.coinId)
        data.type = tonumber(v.type)
        data.itemId = tonumber(v.itemId)
        data.baseValue = tonumber(v.baseValue)
        data.activeTime = tonumber(v.activeTime)
        data.maxTimes = tonumber(v.maxTimes)
        data.name = v.name == "@@@" and "" or v.name
        self.gifts[data.id] = data
    end
end

function BirdConfig:initRate(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.birdId = tonumber(v.birdId)
        data.level = tonumber(v.level)
        data.rate = tonumber(v.rate)
        self.rate[data.id] = data
    end
end

function BirdConfig:initLevel(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.level = tonumber(v.level)
        data.exp = tonumber(v.exp)
        self.level[data.level] = data
    end
end

function BirdConfig:initParts(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.type = tonumber(v.type)
        data.bodyName = v.bodyName
        data.bodyId = v.bodyId
        self.parts[data.id] = data
    end
end

function BirdConfig:initFoods(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.name = v.name
        data.icon = v.icon
        data.desc = v.desc
        data.exp = tonumber(v.exp)
        self.foods[data.id] = data
    end
end

function BirdConfig:getBirdInfoById(id)
    if self.birds[tonumber(id)] ~= nil then
        return self.birds[tonumber(id)]
    end

    return nil
end

function BirdConfig:getCombineBirdIdByQuality(quality)
    local weight = 0
    local birds = {}
    for i, v in pairs(self.combine) do
        if v.quality == quality then
            weight = weight + v.weight
            table.insert(birds, v)
        end
    end

    if weight == 0 then
        return 0
    end


    local randWeight = HostApi.random("combine", 0, weight)
    local num = 0
    for i, v in pairs(birds) do
        if randWeight >= num and randWeight <= num + v.weight then
            return v
        end
        num = num + v.weight
    end

    return birds[1]
end

function BirdConfig:getCombineBirdInfoById(birdId)
    for i, v in pairs(self.combine) do
        if tonumber(v.birdId) == tonumber(birdId) then
            return v
        end
    end

    return nil
end

function BirdConfig:getRateByIdAndLevel(birdId, level)
    for i, v in pairs(self.rate) do
        if v.birdId == birdId and v.level == level then
            return v.rate
        end
    end

    return 0
end

function BirdConfig:getMaxLevel()
    return #self.level
end

function BirdConfig:getLevelByExp(exp)
    for i, v in pairs(self.level) do
        if v.exp > exp then
            return v.level - 1
        end
    end

    return self:getMaxLevel()
end

function BirdConfig:getExpByLevel(level)
    for i, v in pairs(self.level) do
        if v.level == level then
            return v.exp
        end
    end

    return 0
end

function BirdConfig:getGiftInfoByCoinId(coinId)
    for i, v in pairs(self.gifts) do
        if tonumber(v.coinId) == tonumber(coinId) then
            return v
        end
    end

    return nil
end

function BirdConfig:getGiftInfoById(id)
    if self.gifts[tonumber(id)] ~= nil then
        return self.gifts[tonumber(id)]
    end

    return nil
end

function BirdConfig:getGiftInfoByItemId(itemId)
    for i, v in pairs(self.gifts) do
        if tonumber(v.baseValue) == tonumber(itemId) and tonumber(v.type) == 3 then
            return v
        end
    end

    return nil
end

function BirdConfig:getPartIdsById(id)
    local partIds = {
        glassesId = "",
        hatId = "",
        beakId = "",
        wingId = "",
        tailId = "",
        effectId = "",
        bodyId = ""
    }
    if self.birds[id] ~= nil then
        partIds = {
            glassesId = self.birds[id].glassesId,
            hatId = self.birds[id].hatId,
            beakId = self.birds[id].beakId,
            wingId = self.birds[id].wingId,
            tailId = self.birds[id].tailId,
            effectId = self.birds[id].effectId,
            bodyId = self.birds[id].actorBodyId
        }
    end

    return partIds
end

function BirdConfig:getPartInfoById(id)
    if self.parts[tonumber(id)] ~= nil then
        return self.parts[tonumber(id)]
    end

    return nil
end

function BirdConfig:getExpByFoodId(foodId)
    if self.foods[foodId] ~= nil then
        return self.foods[foodId].exp
    end

    return 0
end

function BirdConfig:getFoodInfoById(foodId)
    if self.foods[foodId] ~= nil then
        return self.foods[foodId]
    end

    return nil
end

function BirdConfig:getExtraColorCollect(birdId, colorId)
    if self.birds[tonumber(birdId)] == nil then
        return 0
    end

    for i, v in pairs(self.birds[tonumber(birdId)].extraColorCollect) do
        local data = StringUtil.split(v, ":")
        if data[1] ~= nil and data[2] ~= nil then
            if colorId == tonumber(data[1]) then
                return tonumber(data[2]) / 100
            end
        end
    end

    return 0
end

return BirdConfig