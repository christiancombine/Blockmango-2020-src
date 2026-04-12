---
--- Created by Jimmy.
--- DateTime: 2017/10/19 0019 15:36
---
require "config.GameConfig"
require "config.TeamConfig"
require "config.FlagConfig"
require "Match"

GameTeam = class()

GameTeam.id = 0

GameTeam.config = {}

GameTeam.name = "Red"
GameTeam.initPos = {}
GameTeam.color = TextFormat.colorRed

GameTeam.flags = 0
GameTeam.kills = 0
GameTeam.dies = 0
GameTeam.curPlayers = 0
GameTeam.score = 0

GameTeam.isHaveBed = true
GameTeam.openChestCache = {}
GameTeam.flagCache = {}

function GameTeam:ctor(id)
    self.id = id
    self:init()
end

function GameTeam:init()
    self.config = TeamConfig:getTeamConfig(self.id)
    self.name = self.config.name
    self.initPos = self.config.bornArea.bornLoc
    self.color = TeamConfig:getTeamColor(self.id)
    EngineWorld:setBlock(self.config.signPos, 63, 20 - 8 * self.id)
    EngineWorld:getWorld():resetSignText(self.config.signPos, 1, Messages:getFlagHint())
    self:reset()
end

function GameTeam:reset()
    self.flags = 0
    self.kills = 0
    self.dies = 0
    self.score = 0
    self:resetRandomInv()
    self.openChestCache = {}
    self.flagCache = {}
end

function GameTeam:resetRandomInv()
    self:prepareRandomInv(self.config.bornArea.area.randomInv)
    for i, v in pairs(self.config.flagArea) do
        self:prepareRandomInv(v.area.randomInv)
    end
end

function GameTeam:prepareRandomInv(randomInv)
    for i, v in pairs(randomInv) do
        local invLoc = {}
        local locBegin = v.beginPos
        local locEnd = v.endPos
        local height = v.height
        invLoc.x = locBegin.x
        invLoc.y = locBegin.y
        invLoc.z = locBegin.z
        while (invLoc.x ~= locEnd.x or invLoc.z ~= locEnd.z) do
            if (invLoc.x ~= locEnd.x) then
                if invLoc.x > locEnd.x then
                    invLoc.x = invLoc.x - 1
                else
                    invLoc.x = invLoc.x + 1
                end
            end
            if invLoc.z ~= locEnd.z then
                if invLoc.z > locEnd.z then
                    invLoc.z = invLoc.z - 1
                else
                    invLoc.z = invLoc.z + 1
                end
            end
            for i = 1, height do
                invLoc.y = locBegin.y + i - 1
                EngineWorld:setBlock(VectorUtil.newVector3i(invLoc.x, invLoc.y, invLoc.z), 54)
            end
        end
    end
end

function GameTeam:locInBornArea(vec3)
    return self.config.bornArea.area:inArea(vec3)
end

function GameTeam:locInFlagArea(vec3)
    for i, v in pairs(self.config.flagArea) do
        if v.area:inArea(vec3) then
            return true, v
        end
    end
    return false, nil
end

function GameTeam:onChestOpen(vec3, inv)

    if self.openChestCache[VectorUtil.hashVector3(vec3)] then
        return
    end

    if self:locInBornArea(vec3) then
        self.openChestCache[VectorUtil.hashVector3(vec3)] = true
        inv:clearChest()
        InventoryUtil:fillItem(inv, GameConfig.inventory[1])
    else
        local isInFlagArea, flagArea = self:locInFlagArea(vec3)
        if isInFlagArea and flagArea ~= nil then
            self.openChestCache[VectorUtil.hashVector3(vec3)] = true
            inv:clearChest()
            local flag = FlagConfig.flag[flagArea.flagId]
            if flag ~= nil then
                inv:initByItem(0, flag.id, 10, flag.meta)
            end
        end
    end
end

function GameTeam:onBlockPlace(blockId, blockMeta, vec3)
    local key = VectorUtil.hashVector3(vec3)
    local flagPlatform = self.config.flagPlatforms[key]
    if flagPlatform == nil then
        if self:locInBornArea(vec3) or self:locInFlagArea(vec3) then
            return false, false
        end
    else
        local flag = FlagConfig.flag[flagPlatform.flagId]
        if flag ~= nil and flag.id == blockId and flag.meta == blockMeta then
            if self:onPlayerPlaceFlag(flagPlatform.flagId) then
                return true, true
            end
        end
        return false, true
    end
    return true, false
end

function GameTeam:onPlayerDeath()
    self.dies = self.dies + 1
    if self.score + GameMatch.SCORE_DIE < 0 then
        self.score = 0
    else
        self.score = self.score + GameMatch.SCORE_DIE
    end
end

function GameTeam:onPlayerKill()
    self.kills = self.kills + 1
    self.score = self.score + GameMatch.SCORE_KILL
end

function GameTeam:onPlayerPlaceFlag(flagId)
    if self.flagCache[flagId] ~= true then
        self.flags = self.flags + 1
        self.score = self.score + GameMatch.SCORE_FLAG
        self.flagCache[flagId] = true
        return true
    end
    return false
end

function GameTeam:addPlayer()
    self.curPlayers = self.curPlayers + 1
end

function GameTeam:subPlayer()
    if self.curPlayers > 0 then
        self.curPlayers = self.curPlayers - 1
    end
end

function GameTeam:getDisplayName()
    return self.color .. "[" .. self.name .. "]"
end

