require "util.Tools"
BonusConfig = {}
BonusConfig.bonusPools = {}
BonusConfig.bonusTeams = {}
BonusConfig.bonusItems = {}
BonusConfig.npc = {}

function BonusConfig:initBonusPools(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.isKey = tonumber(v.isKey)
        data.weight = Tools.SplitNumber(v.weight, "#")
        self.bonusPools[data.id] = data
    end
end

function BonusConfig:initBonusTeams(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.poolId = tonumber(v.poolId)
        data.weight = tonumber(v.weight)
        self.bonusTeams[data.id] = data
    end
end

function BonusConfig:GetBonusTeamsByPoolId(poolId)
    local tab = {}
    for _, v in pairs(self.bonusTeams) do
        if v.poolId == poolId then
            table.insert(tab, {id = v.id, weight = v.weight})
        end
    end
    table.sort(
        tab,
        function(a, b)
            return a.id < b.id
        end
    )
    return tab
end

function BonusConfig:initBonusItems(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.teamId = tonumber(v.teamId)
        data.itemId = tonumber(v.itemId)
        data.num = tonumber(v.num)
        data.meta = tonumber(v.meta)
        data.image = v.image
        data.quality =  tonumber(v.quality)
        self.bonusItems[data.id] = data
    end
end

function BonusConfig:GetItemByTeamId(teamId)
    local tab = {}
    for _, info in pairs(self.bonusItems) do
        if info.teamId == teamId then
            table.insert(tab, {id = info.itemId, num = info.num, icon = info.image})
        end
    end
    if #tab == 0 then
        print("grop id = " .. teamId .. "  not config reward")
    end
    return tab
end

function BonusConfig:initNpc(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.name = v.name
        data.actor = v.actor
        data.yaw = tonumber(v.yaw)
        data.actorBody = v.actorBody
        data.actorBodyId = v.actorBodyId
        data.moneyType = tonumber(v.moneyType)
        data.price = tonumber(v.price)
        data.pos = Tools.CastToVector3(v.x, v.y, v.z)
        data.minPos = Tools.CastToVector3i(v.x1, v.y1, v.z1)
        data.maxPos = Tools.CastToVector3i(v.x2, v.y2, v.z2)
        data.pools = Tools.SplitNumber(v.pools, "#")
        data.icon = v.icon
        data.birds = Tools.SplitNumber(v.birds)
        data.prob = Tools.SplitNumber(v.prob)
        data.type = tonumber(v.type)
        self.npc[data.id] = data
    end
end

function BonusConfig:isKeyPoolById(id)
    for i, v in pairs(self.bonusPools) do
        if v.id == id and v.isKey == 1 then
            return true
        end
    end
    return false
end

function BonusConfig:getPoolInfo(id, offset)
    local tab = {}
    local weight = 0
    for _, v in pairs(self.bonusPools) do
        table.insert(tab, v.weight[offset])
        if v.id == id then
            weight = v.weight[offset]
        end
    end
    return weight, tab
end

function BonusConfig:getEggsInfo()
    local data = {}
    for _, v in pairs(self.npc) do
        if v.icon ~= "@@@" then
            table.insert(data, v)
        end
    end

    table.sort(data, function(a, b)
        return a.id < b.id
    end)

    return data
end

return BonusConfig
