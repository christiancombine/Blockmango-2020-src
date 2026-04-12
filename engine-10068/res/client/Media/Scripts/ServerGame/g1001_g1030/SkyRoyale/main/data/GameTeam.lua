GameTeam = class()

GameTeam.id = 0
GameTeam.name = "Red"
GameTeam.curPlayers = 0
GameTeam.color = TextFormat.colorRed
GameTeam.isWin = false
GameTeam.kills = 0
GameTeam.deathTimes = 0

gameResult = 
{
    WIN = 0,
    LOSE = 1,
    DRAW = 2
}

function GameTeam:ctor(config, color)
    self.id = tonumber(config.id)
    self.name = config.name
    self.color = color
    self.initPosX = config.initPosX
    self.initPosY = config.initPosY
    self.initPosZ = config.initPosZ
    self.startX = config.startX
    self.startY = config.startY
    self.startZ = config.startZ
    self.endX = config.endX
    self.endY = config.endY
    self.endZ = config.endZ
    self.respawnStartX = config.respawnStartX
    self.respawnStartY = config.respawnStartY
    self.respawnStartZ = config.respawnStartZ
    self.respawnEndX = config.respawnEndX
    self.respawnEndY = config.respawnEndY
    self.respawnEndZ = config.respawnEndZ
end

function GameTeam:getGameResult()
    return gameResult.WIN
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

function GameTeam:getTeleportPos()
    local x = self.initPosX
    local y = self.initPosY
    local z = self.initPosZ

    return VectorUtil.newVector3(x, y, z)
end

function GameTeam:getRespawnPos()
    local x = math.random(math.min(self.respawnStartX, self.respawnEndX), math.max(self.respawnStartX, self.respawnEndX))
    local y = math.random(math.min(self.respawnStartY, self.respawnEndY), math.max(self.respawnStartY, self.respawnEndY))
    local z = math.random(math.min(self.respawnStartZ, self.respawnEndZ), math.max(self.respawnStartZ, self.respawnEndZ))

    return VectorUtil.newVector3(x, y, z)
end

return GameTeam