AITeamConfig = {}
AITeamConfig.Config = {}

function AITeamConfig:init(config)
    for _, v in pairs(config) do
        local item = {}
        item.index = tonumber(v.index)
        item.gameTime = tonumber(v.gameTime)
        item.teamNum = tonumber(v.teamNum)
        self.Config[item.index] = item
    end
end

function AITeamConfig:getConfig(gameTime)
    for _, config in pairs(self.Config) do
        if tonumber(gameTime) < tonumber(self.Config[1].gameTime) then
            return  config.teamNum
        end
    end
    return self.Config[#self.Config].teamNum
end