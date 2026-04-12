---
--- Created by Jimmy.
--- DateTime: 2018/8/21 0021 15:43
---
require "config.BasementConfig"

GameBasement = class()

function GameBasement:ctor(config)
    self.entityId = 0
    self.hp = config.HP
    self.maxhp = config.HP
    self.attack = config.Attack
    self.defenseX = config.DefenseX
    self.teamId = config.TeamID
    self.type = config.Type
    self.money = config.Money
    self.team = GameMatch.Teams[config.TeamID]
    if self.team == nil then
        error("basement team is nil --> teamId:" .. config.TeamID)
    end
    self.isDeath = false
    self.isNeedSync = false
    self.team:setBasement(self)
    self:onCreate(config)
end

function GameBasement:onCreate(basement)
    local pos = basement.Vec3
    local yaw = basement.Yaw
    local id = basement.ID
    local actor = basement.Actor
    self.entityId = EngineWorld:addCreatureNpc(pos, id, yaw, actor, function(entity)
        entity:setTeamId(self.teamId)
    end)
    GameManager:onAddBasements(self)
end

function GameBasement:onTick(ticks)
    if os.time() % 5 ~= 0 then
        return
    end
    if self.isNeedSync == false then
        return
    end
    GameManager:updateBasementLife()
    self.isNeedSync = false
end

function GameBasement:onPlayerHit(player, attackType)
    if self.teamId == player:getTeamId() then
        return false
    end
    self:onAttacked(player)
    return true
end

function GameBasement:onAttack(player)
    local damage = self.attack * (1 - player.defenseX) * AttackXConfig:getAttackX(self.type, player.type)
    player:onAttacked(damage)
end

function GameBasement:onAttacked(player)
    if self.hp <= 0 then
        return
    end
    self.isNeedSync = true
    local damage = player.attack * (1 - self.defenseX) * AttackXConfig:getAttackX(player.type, self.type)
    damage = math.max(0, math.floor(damage))
    player:addMoney(math.floor(damage / self.maxhp * self.money))
    self.hp = self.hp - damage
    player:addPlayerScore(math.ceil(damage / self.maxhp * 20))
    if self.hp <= 0 then
        GameMatch:showMsgBasementCollapsed(self.teamId)
        self:onDeath()
    else
        GameMatch:showMsgBasementUnderAttack(self.teamId)
    end
end

function GameBasement:onPlayerQuit()
    if self.team:isDeath() then
        self:onDeath()
        self.isNeedSync = true
    end
end

function GameBasement:isDead()
    return self.isDeath
end

function GameBasement:onDeath()
    self.isDeath = true
    self:killSameTeamPlayers()
    self:killSameTeamTower()
    self:killSameTeamRuins()
    if self.entityId > 0 then
        EngineWorld:getWorld():killCreature(self.entityId)
        self.entityId = 0
    end
    self.hp = 0
    GameMatch:ifGameOverByTeam()
end

function GameBasement:killSameTeamPlayers()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player:getTeamId() == self.teamId then
            player:subHealth(99999)
        end
    end
end

function GameBasement:killSameTeamTower()
    GameManager:clearTowers(self.teamId)
end

function GameBasement:killSameTeamRuins()
    GameManager:clearRuins(self.teamId)
end

function GameBasement:onClear()
    if self.entityId ~= 0 then
        EngineWorld:removeEntity(self.entityId)
        self.entityId = 0
    end
    GameManager:onRemoveBasements(self)
end

return GameBasement

