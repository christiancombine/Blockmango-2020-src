---
--- Created by Jimmy.
--- DateTime: 2018/4/25 0025 14:10
---

PlayerLevelConfig = {}
PlayerLevelConfig.levels = {}

function PlayerLevelConfig:init(levels)
    self:initPlayerLevels(levels)
end

function PlayerLevelConfig:initPlayerLevels(levels)
    for id, level in pairs(levels) do
        local item = {}
        item.level = tonumber(level.level)
        item.exp = tonumber(level.exp)
        self.levels[id] = item
    end
end

function PlayerLevelConfig:getUpgradeExpByLv(lv)
    local level = self.levels[tostring(lv)]
    if level then
        return level.exp
    end
    return GameConfig.defaultUpgradeExp
end

return PlayerLevelConfig