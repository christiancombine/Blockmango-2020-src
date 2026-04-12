AIPlayerConfig = {}
AIPlayerConfig.Config = {}

local DressGroup = {}
local ItemGroup = {}

AIPlayerConfig.number = 0

function AIPlayerConfig:init()
    AIPlayerConfig:initDress(FileUtil.getConfigFromCsv("AiDressConfig.csv"))
    AIPlayerConfig:initItems(FileUtil.getConfigFromCsv("AiItemConfig.csv"))
    AIPlayerConfig:initPlayer(FileUtil.getConfigFromCsv("AiPlayerConfig.csv"))
end

function AIPlayerConfig:initPlayer(config)
    for _, v in pairs(config) do
        local item = {}
        item.Id = tostring(v.Id)
        item.groupId = tostring(v.groupId)
        item.teamId = tostring(v.teamId)
        item.weight = tostring(v.weight)
        item.aiConfigId = tostring(v.aiConfigId)
        item.itemGroupId = tostring(v.itemGroup)
        item.dressGroupId = tostring(v.dressGroup)
        item.itemGroup = ItemGroup[item.itemGroupId] or {}
        item.dressGroup = DressGroup[item.dressGroupId] or {}
        self.Config[item.Id] = item
        AIPlayerConfig.number = AIPlayerConfig.number + 1
    end
end

function AIPlayerConfig:initDress(config)
    for _, v in pairs(config) do
        local groupId = tostring(v.groupId)
        local dressId = tonumber(v.dressId)
        local itemIds = ItemMapping:getOldItemIdsByDressId(dressId)
        local dressGroup = DressGroup[groupId] or {}
        for _, itemId in pairs(itemIds) do
            dressGroup[tostring(itemId)] = dressId
        end
        DressGroup[groupId] = dressGroup
    end
end

function AIPlayerConfig:initItems(config)
    for _, v in pairs(config) do
        local item = {}
        item.Id = tostring(v.Id)
        item.aiConfigId = tonumber(v.aiConfigId)
        item.groupId = tostring(v.groupId)
        item.itemId = tonumber(v.itemId)
        item.dressId = tonumber(v.dressId)
        item.num = tonumber(v.num)
        local core = tonumber(v.isCore or 0)
        if core ~= 0 then
            item.isCore = true
        else
            item.isCore = false
        end
        ItemGroup[item.groupId] = ItemGroup[item.groupId] or {}
        table.insert(ItemGroup[item.groupId], item)
    end
end

function AIPlayerConfig:getConfigById(id)
    if self.Config[tostring(id)] then
        return self.Config[tostring(id)]
    end
    return AIPlayerConfig:randomOneConfig()
end

function AIPlayerConfig:getConfigs(groupId, teamId)
    local configs = {}
    for _, config in pairs(self.Config) do
        if tonumber(config.groupId) == tonumber(groupId) and tonumber(config.teamId) == tonumber(teamId) then
            table.insert(configs, config)
        end
    end
    return configs
end

function AIPlayerConfig:randomOneConfig()
    if AIPlayerConfig.number == 0 then
        return
    end
    local index = 0
    local randomIndex = math.random(1, AIPlayerConfig.number)
    for _, config in pairs(AIPlayerConfig.Config) do
        index = index + 1
        if index == randomIndex then
            return config
        end
    end
end