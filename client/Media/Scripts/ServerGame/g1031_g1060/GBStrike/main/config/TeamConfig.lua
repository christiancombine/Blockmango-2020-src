---
--- Created by Jimmy.
--- DateTime: 2018/9/25 0025 11:37
---
require "data.GameTeam"

TeamConfig = {}

function TeamConfig:init(teams)
    self:initTeams(teams)
end

function TeamConfig:initTeams(teams)
    for _, team in pairs(teams) do
        table.insert(GameMatch.Teams, GameTeam.new(team))
    end
    table.sort(GameMatch.Teams, function(a, b)
        return a.id < b.id
    end)
end

return TeamConfig