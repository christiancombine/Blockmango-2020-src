---
--- Created by Jimmy.
--- DateTime: 2017/10/20 0020 14:49
---

TeamConfig = {}

TeamConfig.teams = {}

function TeamConfig:initTeams(teams)
    for i, v in pairs(teams) do
        self.teams[v.id] = {}
        self.teams[v.id].id = v.id
        self.teams[v.id].name = v.name
        self.teams[v.id].initPos = VectorUtil.newVector3i(v.initPos[1], v.initPos[2], v.initPos[3])
        self.teams[v.id].storePos = VectorUtil.newVector3(v.storePos[1], v.storePos[2], v.storePos[3])
        self.teams[v.id].storeYaw = tonumber(v.storePos[4])
        self.teams[v.id].storeName = v.storeName
        self.teams[v.id].eggPos = VectorUtil.newVector3(v.eggPos[1], v.eggPos[2], v.eggPos[3])
        self.teams[v.id].eggYaw = tonumber(v.eggPos[4])
        self.teams[v.id].eggActor = tostring(v.eggActor)
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
    elseif id == 2 then
        return TextFormat.colorBlue
    elseif id == 3 then
        return TextFormat.colorGreen
    else
        return TextFormat.colorYellow
    end
end

function TeamConfig:prepareEgg()
    for i, v in pairs(self.teams) do
        local entityId = EngineWorld:addActorNpc(v.eggPos, v.eggYaw, v.eggActor, function(entity)
            entity:setHeadName(v.name or "")
        end)
        GameMatch.egg[v.id] = {}
        GameMatch.egg[v.id].id = v.id
        GameMatch.egg[v.id].entityId = entityId
        GameMatch.egg[v.id].name = v.name
    end
end

return TeamConfig