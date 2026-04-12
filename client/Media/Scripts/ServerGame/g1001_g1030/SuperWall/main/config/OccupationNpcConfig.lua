---
--- Created by Jimmy.
--- DateTime: 2018/8/28 0028 10:40
---
OccupationNpcConfig = {}
OccupationNpcConfig.npcs = {}

function OccupationNpcConfig:init(npcs)
    self:initOccupationNpcs(npcs)
end

function OccupationNpcConfig:initOccupationNpcs(npcs)
    for _, npc in pairs(npcs) do
        local item = {}
        item.id = npc.id
        item.name = npc.name
        item.actor = npc.actor
        item.position = VectorUtil.newVector3(npc.x, npc.y, npc.z)
        item.yaw = tonumber(npc.yaw)
        self.npcs[#self.npcs + 1] = item
    end
end

return OccupationNpcConfig