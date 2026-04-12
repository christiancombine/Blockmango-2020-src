---
--- Created by Jimmy.
--- DateTime: 2018/3/21 0021 10:43
---

StrongboxConfig = {}
StrongboxConfig.strongbox = {}

function StrongboxConfig:init(config)
    self:initStrongbox(config.strongbox)
end

function StrongboxConfig:initStrongbox(config)
    for i, v in pairs(config) do
        local box = {}
        box.pos = VectorUtil.newVector3i(v.initPos[1], v.initPos[2], v.initPos[3])
        box.yaw = tonumber(v.initPos[4])
        box.itemId = tonumber(v.itemId) or "130"
        self.strongbox[VectorUtil.hashVector3(box.pos)] = box
    end
end

function StrongboxConfig:prepareStrongbox()
    for i, v in pairs(self.strongbox) do
        EngineWorld:setBlock(v.pos, v.itemId, v.yaw)
    end
end

return StrongboxConfig