---
--- Created by Jimmy.
--- DateTime: 2018/8/23 0023 11:56
---
GameTower = class()

function GameTower:ctor(config)
    self.config = config
    self.entityId = 0
    self.id = self.config.id
    self.teamId = self.config.teamId
    self.fullHp = self.config.hp
    self.hp = self.config.hp
    self.attack = self.config.attack
    self.defenseX = self.config.defenseX
    self.actor = self.config.actor
    self.position = self.config.position
    self.yaw = self.config.yaw
    self.type = self.config.type
    self.money = self.config.money
    self.buff = self.config.buff
    self.isDeath = false
    self.deathTime = 0
    self:onCreate()
end

function GameTower:onCreate()
    self.entityId = EngineWorld:addCreatureNpc(self.position, self.id, self.yaw, self.actor, function(entity)
        entity:setTeamId(self.teamId)
    end)
    GameManager:onAddTower(self)
end

function GameTower:onPlayerHit(player, attackType)
    if self.teamId == player:getTeamId() then
        return false
    end
    self:onAttacked(player)
    return true
end

function GameTower:onTick(ticks)
    if self.isDeath == false then
        return
    end
    if os.time() - self.deathTime >= 3 then
        self:buildRuins()
    end
end

function GameTower:buildRuins()
    self.isDeath = false
    GameRuins.new(self.config)
    GameManager:onRemoveTower(self)
end

function GameTower:onAttack(player)
    local damage = self.attack * (1 - player.defenseX) * AttackXConfig:getAttackX(self.type, player.type)
    player:onAttacked(damage)
    if self.buff.id ~= 0 and self.buff.time ~= 0 then
        player:addEffect(self.buff.id, self.buff.time, self.buff.lv)
    end
end

function GameTower:onAttacked(player)
    if self.hp <= 0 then
        return
    end
    local damage = player.attack * (1 - self.defenseX) * AttackXConfig:getAttackX(player.type, self.type)
    damage = math.max(0, math.floor(damage))
    player:addMoney(math.floor(damage / self.fullHp * self.money))
    self.hp = self.hp - damage
    if self.hp <= 0 then
        self:onDeath()
        GameMatch:showMsgTowerCollapsed(self.teamId)
    end
end

function GameTower:onDeath()
    self.isDeath = true
    self.deathTime = os.time()
    if self.entityId ~= 0 then
        EngineWorld:getWorld():killCreature(self.entityId)
        self.entityId = 0
    end
end

function GameTower:onClear()
    if self.entityId ~= 0 then
        EngineWorld:removeEntity(self.entityId)
        self.entityId = 0
    end
    GameManager:onRemoveTower(self)
end

return GameTower