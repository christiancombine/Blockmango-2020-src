---
--- Created by Jimmy.
--- DateTime: 2018/4/25 0025 13:13
---

MonsterConfig = {}
MonsterConfig.monsters = {}

function MonsterConfig:init(monsters)
    self:initMonsters(monsters)
end

function MonsterConfig:initMonsters(monsters)
    for id, monster in pairs(monsters) do
        local item = {}
        item.id = tonumber(id or "0") or 0
        item.name = monster.name
        item.actor = monster.actor
        item.aiFile = monster.aiFile
        item.isUp = tonumber(monster.isUp) == 1
        item.hp = tonumber(monster.hp)
        item.attack = tonumber(monster.attack)
        item.defense = tonumber(monster.defense)
        item.attackX = tonumber(monster.attackX)
        item.attackSpeed = tonumber(monster.attackSpeed)
        item.attackDistance = tonumber(monster.attackDistance)
        item.attackCd = tonumber(monster.attackCd)
        item.moveSpeed = tonumber(monster.moveSpeed)
        item.addHp = tonumber(monster.addHp)
        item.exp = tonumber(monster.exp)
        item.gold = tonumber(monster.gold)
        item.isChangeTarget = tonumber(monster.isChangeTarget)
        item.changeTargetCd = tonumber(monster.changeTargetCd)
        item.isEnterHint = tonumber(monster.isEnterHint) == 1
        item.isBoss = tonumber(monster.isBoss) == 1
        item.isAutoAttack = tonumber(monster.isAutoAttack)
        item.attackType = tonumber(monster.attackType)
        item.patrolDistance = tonumber(monster.patrolDistance)
        item.skillId = tonumber(monster.skillId)
        item.skillCd = tonumber(monster.skillCd)
        local effect = StringUtil.split(monster.effect, "#")
        item.effect = {}
        item.effect.id = tonumber(effect[1])
        item.effect.time = tonumber(effect[2])
        item.effect.lv = tonumber(effect[3])
        self.monsters[id] = item
    end
    self:prepareMonster()
end

function MonsterConfig:prepareMonster()
    for i, monster in pairs(self.monsters) do
        local type = 2
        if monster.isBoss then
            type = 3
        end
        HostApi.addMonsterSetting(MonsterConfig.newMonsterSetting(monster, type))
    end
end

function MonsterConfig.newMonsterSetting(monster, type)
    local setting = MonsterSetting.new()
    setting.monsterId = monster.id
    setting.monsterBtTree = monster.aiFile
    setting.monsterType = type
    setting.isAutoAttack = monster.isAutoAttack
    setting.attackDistance = monster.attackDistance
    setting.attackCd = monster.attackCd
    setting.moveSpeed = monster.moveSpeed
    setting.attackType = monster.attackType
    setting.patrolDistance = monster.patrolDistance
    setting.skillId = monster.skillId
    setting.skillCd = monster.skillCd
    return setting
end

function MonsterConfig:getMonsterById(id)
    local monster = self.monsters[id]
    if monster then
        return monster
    end
    return nil
end

return MonsterConfig