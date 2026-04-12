---
--- Created by Jimmy.
--- DateTime: 2018/6/11 0011 10:23
---
RespawnConfig = {}
RespawnConfig.respawns = {}

function RespawnConfig:init(config)
    self:initRespawn(config)
end

function RespawnConfig:initRespawn(respawns)
    for i, respawn in pairs(respawns) do
        local item = {}
        item.time = tonumber(respawn.time)
        item.wait = tonumber(respawn.wait)
        self.respawns[i] = item
    end
end

function RespawnConfig:getRespawnCd(second)
    local result
    for i, respawn in pairs(self.respawns) do
        if result == nil then
            result = respawn
        end
        if second >= respawn.time then
            result = respawn
        end
    end
    if result ~= nil then
        return result.wait
    else
        return 5
    end
end

return RespawnConfig