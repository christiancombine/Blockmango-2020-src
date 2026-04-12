TreeConfig = {}
TreeConfig.tree = {}
TreeConfig.rewards = {}

function TreeConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.quantity = tonumber(v.quantity)
        data.actor = v.actor
        data.body = v.body
        data.bodyId1 = v.bodyId1
        data.bodyId2 = v.bodyId2
        data.bodyId3 = v.bodyId3
        data.bodyId4 = v.bodyId4
        data.basicPoint = tonumber(v.basicPoint)
        data.possibility = tonumber(v.possibility)
        data.waveAmount = tonumber(v.waveAmount)
        data.waveInterval = tonumber(v.waveInterval)
        data.rewardTeam = StringUtil.split(v.rewardTeam, "#")
        data.extraAmount = tonumber(v.extraAmount)
        data.tipId = tonumber(v.tipId)
        self.tree[data.id] = data
    end
end

function TreeConfig:initRewards(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.teamId = tonumber(v.teamId)
        data.coinId = tonumber(v.coinId)
        data.amount = tonumber(v.amount)
        self.rewards[data.id] = data
    end
end

function TreeConfig:getTreeInfo(treeId)
    if self.tree[treeId] ~= nil then
        return self.tree[treeId]
    end

    return nil
end

function TreeConfig:generateTreeId()
    local totalWeight = 0

    for _, v in pairs(self.tree) do
        totalWeight = totalWeight + v.possibility
    end

    if totalWeight == 0 then
        return 0
    end

    local randWeight = HostApi.random("generateTreeId", 0, totalWeight)
    local num = 0
    for i, v in pairs(self.tree) do
        if randWeight >= num and randWeight <= num + v.possibility then
            return v.id
        end
        num = num + v.possibility
    end

    return self.tree[1].id
end

function TreeConfig:getRewards(fieldId, treeId)
    local fieldInfo = FieldConfig:getFieldInfo(fieldId)
    if fieldInfo == nil then
        return nil
    end

    local treeInfo = TreeConfig:getTreeInfo(treeId)
    if treeInfo == nil then
        return nil
    end

    local extraReward = fieldInfo.extraReward
    local extraAmount = treeInfo.extraAmount

    local items = {}
    for _ = 1, extraAmount do
        local data = {}
        data.itemId = extraReward
        data.weight = HostApi.random("treeReward", 1, 100)
        table.insert(items, data)
    end

    local rewards = TreeConfig:getRewardItems(treeInfo.rewardTeam)

    for _, v in pairs(rewards) do
        for _ = 1, v.num do
            local data = {}
            data.itemId = v.itemId
            data.weight = HostApi.random("treeReward", 1, 100)
            table.insert(items, data)
        end
    end

    table.sort(items, function(a , b)
        if a.weight ~= b.weight then
            return a.weight < b.weight
        end
        return a.itemId < b.itemId
    end)

    return items
end

function TreeConfig:getRewardItems(rewardTeam)
    local items = {}
    if #rewardTeam < 1 then
        return items
    end

    local num = HostApi.random("getRewardTeam", 1, #rewardTeam)

    for _, v in pairs(self.rewards) do
        if tonumber(v.teamId) == tonumber(num)
        and tonumber(v.amount) > 0 and tonumber(v.coinId) > 0 then
            local data = {}
            data.num = tonumber(v.amount)
            data.itemId = tonumber(v.coinId)
            table.insert(items, data)
        end
    end

    return items
end

return TreeConfig