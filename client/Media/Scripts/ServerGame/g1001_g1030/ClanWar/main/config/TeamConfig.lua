---
--- Created by Jimmy.
--- DateTime: 2017/10/20 0020 14:49
---
require "config.Area"

TeamConfig = {}

TeamConfig.teams = {}

function TeamConfig:initTeams(teams)
    for i, v in pairs(teams) do
        local team = {}
        self.teams[i] = {}
        team.id = i
        team.name = v.teamName
        team.signPos = VectorUtil.newVector3i(v.signPos[1], v.signPos[2], v.signPos[3])
        team.bornArea = self:initBornArea(v.bornArea)
        team.flagArea = self:initFlagArea(v.flagArea)
        team.flagPlatforms = self:initFlagPlatforms(v.flagPlatforms)
        self.teams[i] = team
    end
end

function TeamConfig:initBornArea(bornArea)
    local born = {}
    born.area = Area.new(bornArea.area)
    born.bornLoc = VectorUtil.newVector3i(bornArea.bornLoc[1], bornArea.bornLoc[2], bornArea.bornLoc[3])
    return born
end

function TeamConfig:initFlagArea(flagArea)
    local flag = {}
    for i, v in pairs(flagArea) do
        flag[i] = {}
        flag[i].area = Area.new(v.area)
        flag[i].flagId = v.flagId
    end
    return flag
end

function TeamConfig:initFlagPlatforms(flagPlatforms)
    local platforms = {}
    for i, v in pairs(flagPlatforms) do
        local flagLoc = VectorUtil.newVector3i(v.flagLoc[1], v.flagLoc[2], v.flagLoc[3]);
        local key = VectorUtil.hashVector3(flagLoc)
        platforms[key] = {}
        platforms[key].flagLoc = flagLoc
        platforms[key].flagId = v.flagId
    end
    return platforms
end

function TeamConfig:getTeamConfig(id)
    return self.teams[id]
end

function TeamConfig:getTeamColor(id)
    if id == 1 then
        return TextFormat.colorRed
    else
        return TextFormat.colorBlue
    end
end

return TeamConfig