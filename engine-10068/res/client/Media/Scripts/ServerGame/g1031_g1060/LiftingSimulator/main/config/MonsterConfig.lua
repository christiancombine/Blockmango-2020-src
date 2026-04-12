MonsterConfig = {}

MonsterConfig.settings = {}

local function addMonsterSetting(setting)
    local monster = MonsterSetting.new()
    monster.monsterId = setting.monsterId
    monster.monsterBtTree = setting.monsterBtTree
    monster.actorName = setting.actorName
    monster.monsterType = setting.monsterType
    monster.attackType = setting.attackType
    monster.isAutoAttack = setting.isAutoAttack
    monster.attackDistance = setting.attackDistance
    monster.attackCd = setting.attackCd
    monster.attackCount = setting.attackCount
    monster.moveSpeed = setting.moveSpeed
    monster.patrolDistance = setting.patrolDistance
    monster.chaseRange = setting.chaseRange
    monster.skillId = setting.skillId
    monster.skillCd = setting.skillCd
    monster.bombEffect = setting.bombEffect
    HostApi.addMonsterSetting(monster)
end

function MonsterConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.monsterId = tonumber(vConfig.n_monsterId) or 0 --怪物id
        data.monsterBtTree = vConfig.s_monsterBtTree or "" --AI行为树
        data.actorName = vConfig.s_actorName or "" --模型名称
        data.attackedDis = tonumber(vConfig.n_attackedDis) or 0 --怪物血量
        data.hp = tonumber(vConfig.n_hp) or 0 --怪物血量
        data.attack = tonumber(vConfig.n_attack) or 0 --怪物攻击力
        data.monsterType = 3
        data.attackType = tonumber(vConfig.n_attackType) or 0 --攻击类型
        data.isAutoAttack = tonumber(vConfig.n_isAutoAttack) or 0 --是否自动攻击
        data.attackDistance = tonumber(vConfig.n_attackDistance) or 0 --攻击距离
        data.attackCd = tonumber(vConfig.n_attackCd) or 0 --攻击cd
        data.attackCount = tonumber(vConfig.n_attackCount) or 0 --攻击次数
        data.moveSpeed = tonumber(vConfig.n_moveSpeed) or 0 --移动速度
        data.patrolDistance = tonumber(vConfig.n_patrolDistance) or 0 --巡逻范围
        data.chaseRange = tonumber(vConfig.n_chaseRange) or 0 --追击范围
        data.skillId = tonumber(vConfig.n_skillId) or 0 --技能id
        data.skillCd = tonumber(vConfig.n_skillCd) or 0 --技能cd
        data.bombEffect = vConfig.s_bombEffect or "" --远程攻击效果
        data.scale = tonumber(vConfig.n_scale) or 1 --缩放值
        data.hpQuantity = tonumber(vConfig.n_hpQuantity) or 1 --血条数
        data.returnHpInterval = tonumber(vConfig.n_returnHpInterval) or 1 --回血间隔时间
        data.returnHpValue = tonumber(vConfig.n_returnHpValue) or 1 --回血百分比(0-100)
        table.insert(MonsterConfig.settings, data)
        addMonsterSetting(data)
    end
end

function MonsterConfig:findById(monsterId)
    for _, setting in pairs(self.settings) do
        if setting.monsterId == monsterId  then
            return setting
        end
    end
end

return MonsterConfig
