---
--- Created by Jimmy.
--- DateTime: 2017/11/14 0014 16:10
---
require "data.GameBasement"

Scene = class()

function Scene:ctor(config)
    self.monsters = {}
    self.monsterNum = 0
    self.kills = 0
    self.basement = nil
    self:init(config)
end

function Scene:init(config)
    self.name = tonumber(config.name)
    self:initPos(config.initPos)
    self:initBasementPos(config.basementPos)
    self:initMonsterPos(config.monsterPos)
    self:initBossPos(config.bossPos)
end

function Scene:initPos(initPos)
    self.initPos = VectorUtil.newVector3(initPos[1], initPos[2], initPos[3])
end

function Scene:initBasementPos(basementPos)
    self.basementPos = {}
    self.basementPos.position = VectorUtil.newVector3(basementPos[1], basementPos[2], basementPos[3])
    self.basementPos.yaw = tonumber(basementPos[4])
end

function Scene:initMonsterPos(monsterPos)
    self.monsterPos = {}
    for i, position in pairs(monsterPos) do
        local pos = {}
        pos.initPos = VectorUtil.newVector3(position.initPos[1], position.initPos[2], position.initPos[3])
        pos.yaw = tonumber(position.initPos[4])
        self.monsterPos[i] = pos
    end
end

function Scene:initBossPos(bossPos)
    self.bossPos = {}
    for i, position in pairs(bossPos) do
        local pos = {}
        pos.initPos = VectorUtil.newVector3(position.initPos[1], position.initPos[2], position.initPos[3])
        pos.yaw = tonumber(position.initPos[4])
        self.bossPos[i] = pos
    end
end

function Scene:getMonster(entityId)
    return self.monsters[tostring(entityId)]
end

function Scene:getKills()
    return self.kills
end

function Scene:updateBasement()
    if self.basement == nil then
        self.basement = GameBasement.new()
    else
        self.basement:updateConfig()
    end
end

function Scene:getMonsterCount()
    return self.monsterNum
end

function Scene:onMonsterAdd(monster)
    self.monsters[tostring(monster.entityId)] = monster
    self.monsterNum = self.monsterNum + 1
end

function Scene:onMonsterDie(entityId)
    local monster = self.monsters[tostring(entityId)]
    if monster ~= nil then
        self.monsters[tostring(entityId)] = nil
        self.kills = self.kills + 1
        self.monsterNum = self.monsterNum - 1
        HostApi.updateMonsterInfo(GameMatch.level, GameMatch.levelMonsterNum - self.kills, 10 - GameMatch.level % 10)
    end
end

function Scene:onTick()
    for i, monster in pairs(self.monsters) do
        monster:onTick()
    end
end

function Scene:clearMonster()
    for i, monster in pairs(self.monsters) do
        monster:onRemove(false)
        self.monsters[tostring(monster.entityId)] = nil
    end
    self.kills = 0
    self.monsterNum = 0
    self.monsters = {}
end

function Scene:getMonsterByEntityId(entityId)
    for i, monster in pairs(self.monsters) do
        if monster.entityId == entityId then
            return monster
        end
    end
    return nil
end

function Scene:getBasementByEntityId(entityId)
    if self.basement == nil then
        return nil
    end
    if entityId == self.basement.entityId then
        return self.basement
    end
    return nil
end

function Scene:getMonsterPos()
    return self.monsterPos[math.random(1, #self.monsterPos)]
end

function Scene:getBossPos()
    return self.bossPos[math.random(1, #self.bossPos)]
end

function Scene:onPlayerQuit(player)
    for i, monster in pairs(self.monsters) do
        monster:onRemoveAttacker(player)
    end
end

function Scene:reset()
    self:clearMonster()
    self.basement:onRemove()
    self.basement = nil
end

return Scene