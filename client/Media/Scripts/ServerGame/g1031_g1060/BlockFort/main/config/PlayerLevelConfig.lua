PlayerLevelConfig = {}
PlayerLevelConfig.playerLevel = {}

function PlayerLevelConfig:init(config)
    for _, v in pairs(config) do
        local data = {}
        data.level = tonumber(v.level)
        data.exp = tonumber(v.exp)
        self.playerLevel[data.level] = data
    end
end

function PlayerLevelConfig:getExpByLevel(level)
    level = tonumber(level)
    if self.playerLevel[level] ~= nil then
        return self.playerLevel[level].exp
    end

    return 0
end

function PlayerLevelConfig:getMaxLevel()
    return #self.playerLevel
end

return PlayerLevelConfig