
require "config.XTools"
XTeamConfig = {}

XTeamConfig.teamConfigs = {}

function XTeamConfig:OnInit(configs)
    for _, config in pairs(configs) do
        self:NewTemplate(config)
    end
end

function XTeamConfig:NewTemplate(config)
    local configTab = {}
    configTab.teamId = tonumber(config.TeamId)
    configTab.nameColor = config.NameColor
    configTab.teamName = config.TeamName
    configTab.yaw = tonumber(config.Yaw)
    configTab.SpawnPos = XTools:CastToVector(config.SpawnPosX, config.SpawnPosY, config.SpawnPosZ)
    self.teamConfigs[configTab.teamId] = configTab
    return configTab
end

function XTeamConfig:GetTeamNameById(id)
    local team = self.teamConfigs[id]
    if type(team) == 'table' then
        return team.teamName
    end
    return nil
end

function XTeamConfig:getSpawnPosByTeamId(teamId)
    local team = self.teamConfigs[teamId]
    if team ~= nil  then
        return team.SpawnPos
    end
    return nil
end

function XTeamConfig:getTeams()
    return self.teamConfigs
end
