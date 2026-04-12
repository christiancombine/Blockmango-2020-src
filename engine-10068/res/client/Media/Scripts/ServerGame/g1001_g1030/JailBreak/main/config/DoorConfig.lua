---
--- Created by Jimmy.
--- DateTime: 2018/2/3 0003 10:44
---

DoorConfig = {}
DoorConfig.doors = {}

function DoorConfig:init(config)
    self:initDoor(config.door)
end

function DoorConfig:initDoor(config)
    for i, v in pairs(config) do
        local door = {}
        door.id = tonumber(v.id)
        door.pos = VectorUtil.newVector3i(v.pos[1], v.pos[2], v.pos[3])
        door.meta = tonumber(v.meta)
        door.name = tonumber(v.name)
        door.threat = tonumber(v.threat)
        self.doors[VectorUtil.hashVector3(door.pos)] = door
    end
end

function DoorConfig:prepareDoor()
    for i, door in pairs(self.doors) do
        EngineWorld:setBlock(door.pos, door.id, door.meta)
    end
end

function DoorConfig:getDoor(pos)
    return self.doors[VectorUtil.hashVector3(pos)]
end

return DoorConfig