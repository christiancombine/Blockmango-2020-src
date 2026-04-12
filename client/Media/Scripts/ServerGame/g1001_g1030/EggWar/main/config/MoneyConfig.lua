---
--- Created by Jimmy.
--- DateTime: 2017/11/21 0021 10:52
---
MoneyConfig = {}
MoneyConfig.money = {}
MoneyConfig.coinMapping = {}

function MoneyConfig:init(config)
    self:initMoney(config)
end

function MoneyConfig:initMoney(config)
    for i, v in pairs(config) do
        local money = {}
        money.id = tonumber(v.id)
        money.position = VectorUtil.newVector3(v.position[1], v.position[2], v.position[3])
        money.npcPosition = VectorUtil.newVector3(v.npcPosition[1], v.npcPosition[2], v.npcPosition[3])
        money.npcYaw = v.npcPosition[4]
        money.actorName = tostring(v.actorName) or nil
        money.name = tostring(v.name) or nil
        money.mineral = {}
        for j, k in pairs(v.mineral) do
            local mineral = {}
            mineral.itemId = tonumber(k.itemId)
            mineral.level = tonumber(k.level)
            mineral.life = tonumber(k.life)
            mineral.time = tonumber(k.time)
            mineral.num = tonumber(k.num)
            mineral.costItem = tonumber(k.price[1])
            mineral.costNum = tonumber(k.price[2])
            mineral.itemName = tonumber(k.itemName)
            money.mineral[j] = mineral
        end
        self.money[i] = money
    end
end

function MoneyConfig:getMoneyConfigByIdAndLevel(id, level)
    for i, v in pairs(self.money) do
        if id == v.id then
            for j, k in pairs(v.mineral) do
                if k.level == level then
                    return k
                end
            end
        end
    end
    return nil
end

function MoneyConfig:initCoinMapping(coinMapping)
    for i, v in pairs(coinMapping) do
        self.coinMapping[i] = {}
        self.coinMapping[i].coinId = v.coinId
        self.coinMapping[i].itemId = v.itemId
    end
end

function MoneyConfig:prepareNpc()
    for i, v in pairs(self.money) do
        GameMatch.money[v.id] = {}
        GameMatch.money[v.id].id = v.id
        GameMatch.money[v.id].mineral = self.money[v.id].mineral[1]
        GameMatch.money[v.id].position = self.money[v.id].position
        if v.actorName ~= nil then
            local entityId = EngineWorld:addActorNpc(v.npcPosition, v.npcYaw, v.actorName, function(entity)
                entity:setHeadName(v.name or "")
                entity:setSkillName("idle")
            end)
            GameMatch.money[v.id].entityId = entityId
        end
    end
end

return MoneyConfig