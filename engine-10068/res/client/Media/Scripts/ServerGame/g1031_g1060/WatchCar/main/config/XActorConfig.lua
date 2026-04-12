XActorConfig = {}
XActorConfig.configs = {}
function XActorConfig:OnInit(configs)
    for _, config in pairs(configs) do
        local tab = {}
        tab.ActorId = config.ActorId
        tab.ActorName = config.ActorName ~= '#' and config.ActorName or nil
        local teamId = tonumber(config.ActorBindTeamId)
        if self.configs[teamId] == nil then
            self.configs[teamId] = {}
        end
        table.insert(self.configs[teamId], tab)
    end
end

function XActorConfig:getRandActorByTeamId(teamId)
    local tab = self.configs[teamId]
    if type(tab) == "table" then
        local randNum = math.random(1, #tab)
        local actorCon = tab[randNum]
        local actorName = actorCon.ActorName
        return actorName
    end
    return nil
end
