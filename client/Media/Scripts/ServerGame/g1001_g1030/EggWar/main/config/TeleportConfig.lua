---
--- Created by Jimmy.
--- DateTime: 2018/2/27 0027 11:10
---

TeleportConfig = {}
TeleportConfig.teleport = {}

function TeleportConfig:init(config)
    for i, v in pairs(config) do
        local teleport = {}
        for j, item in pairs(v) do
            local key = item.id
            teleport[key] = {}
            teleport[key].pos = {}
            for k, pos in pairs(item.pos) do
                teleport[key].pos[k] = {}
                if item.pos ~= nil then
                    teleport[key].pos[k] = VectorUtil.newVector3(pos.x, pos.y, pos.z)
                end
            end
        end
        self.teleport[i] = teleport
    end
end

function TeleportConfig:getRandomPosition(teamId, tpItemId)
    if self.teleport[teamId][tpItemId] ~= nil then
        if #self.teleport[teamId][tpItemId].pos > 0 then
            math.randomseed(os.clock()*1000000)
            local ramdomNum = math.random(1, #self.teleport[teamId][tpItemId].pos)
            for i, v in pairs(self.teleport[teamId][tpItemId].pos) do
                if i == ramdomNum then
                    return v
                end
            end
        end
    end
    return nil
end

return TeleportConfig

