VehicleConfig = {}
VehicleConfig.vehicles = {}

function VehicleConfig:init(config)
    for i, v in pairs(config) do
        local vehicle = {}
        vehicle.type = tonumber(v.type)
        vehicle.pos = VectorUtil.newVector3(v.x, v.y, v.z)
        vehicle.yaw = tonumber(v.yaw)
        table.insert(self.vehicles, vehicle)
    end
end

function VehicleConfig:prepareVehicle()
    for i, vehicle in pairs(self.vehicles) do
        vehicle.entityId = EngineWorld:addVehicleNpc(vehicle.pos, vehicle.type, vehicle.yaw)
    end
end

return VehicleConfig