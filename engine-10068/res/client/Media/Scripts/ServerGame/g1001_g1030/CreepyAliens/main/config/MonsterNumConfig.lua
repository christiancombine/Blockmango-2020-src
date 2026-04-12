---
--- Created by Jimmy.
--- DateTime: 2018/4/25 0025 13:13
---

MonsterNumConfig = {}
MonsterNumConfig.monsterNums = {}

function MonsterNumConfig:init(monsterNums)
    self:initMonsterNums(monsterNums)
end

function MonsterNumConfig:initMonsterNums(monsterNums)
    for id, monsterNum in pairs(monsterNums) do
        local item = {}
        item.id = id
        item.monsterId = monsterNum.monsterId
        item.num = tonumber(monsterNum.num)
        self.monsterNums[id] = item
    end
end

function MonsterNumConfig:getMonsterNumById(id)
    local monster = self.monsterNums[id]
    if monster then
        return monster
    end
    for i, monster in pairs(self.monsterNums) do
        return monster
    end
end

return MonsterNumConfig