---
--- Created by Jimmy.
--- DateTime: 2017/10/31 0031 10:01
---
require "config.TeamConfig"
require "Match"

GameTeam = class()

GameTeam.id = 0
GameTeam.name = "Red"
GameTeam.color = TextFormat.colorRed
GameTeam.curPlayers = 0

function GameTeam:ctor(config)
    self.id = config.id
    self.name = config.name
    self.color = config.color
end

function GameTeam:addPlayer()
    self.curPlayers = self.curPlayers + 1
end

function GameTeam:subPlayer()
    if self.curPlayers > 0 then
        self.curPlayers = self.curPlayers - 1
    end
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
