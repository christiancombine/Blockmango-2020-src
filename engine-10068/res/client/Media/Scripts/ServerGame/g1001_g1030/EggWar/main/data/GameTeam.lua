---
--- Created by Jimmy.
--- DateTime: 2017/10/19 0019 15:36
---
require "data.GameMerchant"
require "Match"

GameTeam = class()

GameTeam.id = 0
GameTeam.name = "Red"
GameTeam.initPos = {}
GameTeam.bedPos = {}
GameTeam.curPlayers = 0
GameTeam.color = TextFormat.colorRed
GameTeam.isHaveBed = true
GameTeam.storeId = 0
GameTeam.merchant = nil

function GameTeam:ctor(config, color)
    self.id = config.id
    self.name = config.name
    self.initPos = config.initPos
    self.bedPos = config.bedPos
    self.color = color
    self.merchant = GameMerchant.new(self.id, config.storePos, config.storeYaw, config.storeName)
    GameMatch.Merchants[self.id] = self.merchant
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

function GameTeam:isDeath()
    return self.curPlayers == 0
end

function GameTeam:destroyBed()
    self.isHaveBed = false
    HostApi.sendBedDestroy(self.id)
end

function GameTeam:getDisplayName()
    return self.color .. "[" .. self.name .. "]"
end

function GameTeam:getDisplayStatus()
    if self.isHaveBed then
        return self.color .. self.name .. "[O]"
    else
        return self.color .. self.name .. "[X]"
    end
end
