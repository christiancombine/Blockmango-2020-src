---
--- Created by Jimmy.
--- DateTime: 2017/10/30 0030 10:06
---
require "config.Area"
require "config.ProduceConfig"

TeamConfig = {}

TeamConfig.teams = {}

function TeamConfig:init(teams)
    for i, v in pairs(teams) do
        local team = {}
        team.id = v.id
        team.name = v.name
        team.initPos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        team.area = Area.new(v.area)
        team.produce = ProduceConfig.new(v.produce)
        self.teams[v.id] = team
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
    else
        return TextFormat.colorBlue
    end
end

return TeamConfig