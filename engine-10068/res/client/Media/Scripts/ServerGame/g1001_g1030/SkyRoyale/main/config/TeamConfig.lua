TeamConfig = {}
TeamConfig.teams = {}

function TeamConfig:initTeams(teams)
    for _, team in pairs(teams) do
        local data = {}
        data.id = tonumber(team.id)
        data.name = team.name
        data.initPosX = tonumber(team.initPosX)
        data.initPosY = tonumber(team.initPosY)
        data.initPosZ = tonumber(team.initPosZ)
        data.startX = tonumber(team.startX)
        data.startY = tonumber(team.startY)
        data.startZ = tonumber(team.startZ)
        data.endX = tonumber(team.endX)
        data.endY = tonumber(team.endY)
        data.endZ = tonumber(team.endZ)
        data.respawnStartX = tonumber(team.respawnStartX)
        data.respawnStartY = tonumber(team.respawnStartY)
        data.respawnStartZ = tonumber(team.respawnStartZ)
        data.respawnEndX = tonumber(team.respawnEndX)
        data.respawnEndY = tonumber(team.respawnEndY)
        data.respawnEndZ = tonumber(team.respawnEndZ)
        self.teams[data.id] = data
    end
end

function TeamConfig:getTeam(id)
    local team = self.teams[id]
    if team == nil then
        team = self.teams[1]
    end
    return team
end

function TeamConfig:getTeamColor(id)
    if id == 1 then
        return TextFormat.colorRed
    elseif id == 2 then
        return TextFormat.colorBlue
    elseif id == 3 then
        return TextFormat.colorGreen
    else
        return TextFormat.colorYellow
    end
end

function TeamConfig:removeFootBlock()

    for _, team in pairs(self.teams) do
        local startX = math.min(team.startX, team.endX)
        local endX = math.max(team.startX, team.endX)
        local startY = math.min(team.startY, team.endY)
        local endY = math.max(team.startY, team.endY)
        local startZ = math.min(team.startZ, team.endZ)
        local endZ = math.max(team.startZ, team.endZ)

        for x = startX, endX do
            for y = startY, endY + 1 do
                for z = startZ, endZ do
                    local vec = VectorUtil.newVector3i(x, y - 1, z)
                    EngineWorld:setBlockToAir(vec)
                end
            end
        end
    end
end

return TeamConfig