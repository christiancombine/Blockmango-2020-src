---
--- Created by Jimmy.
--- DateTime: 2018/8/23 0023 12:05
---
require "data.GameTower"
require "data.GameRuins"

TowerConfig = {}
TowerConfig.towers = {}

function TowerConfig:init(towers)
    self:initTowers(towers)
end

function TowerConfig:initTowers(towers)
    for _, tower in pairs(towers) do
        local item = {}
        item.id = tonumber(tower.id)
        item.teamId = tonumber(tower.teamId)
        item.AIFile = tower.AIFile
        item.actor = tower.actor
        item.position = VectorUtil.newVector3(tower.x, tower.y, tower.z)
        item.yaw = tonumber(tower.yaw)
        item.ruinsId = tonumber(tower.ruinsId)
        item.ruinsAIFile = tower.ruinsAIFile
        item.ruinsActor = tower.ruinsActor
        item.ruinsName = tower.ruinsName
        item.tipHint = tower.tipHint
        item.price = tonumber(tower.price)
        item.defenseX = tonumber(tower.defenseX)
        item.attack = tonumber(tower.attack)
        item.hp = tonumber(tower.hp)
        item.attackDistance = tonumber(tower.attackDistance)
        item.attackCd = tonumber(tower.attackCd)
        item.attackCount = tonumber(tower.attackCount)
        local c_buff = StringUtil.split(tower.buff, "#")
        item.buff = { id = tonumber(c_buff[1]), lv = tonumber(c_buff[2]), time = tonumber(c_buff[3]) }
        item.money = tonumber(tower.money)
        item.type = tower.type
        self.towers[#self.towers + 1] = item
        self:addTowerConfigToEngine(item)
    end
end

function TowerConfig:addTowerConfigToEngine(config)
    local tower = MonsterSetting.new()
    tower.monsterId = config.id
    tower.monsterBtTree = config.AIFile
    tower.monsterType = 4
    tower.isAutoAttack = true
    tower.attackDistance = config.attackDistance
    tower.attackCd = config.attackCd
    tower.attackCount = config.attackCount
    tower.moveSpeed = 0
    tower.attackType = 3
    tower.patrolDistance = 0
    tower.skillId = 0
    tower.skillCd = 0
    HostApi.addMonsterSetting(tower)
    local ruins = MonsterSetting.new()
    ruins.monsterId = config.ruinsId
    ruins.monsterBtTree = config.ruinsAIFile
    ruins.monsterType = 4
    ruins.isAutoAttack = false
    ruins.attackDistance = 0
    ruins.attackCd = 100
    ruins.moveSpeed = 0
    ruins.attackType = 1
    ruins.patrolDistance = 0
    ruins.skillId = 0
    ruins.skillCd = 0
    HostApi.addMonsterSetting(ruins)
end

return TowerConfig