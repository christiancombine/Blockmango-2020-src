GameTeam = class()

GameTeam.id = 0
GameTeam.name = "Red"
GameTeam.initPos = {}
GameTeam.curPlayers = 0
GameTeam.color = TextFormat.colorRed
GameTeam.resource = 0
GameTeam.maxResource = 0
GameTeam.isResourceFull = false
GameTeam.isWin = false


function GameTeam:ctor(config, color)
    self.id = tonumber(config.id)
    self.name = config.name
    self.initPos = config.initPos
    self.yaw = config.yaw
    self.exchangePos = config.exchangePos
    self.color = color
    self.resource = 0
    self.maxResource = tonumber(config.maxResource)
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

function GameTeam:addResource(resource)
    if resource ~= 0 then
        self.resource = self.resource + resource
        if self:isFullResource() then
            self.resource = self.maxResource
        end
    end
end

function GameTeam:isFullResource()
    return self.resource >= self.maxResource
end

return GameTeam