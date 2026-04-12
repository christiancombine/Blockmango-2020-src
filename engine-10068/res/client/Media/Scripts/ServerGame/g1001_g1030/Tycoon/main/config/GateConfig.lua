---
--- Created by Jimmy.
--- DateTime: 2018/7/4 0004 15:00
---

GateConfig = {}
GateConfig.gates = {}

function GateConfig:init(gates)
    self:initGates(gates)
end

function GateConfig:initGates(gates)
    for id, c_item in pairs(gates) do
        local gate = {}
        gate.id = id
        gate.isBuilding = false
        gate.fileName = c_item.fileName
        gate.gateNpcActor = c_item.gateNpcActor
        gate.gateNpcName = c_item.gateNpcName
        gate.gateNpcPos = VectorUtil.newVector3(c_item.x, c_item.y, c_item.z)
        gate.gateNpcYaw = tonumber(c_item.yaw)
        gate.value = tonumber(c_item.value)
        gate.price = tonumber(c_item.price)
        gate.startPos = VectorUtil.newVector3(c_item.startX, c_item.startY, c_item.startZ)
        gate.guardActor = c_item.guardActor
        gate.guardName = c_item.guardName
        gate.guard1 = {}
        gate.guard1.pos = VectorUtil.newVector3(c_item.guard1x, c_item.guard1y, c_item.guard1z)
        gate.guard1.yaw = tonumber(c_item.guard1yaw)
        gate.guard2 = {}
        gate.guard2.pos = VectorUtil.newVector3(c_item.guard2x, c_item.guard2y, c_item.guard2z)
        gate.guard2.yaw = tonumber(c_item.guard2yaw)
        gate.buildingIds = StringUtil.split(c_item.buildingIds, "#")
        self.gates[id] = gate
    end
end

function GateConfig:getGate(gateId)
    local gate = self.gates[gateId]
    if gate then
        return gate
    end
    error("Gate config error, can't find gateId:" .. gateId)
end

return GateConfig