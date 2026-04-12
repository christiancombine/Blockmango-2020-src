---
--- Created by Jimmy.
--- DateTime: 2018/9/25 0025 15:36
---
RespawnConfig = {}
RespawnConfig.respawns = {}

function RespawnConfig:init(respawns)
    self:initRespawn(respawns)
end

function RespawnConfig:initRespawn(respawns)
    for _, respawn in pairs(respawns) do
        local item = {}
        item.id = respawn.id
        item.teamId = tonumber(respawn.teamId)
        item.waitTime = tonumber(respawn.waitTime)
        item.invincibleTime = tonumber(respawn.invincibleTime)
        item.position = VectorUtil.newVector3i(respawn.x, respawn.y, respawn.z)
        table.insert(self.respawns, item)
    end
end

function RespawnConfig:randomRespawn(teamId)
    local items = {}
    for id, respawn in pairs(self.respawns) do
        if respawn.teamId == teamId then
            table.insert(items, respawn)
        end
    end
    return items[math.random(1, #items)]
end

return RespawnConfig