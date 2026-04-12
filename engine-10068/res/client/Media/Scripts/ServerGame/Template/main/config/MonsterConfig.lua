MonsterConfig = {}

MonsterConfig.settings = {}

function MonsterConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.monsterId = tonumber(vConfig.n_monsterId) or 0 --怪物id
        data.monsterBtTree = vConfig.s_monsterBtTree or "" --AI行为树
        data.actorName = vConfig.s_actorName or "" --模型名称
        data.monsterType = tonumber(vConfig.n_monsterType) or 0 --怪物类型
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
        table.insert(MonsterConfig.settings, data)
    end
end

return MonsterConfig
