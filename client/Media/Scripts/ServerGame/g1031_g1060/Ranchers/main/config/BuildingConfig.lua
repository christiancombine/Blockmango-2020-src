BuildingConfig = {}
BuildingConfig.building = {}
BuildingConfig.firstAnimalBuildingPos = {}
BuildingConfig.firstAnimalBuildingYaw = 0

BuildingConfig.TYPE_FARMING= 1
BuildingConfig.TYPE_FACTORY = 2
BuildingConfig.TYPE_PLANT = 3

BuildingConfig.QUEUE_LOCK = 0
BuildingConfig.QUEUE_LEISURE = 1
BuildingConfig.QUEUE_WAIT = 2
BuildingConfig.QUEUE_WORKING = 3
BuildingConfig.QUEUE_DONE = 4

BuildingConfig.STATE_LOCK = 0
BuildingConfig.STATE_UNLOCK = 1

function BuildingConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.actorId = tonumber(v.actorId)
        data.type = tonumber(v.type)
        data.moneyType = tonumber(v.moneyType)
        data.price = tonumber(v.price)
        data.exp = tonumber(v.exp)
        data.prosperity = tonumber(v.prosperity)
        data.productIds = StringUtil.split(v.productIds, "#")
        data.initQueueNum = tonumber(v.initQueueNum)
        data.maxQueueNum = tonumber(v.maxQueueNum)
        data.productCapacity = tonumber(v.productCapacity)
        data.playerLevel = tonumber(v.playerLevel)
        data.queueUnlockMoney = StringUtil.split(v.queueUnlockMoney, "#")
        self.building[data.id] = data
    end
end

function BuildingConfig:initAnimalBuildingPos(initPos)
    self.firstAnimalBuildingPos = VectorUtil.newVector3(initPos[1], initPos[2], initPos[3])
    self.firstAnimalBuildingYaw = initPos[4]
end

function BuildingConfig:getFirstAnimalBuildingInfo()
    return self.building[600001]
end

function BuildingConfig:getBuildingInfoByActorId(actorId)
    for i, v in pairs(self.building) do
        if v.actorId == actorId then
            return v
        end
    end

    return nil
end

function BuildingConfig:getBuildingInfoById(id)
    for i, v in pairs(self.building) do
        if v.id == id then
            return v
        end
    end

    return nil
end

function BuildingConfig:getQueuesUnlockMoneyById(id)
    if self.building[id] == nil then
        return nil
    end

    local data = {}
    for i, v in pairs(self.building[id].queueUnlockMoney) do
        data[i] = {}
        local money = StringUtil.split(v, ":")
        data[i].money = tonumber(money[1])
        data[i].moneyType = tonumber(money[2])
    end

    return data
end

function BuildingConfig:getQueueUnlockMoneyByQueueId(id, queueId)
    if self.building[id] == nil then
        return nil
    end

    local data = self:getQueuesUnlockMoneyById(id)
    if data ~= nil then
        if data[queueId] ~= nil then
            return data[queueId]
        end
    end

    return nil
end

function BuildingConfig:getProductInfoById(id)
    if self.building[id] == nil then
        return nil
    end
    
    local items = {}
    for i, v in pairs(self.building[id].productIds) do
        items[i] = tonumber(v)
    end

    return items
end

function BuildingConfig:getProsperityById(id)
    if self.building[id] == nil then
        return 0
    end

    return self.building[id].prosperity
end

function BuildingConfig:isInWorld(entityId)
    if entityId <= 0 then
        return false
    end

    return true
end

return BuildingConfig