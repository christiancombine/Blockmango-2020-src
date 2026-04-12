XGameTeam = class()
XGameTeam.teamId = 0
XGameTeam.teamName = ""

XGameTeam.spawnPosition = {}
XGameTeam.curPlayerCount = 0
XGameTeam.bindPlayers = {}

function XGameTeam:ctor(teamConfig)
    self.teamId = teamConfig.teamId
    self.teamName = teamConfig.teamName
    self.nameColor = teamConfig.nameColor
    self.spawnPosition = teamConfig.SpawnPos
    self.curPlayerCount = 0
    self.bindPlayers = {}
    self.yaw = teamConfig.yaw
end

function XGameTeam:addPlayer(player)
    local id = player:getEntityId()
    if id ~= nil then
        self.bindPlayers[id] = player
        self.curPlayerCount = self.curPlayerCount + 1
    end
end

function XGameTeam:removePlayer(player)
    local id = player:getEntityId()
    if id ~= nil then
        self.bindPlayers[id] = nil
        self.curPlayerCount = self.curPlayerCount - 1
    end
end

function XGameTeam:getTeamId()
    return self.teamId
end

function XGameTeam:getTeamName()
    if self.teamName == nil or self.teamName == "" then
        return nil
    end
    return self.teamName
end
