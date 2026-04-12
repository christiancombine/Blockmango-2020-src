---
--- Created by Jimmy.
--- DateTime: 2017/10/31 0031 10:02
---
TeamConfig = {}

TeamConfig.teams = {}

function TeamConfig:init()
    for i = 1, 6 do
        self.teams[i] = {}
        self.teams[i].id = i
        self.teams[i].name = self:getTeamName(i)
        self.teams[i].color = self:getTeamColor(i)
    end
end

function TeamConfig:getTeam(id)
    local team = self.teams[id]
    if team == nil then
        team = self.teams[1]
    end
    return team
end

function TeamConfig:getTeamName(id)
    if id == 1 then
        return "Red"
    elseif id == 2 then
        return "Blue"
    elseif id == 3 then
        return "Green"
    elseif id == 4 then
        return "Yellow"
    elseif id == 5 then
        return "Purple"
    else
        return "Orange"
    end
end

function TeamConfig:getTeamColor(id)
    if id == 1 then
        return TextFormat.colorRed
    elseif id == 2 then
        return TextFormat.colorBlue
    elseif id == 3 then
        return TextFormat.colorGreen
    elseif id == 4 then
        return TextFormat.colorYellow
    elseif id == 5 then
        return TextFormat.colorPurple
    else
        return TextFormat.colorOrange
    end
end

return TeamConfig