---
--- Created by Jimmy.
--- DateTime: 2018/5/15 0015 9:44
---
require "config.BasementConfig"
require "Match"

GameBasement = class()

function GameBasement:ctor()
    self.entityId = 0
    self.config = {}
    self.level = 0
    self.hp = 0
    self.fullHp = 0
    self.defense = 0
    self.isRemove = false
    self.updateUiTime = os.time()
    self:createBasement()
end

function GameBasement:updateConfig()
    local percentage = self.hp / self.fullHp
    self.config = BasementConfig:getBasementById(GameMatch.curGameLevel.basementId)
    self.level = self.config.level
    self.fullHp = self.config.hp
    if self.level == 1 then
        self.hp = self.fullHp
    else
        self.hp = math.min(self.fullHp * (percentage + GameConfig.BasementUpgradeHealthReply), self.fullHp)
    end
    self.defense = self.config.defense
    HostApi.updateBasementLife(self.hp, self.fullHp)
end

function GameBasement:createBasement()
    self:updateConfig()
    local scene = GameMatch:getCurGameScene()
    local pos = scene.basementPos
    self.entityId = EngineWorld:addCreatureNpc(pos.position, self.config.id, pos.yaw, "basement.actor")
end

function GameBasement:onAttacked(thower)
    local damage = thower.attackX * (thower.attack ^ 2 / (thower.attack + self.defense))
    damage = math.max(0, damage)
    if damage > 0 then
        self.hp = math.max(0, self.hp - damage)
    end
    if self.hp <= 0 then
        self:onDie()
    else
        if os.time() - self.updateUiTime >= 2 then
            HostApi.updateBasementLife(self.hp, self.fullHp)
            self.updateUiTime = os.time()
        end
    end
end

function GameBasement:onRemove()
    if self.isRemove == false then
        EngineWorld:removeEntity(self.entityId)
        HostApi.updateBasementLife(0, 0)
    end
end

function GameBasement:onDie()
    EngineWorld:getWorld():killCreature(self.entityId)
    GameMatch:doGameOver(false)
    self.isRemove = true
    HostApi.updateBasementLife(0, 0)
end