MonsterConfig = {}
MonsterConfig.monsters = {}

function MonsterConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.genre = tonumber(v.genre)
        data.name = v.name
        data.hp = tonumber(v.hp)
        data.attack = tonumber(v.attack)
        data.defence = tonumber(v.defence)
        data.attackCD = tonumber(v.attackCD)
        data.attackDistance = tonumber(v.attackDistance)
        data.moveSpeed = tonumber(v.moveSpeed)
        data.actorName = v.actorName
        data.actorBody = v.actorBody
        data.actorBodyId = v.actorBodyId
        data.dropRadius = tonumber(v.dropRadius) or 1
        data.rewardItems = StringUtil.split(v.rewardItems, "#")
        data.dieEffect = tostring(v.dieEffect)
        data.viewRange = tonumber(v.viewRange)
        data.viewDegree = tonumber(v.viewDegree)
        data.moveSpeedAddition = tonumber(v.moveSpeedAddition)
        data.canFly = (tonumber(v.canFly) == 1)
        data.flyHigh = tonumber(v.flyHigh)
        data.canEscape = (tonumber(v.canEscape) == 1)
        data.patrolType = tonumber(v.patrolType)
        data.patrolDistanceOnce = tonumber(v.patrolDistanceOnce)
        data.patrolCD = tonumber(v.patrolCD)
        data.alertTime = tonumber(v.alertTime)
        data.pursueDistance = tonumber(v.pursueDistance)
        data.patrolDistance = tonumber(v.patrolDistance)
        data.canTogether = tonumber(v.canTogether)
        data.togetherRange = tonumber(v.togetherRange)
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
        monster.monsterBtTree = "MonsterG1049"
        monster.monsterType = 5
        monster.isAutoAttack = true
        monster.attackDistance = v.attackDistance
        monster.attackCd = v.attackCD
        monster.attackCount = v.attack
        monster.moveSpeed = v.moveSpeed
        monster.attackType = 0
        monster.patrolDistance = v.patrolDistance
        monster.skillId = 0
        monster.skillCd = 0
        monster.viewRange = v.viewRange
        monster.viewDegree = v.viewDegree
        monster.moveSpeedAddition = v.moveSpeedAddition
        monster.canFly = v.canFly
        monster.flyHigh = v.flyHigh
        monster.canEscape = v.canEscape
        monster.patrolType = v.patrolType
        monster.patrolDistanceOnce = v.patrolDistanceOnce
        monster.patrolCD = v.patrolCD
        monster.alertTime = v.alertTime
        monster.pursueDistance = v.pursueDistance
        monster.canTogether = v.canTogether
        monster.togetherRange = v.togetherRange
        monster.deathEffectName = v.dieEffect
        HostApi.addMonsterSetting(monster)
    end
end

return MonsterConfig