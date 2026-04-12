---
--- Created by Jimmy.
--- DateTime: 2018/4/27 0027 10:25
---

GameMonster = class()

function GameMonster:ctor(config, level)
    self.entityId = 0
    self.fullHp = 0
    self.hp = 0
    self.attack = 0
    self.defense = 0
    self.addHp = 0
    self.attackX = 0
    self.exp = 0
    self.gold = 0
    self.actor = "zombie.actor"
    self.config = config
    self.level = level
    self.attackPlayers = {}
    self.createTime = os.time()
    self.changeTargetTime = 0
    self.attackTargetId = 0
    self:initMonsterAttr(config)
    self:addMonsterToWorld()
end

function GameMonster:initMonsterAttr(config)
    if config.isUp then
        self.hp = config.hp * (1 + GameConfig.growthX * self.level ^ GameConfig.correctionX)
        self.attack = config.attack * (1 + GameConfig.growthX * self.level ^ GameConfig.correctionX)
        self.defense = config.defense * (1 + GameConfig.growthX * self.level ^ GameConfig.correctionX)
        self.addHp = config.addHp * (1 + GameConfig.growthX * self.level ^ GameConfig.correctionX)
    else
        self.hp = config.hp
        self.attack = config.attack
        self.defense = config.defense
        self.addHp = config.addHp
    end
    self.fullHp = self.hp
    self.actor = config.actor or "zombie.actor"
    self.exp = config.exp
    self.gold = config.gold
    self.attackX = config.attackX
end

function GameMonster:addMonsterToWorld()
    local scene = GameMatch:getCurGameScene()
    local postion, targetPos
    if self.config.isBoss then
        postion = scene:getBossPos()
        HostApi.updateBossBloodStrip(self.hp, self.fullHp)
    else
        postion = scene:getMonsterPos()
    end
    self.entityId = EngineWorld:addCreatureNpc(postion.initPos, self.config.id, postion.yaw, self.config.actor)
    scene:onMonsterAdd(self)
end

function GameMonster:onTick()
    if (os.time() - self.createTime) % 10 == 0 then
        self:onLifeRecovery()
    end
end

function GameMonster:onLifeRecovery()
    self.hp = math.min(self.fullHp, self.hp + self.addHp)
end

function GameMonster:onRemoveAttacker(attacker)
    if self.attackPlayers[tostring(attacker.userId)] then
        self.attackPlayers[tostring(attacker.userId)] = nil
    end
end

function GameMonster:onAttacked(player)
    local damage = player.attackX * (player.attack ^ 2 / (player.attack + self.defense))
    damage = math.min(damage, self.hp)
    if damage == 0 then
        return
    end
    local attacker = self.attackPlayers[tostring(player.userId)]
    if attacker == nil then
        attacker = {}
        attacker.player = player
        attacker.damage = damage
        attacker.isKiller = false
        self.attackPlayers[tostring(player.userId)] = attacker
    else
        attacker.damage = attacker.damage + damage
    end
    self.hp = self.hp - damage
    if self.hp <= 0 then
        attacker.isKiller = true
        self:onDie()
        player:onKill()
        GameMatch:getCurGameScene():onMonsterDie(self.entityId)
    end
    if self.config.isBoss then
        HostApi.updateBossBloodStrip(self.hp, self.fullHp)
    end
    self:tryChangeTarget()
end

function GameMonster:tryChangeTarget()
    if self.hp <= 0 then
        return
    end
    if self.config.isChangeTarget == 0 then
        return
    end
    if os.time() - self.changeTargetTime < self.config.changeTargetCd then
        return
    end
    local MaxDamageAttacker
    for userId, attacker in pairs(self.attackPlayers) do
        if MaxDamageAttacker == nil then
            MaxDamageAttacker = attacker
        end
        if MaxDamageAttacker.damage < attacker.damage then
            MaxDamageAttacker = attacker
        end
    end
    if MaxDamageAttacker ~= nil and self.attackTargetId ~= MaxDamageAttacker.player:getEntityId() then
        self.changeTargetTime = os.time()
        self.attackTargetId = MaxDamageAttacker.player:getEntityId()
        EngineWorld:getWorld():changeCreatureAttackTarget(self.entityId, self.attackTargetId)
    end
end

function GameMonster:onAttack(player)
    local damage = self.attackX * (self.attack ^ 2 / (self.attack + player.defense))
    player:onAttacked(damage, self.config.effect)
end

function GameMonster:getTotalDamage()
    local damage = 0
    for userId, attacker in pairs(self.attackPlayers) do
        damage = damage + attacker.damage
    end
    return damage
end

function GameMonster:onDie()
    local totalDamage = self:getTotalDamage()
    for userId, attacker in pairs(self.attackPlayers) do
        local exp = self.exp * GameConfig.basePercentage * attacker.damage / totalDamage
        local gold = self.gold * attacker.damage / totalDamage
        if attacker.isKiller then
            exp = exp + self.exp * GameConfig.killPercentage
        end
        attacker.player:addExp(exp)
        attacker.player:addMoney(gold)
    end
    self:onRemove(true)
end

function GameMonster:onRemove(isDie)
    self.attackPlayers = {}
    if isDie then
        EngineWorld:getWorld():killCreature(self.entityId)
    else
        EngineWorld:removeEntity(self.entityId)
    end
    if self.config.isBoss then
        HostApi.updateBossBloodStrip(0, 0)
    end
end