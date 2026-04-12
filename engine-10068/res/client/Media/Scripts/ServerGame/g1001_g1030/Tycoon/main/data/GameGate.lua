---
--- Created by Jimmy.
--- DateTime: 2018/7/5 0005 10:35
---
require "config.BasicConfig"
require "data.GameGuard"
require "data.GameGateBuilder"

GameGate = class()
function GameGate:ctor(domain, config)
    self.timer = 0
    self.isCanClose = false
    self.domain = domain
    self.config = config
    self.guards = {}
    self.isOpen = true
    self.blockPos = {}
    self.pos = BasicConfig:getPosViByBasic(self.domain.basicId, self.config.startPos)
    self.xImage, self.zImage = BasicConfig:getXandZImageByBasics(self.domain.basicId)
    self:onCreate()
end

--生成门购买NPC
function GameGate:onCreate()
    local npcPos = BasicConfig:getPosByBasic(self.domain.basicId, self.config.gateNpcPos)
    local yaw = BasicConfig:getYawByBasic(self.domain.basicId, self.config.gateNpcYaw)
    GameGateBuilder.new(self, npcPos, yaw, self.config.gateNpcActor, self.config.gateNpcName)
    self.domain:addGateBuildingRecord(self.config.id)
end

function GameGate:toggleGate()
    if self.isOpen then
        self:closeGate()
    else
        self:openGate()
    end
end

function GameGate:onTick()
    if self.isOpen == true and self.isCanClose then
        if self.timer >= 1 then
            self:closeGate()
            self.timer = 0
        end
        self.timer = self.timer + 1
    end
end

function GameGate:openGate()
    self.isCanClose = false
    self.isOpen = true
    EngineWorld:destroySchematic(self.config.fileName, self.pos, self.xImage, self.zImage)
end

function GameGate:closeGate()
    self.isOpen = false
    EngineWorld:createSchematic(self.config.fileName, self.pos, self.xImage, self.zImage)
end

function GameGate:getPlayer()
    return self.domain.player
end

function GameGate:getPrice()
    return self.config.price
end

function GameGate:getValue()
    return self.config.value
end

function GameGate:buildGuard()
    self.domain.gate = self
    self.domain:addBuildProgress(self.config.value)
    local guardPos = BasicConfig:getPosByBasic(self.domain.basicId, self.config.guard1.pos)
    local yaw = BasicConfig:getYawByBasic(self.domain.basicId, self.config.guard1.yaw)
    self.guards[1] = GameGuard.new(self, guardPos, yaw, self.config.guardActor, self.config.guardName)
    guardPos = BasicConfig:getPosByBasic(self.domain.basicId, self.config.guard2.pos)
    yaw = BasicConfig:getYawByBasic(self.domain.basicId, self.config.guard2.yaw)
    self.guards[2] = GameGuard.new(self, guardPos, yaw, self.config.guardActor, self.config.guardName)
    local player = self.domain.player
    player:AddGrardToList(self.guards[1])
    player:AddGrardToList(self.guards[2])
    self:closeGate()
end

return GameGate
