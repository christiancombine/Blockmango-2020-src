---
--- Created by Jimmy.
--- DateTime: 2018/6/13 0013 12:35
---
require "config.Area"
require "util.SkillUtil"

GameSkill = class()

function GameSkill:ctor(config, player, initPos)
    self.itemId = config.itemId
    self.length = config.length
    self.width = config.width
    self.height = config.height
    self.duration = config.duration
    self.damage = config.damage
    self.player = player
    self.initPos = initPos
    self.area = {}
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
    if self.itemId == ItemID.ENDER_PEARL then
        SkillUtil:useEnderPearl(self.initPos, self.player)
    end
    if self.itemId == ItemID.GRENADE_TREMOR then
        SkillUtil:createExplosion(self.initPos, self.player, self.damage)
        self:doExplosionDamage()
    end
    if self.itemId == ItemID.FROZEN_BALL then
        SkillUtil:useFrozenBall(self.area, self.player, self.duration * 1000)
    end
    self:onRemove()
end

function GameSkill:doExplosionDamage()
    local targets = {}
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isLife and player:getTeamId() ~= self.player:getTeamId() then
            local position = player:getPosition()
            if self.area:inArea(position) then
                table.insert(targets, player)
            end
        end
    end
    for _, player in pairs(targets) do
        player:subHealth(self.damage)
    end
end

function GameSkill:onRemove()
    self = {}
end

return GameSkill