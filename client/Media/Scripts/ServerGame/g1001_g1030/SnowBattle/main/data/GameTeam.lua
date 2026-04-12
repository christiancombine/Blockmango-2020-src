---
--- Created by Jimmy.
--- DateTime: 2017/10/19 0019 15:36
---
require "config.TeamConfig"
require "Match"

GameTeam = class()

GameTeam.id = 0
GameTeam.name = "Red"
GameTeam.initPos = {}
GameTeam.area = {}
GameTeam.curPlayers = 0
GameTeam.killNum = 0
GameTeam.color = TextFormat.colorRed

function GameTeam:ctor(config)
    self.id = config.id
    self.name = config.name
    self.initPos = config.initPos
    self.area = config.area
    self.color = TeamConfig:getTeamColor(self.id)
    self.killNum = 0
    self:addCdSignOnProduce()
end

function GameTeam:addPlayer()
    self.curPlayers = self.curPlayers + 1
end

function GameTeam:subPlayer()
    if self.curPlayers > 0 then
        self.curPlayers = self.curPlayers - 1
    end
    if GameMatch.hasStartGame and self.curPlayers == 0 then
        self.isHaveBed = false
    end
end

function GameTeam:addKillNum()
    self.killNum = self.killNum + 1
end

function GameTeam:subKillNum()
    self.killNum = math.max(0, self.killNum - 1)
end

function GameTeam:getKillNum()
    return self.killNum
end

function GameTeam:inArea(vec3)
    return self.area:inArea(vec3)
end

function GameTeam:inBlockPlaceArea(vec3)
    return self.area:inBlockPlaceArea(vec3)
end

function GameTeam:isDeath()
    return self.curPlayers == 0
end

function GameTeam:getDisplayName()
    return self.color .. "[" .. self.name .. "]"
end

function GameTeam:getDisplayStatus()
    return self.color .. self.name .. " : " .. self.curPlayers
end

function GameTeam:getDisplayKillNum()
    return self:getDisplayStatus() .. " | " .. self.killNum
end

function GameTeam:addCdSignOnProduce()
    for k, v in pairs(TeamConfig:getTeam(self.id).produce:getAllProduce()) do
        if v.enableCD then
            local signPos = VectorUtil.newVector3i(v.signPos.x, v.signPos.y, v.signPos.z)
            EngineWorld:setBlock(signPos, 63)
            EngineWorld:getWorld():resetSignText(signPos, 1, "00:00")
        end
    end
end
