MonsterConfig = {}
MonsterConfig.monsters = {}

function MonsterConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.name = v.name
        data.hp = tonumber(v.hp)
        data.attack = tonumber(v.attack)
        data.attackCD = tonumber(v.attackCD)
        data.attackDistance = tonumber(v.attackDistance)
        data.moveSpeed = tonumber(v.moveSpeed)
        data.refreshCD = tonumber(v.refreshCD)
        data.actorName = v.actorName
        data.actorBody = v.actorBody
        data.actorBodyId = v.actorBodyId
        data.reward = StringUtil.split(v.reward, "#")
        data.num = StringUtil.split(v.num, "#")
        data.exp = tonumber(v.exp)
        data.fightPoint = tonumber(v.fightPoint)
        data.rewardItems = StringUtil.split(v.rewardItems, "#")
        self.monsters[data.id] = data
    end
end

function MonsterConfig:getMonsterInfoById(id)
    if self.monsters[tonumber(id)] ~= nil then
        return self.monsters[tonumber(id)]
    end
    return nil
end

function MonsterConfig:prepareMonster()
    for i, v in pairs(self.monsters) do
        local monster = MonsterSetting.new()
        monster.monsterId = v.id
        monster.monsterBtTree = "BirdMonster"
        monster.monsterType = 2
        monster.isAutoAttack = true
        monster.attackDistance = v.attackDistance
        monster.attackCd = v.attackCD
        monster.attackCount = v.attack
        monster.moveSpeed = v.moveSpeed
        monster.attackType = 0
        monster.patrolDistance = 0
        monster.skillId = 0
        monster.skillCd = 0
        HostApi.addMonsterSetting(monster)
    end
end

return MonsterConfig