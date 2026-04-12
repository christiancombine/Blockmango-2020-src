AreaConfig = {}
AreaConfig.area = {}
AreaConfig.m_initUnlockedArea = {}
AreaConfig.expandConfig = {}
AreaConfig.expandItems = {}
AreaConfig.landNpcName = "name"
AreaConfig.landNpcActorName = "ranchers_selling_brand.actor"
AreaConfig.waitForUnlockLandNpcName = "waitForUnlock"
AreaConfig.waitForUnlockLandNpcActorName = "ranchers_selling_brand.actor"

function AreaConfig:initArea(config)
    for _, area in pairs(config) do
        local data = {}
        data.id = tonumber(area.id)
        data.startPosX = tonumber(area.startPosX)
        data.startPosY = tonumber(area.startPosY)
        data.startPosZ = tonumber(area.startPosZ)
        data.endPosX = tonumber(area.endPosX)
        data.endPosY = tonumber(area.endPosY)
        data.endPosZ = tonumber(area.endPosZ)
        data.npcPosX = tonumber(area.npcPosX)
        data.npcPosY = tonumber(area.npcPosY)
        data.npcPosZ = tonumber(area.npcPosZ)
        data.fileName = area.fileName
        self.area[data.id] = data
    end
end

function AreaConfig:initNpcName(name)
    self.landNpcName = name
end

function AreaConfig:initNpcActorName(actorName)
    self.landNpcActorName = actorName
end

function AreaConfig:initWaitForUnlockNpcName(name)
    self.waitForUnlockLandNpcName = name
end

function AreaConfig:initWaitForUnlockNpcActorName(actorName)
    self.waitForUnlockLandNpcActorName = actorName
end

function AreaConfig:initUnlockedArea(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        self.m_initUnlockedArea[data.id] = data.id
    end
end

function AreaConfig:initExpandConfig(config)
    for i, v in pairs(config) do
        local data = {}
        data.count = tonumber(v.count)
        data.money = tonumber(v.money)
        data.prosperity = tonumber(v.prosperity)
        data.time = tonumber(v.time)
        data.expandItems = StringUtil.split(v.expandItems, "#")
        data.rewardItems = StringUtil.split(v.rewardItems, "#")
        self.expandConfig[data.count] = data
    end
end

function AreaConfig:getExpandInfoByCount(count)
    local resource = {}
    local config = self.expandConfig[count]
    if config ~= nil then
        resource.count = config.count
        resource.money = config.money
        resource.prosperity = config.prosperity
        resource.time = config.time
        resource.expandItems = {}
        resource.rewardItems = {}
        for i, v in pairs(config.expandItems) do
            resource.expandItems[i] = {}
            local item = StringUtil.split(v, ":")
            if item[2] ~= 0 then
                resource.expandItems[i].itemId = tonumber(item[1])
                resource.expandItems[i].num = tonumber(item[2])
            end
        end

        for i, v in pairs(config.rewardItems) do
            resource.rewardItems[i] = {}
            local item = StringUtil.split(v, ":")
            if item[2] ~= 0 then
                resource.rewardItems[i].itemId = tonumber(item[1])
                resource.rewardItems[i].num = tonumber(item[2])
            end

        end
        return resource
    end

    return nil
end

return AreaConfig