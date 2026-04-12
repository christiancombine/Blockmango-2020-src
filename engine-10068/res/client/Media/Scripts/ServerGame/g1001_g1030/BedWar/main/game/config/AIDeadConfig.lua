AIDeadConfig = {}
AIDeadConfig.Config = {}

function AIDeadConfig:init(config)
    for _, v in pairs(config) do
        local item = {}
        item.index = tonumber(v.index)
        item.groupId = tonumber(v.groupId)
        item.teamId = tonumber(v.teamId)
        item.deadWeight = tonumber(v.deadWeight)
        self.Config[item.index] = item
    end
end

function AIDeadConfig:getConfig(teamId)
    for _, config in pairs(self.Config) do
        if tonumber(config.teamId) == tonumber(teamId) then
            return config.deadWeight
        end
    end
    return 9999
end