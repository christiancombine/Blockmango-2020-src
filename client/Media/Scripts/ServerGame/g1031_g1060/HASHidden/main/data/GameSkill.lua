---
--- Created by leilei.
--- DateTime: 2018/6/13 0013 12:35
---
require "data.Area"
require "util.SkillManager"
require "data.GamePlayer"

GameSkill = class()


function GameSkill:ctor(config, player, initPos)
    self.length = config.length
    self.width = config.width
    self.height = config.height
    self.duration = config.duration
    self.effectType = config.effectType
    self.damage = config.damage
    self.occupationId = config.occupationId
    self.player = player
    self.initPos = initPos
    self.targets = {}
    self.records = {}
    self.area = {}
    self.itemId = config.itemId
    if self.occupationId == GamePlayer.ROLE_SEEK then
        self.initPos = initPos
    else
        self.initPos = player:getPosition()
    end
    self:initArea()
    self:doAction()
end

function GameSkill:initArea()
    local matrix = {}
    matrix[1] = {}
    matrix[1][1] = self.initPos.x - self.length
    matrix[1][2] = self.initPos.y - self.height
    matrix[1][3] = self.initPos.z - self.width
    matrix[2] = {}
    matrix[2][1] = self.initPos.x + self.length
    matrix[2][2] = self.initPos.y + self.height
    matrix[2][3] = self.initPos.z + self.width
    self.area = Area.new(matrix)
end

function GameSkill:doAction()
    if self.occupationId == 1 then
        SkillManager:useColorfulGenades(self.initPos, self.area, self.player, self.damage, self.duration)
    end
    if self.occupationId == 2 then
        self.player.addSpeed = true
        self.player:setSpeedAddition(self.player.speed)
    end
end

function GameSkill:onRemove()
    self = {}
end

return GameSkill