require "config.ManorNpcConfig"

VehicleConfig = {}
VehicleConfig.vehicles = {}

function VehicleConfig:init(config)
    self:initVehicle(config.vehicle)
end

function VehicleConfig:initVehicle(config)
    for i, v in pairs(config) do
        local vehicle = {}
        vehicle.id = tonumber(v.id)
        vehicle.pos = VectorUtil.newVector3(v.pos[1], v.pos[2], v.pos[3])
        vehicle.yaw = tonumber(v.pos[4])
        self.vehicles[i] = vehicle
    end
end

function VehicleConfig:prepareVehicle()
    for i, vehicle in pairs(self.vehicles) do
        vehicle.entityId = EngineWorld:addVehicleNpc(vehicle.pos, vehicle.id, vehicle.yaw)
    end
end

function VehicleConfig:checkAndResetPosition()
    for i, vehicle in pairs(self.vehicles) do
        if EngineWorld:getWorld():hasPlayerByVehicle(vehicle.entityId) == false then
            EngineWorld:getWorld():resetVehiclePositionAndYaw(vehicle.entityId, vehicle.pos, vehicle.yaw)
        end
    end
end

function VehicleConfig:resetPositionBeforeBuildHouse(manorIndex, yardLevel)
    local manor = ManorNpcConfig:getManorById(manorIndex)
    if manor == nil then
        --HostApi.log("==== LuaLog: VehicleConfig:resetPositionBeforeBuildHouse: manor is nil reset vehicle position failure ")
        return
    end

    for i, vehicle in pairs(self.vehicles) do
        if YardConfig:isInYard(manorIndex, yardLevel, EngineWorld:getWorld():getPositionByEntityId(vehicle.entityId)) then
            EngineWorld:getWorld():resetVehiclePositionAndYaw(vehicle.entityId, vehicle.pos, vehicle.yaw)
        end
    end
end

return VehicleConfig