---
--- Created by Jimmy.
--- DateTime: 2018/4/25 0025 13:13
---

BasementConfig = {}
BasementConfig.basements = {}

function BasementConfig:init(basements)
    self:initBasements(basements)
end

function BasementConfig:initBasements(basements)
    for id, basement in pairs(basements) do
        local item = {}
        item.id = id
        item.level = tonumber(basement.level)
        item.hp = tonumber(basement.hp)
        item.defense = tonumber(basement.defense)
        self.basements[id] = item
    end
    self:prepareBasement()
end

function BasementConfig:prepareBasement()
    for i, basement in pairs(self.basements) do
        HostApi.addMonsterSetting(BasementConfig.newMonsterSetting(basement))
    end
end

function BasementConfig.newMonsterSetting(basement)
    local setting = MonsterSetting.new()
    setting.monsterId = basement.id
    setting.monsterBtTree = ""
    setting.monsterType = 1
    setting.isAutoAttack = 0
    setting.attackDistance = 0
    setting.attackCd = 0
    setting.moveSpeed = 0
    setting.attackType = 0
    setting.skillId = 0
    return setting
end

function BasementConfig:getBasementById(id)
    local basement = self.basements[id]
    if basement then
        return basement
    end
    for i, basement in pairs(self.basements) do
        return basement
    end
end

return BasementConfig