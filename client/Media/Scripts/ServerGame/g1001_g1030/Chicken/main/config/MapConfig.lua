require "config.Area"
MapConfig = {}

MapConfig.poisonArea = {}
MapConfig.safeArea = {}
MapConfig.safeAreaSize = {}
MapConfig.safeAreaSize.x = 0
MapConfig.safeAreaSize.y = 0
MapConfig.safeAreaSize.z = 0
MapConfig.safeAreaSize.size = 0
MapConfig.mapArea = {}

function MapConfig:init(config)
    self:initPoisonArea(config.poisonArea)
    self:initSafeArea(config.safeArea)
    self:initMap(config)
end

function MapConfig:initPoisonArea(poisonArea)
    for i, v in pairs(poisonArea) do
        local poisonArea =  {}
        poisonArea.id = tonumber(v.id)
        poisonArea.damage = tonumber(v.damage)
        poisonArea.damageOnTime = tonumber(v.damageOnTime)
        poisonArea.moveSpeed = tonumber(v.moveSpeed)
        self.poisonArea[i] = poisonArea
    end
end

function MapConfig:initSafeArea(safeArea)
    for i, v in pairs(safeArea) do
        local safeArea =  {}
        safeArea.id = tonumber(v.id)
        safeArea.size = tonumber(v.size)
        self.safeArea[i] = safeArea
    end
end

function MapConfig:initMap(config)
    self.safeAreaSize.size = tonumber(config.mapSize)
    self.mapArea = Area.new(config.mapArea)
    self.safeAreaSize.x = self.mapArea.minX
    self.safeAreaSize.y = self.mapArea.minY
    self.safeAreaSize.z = self.mapArea.minZ
end

function MapConfig:getSafeArea(id)
    local areaSize = self.safeArea[id].size
    math.randomseed(tostring(os.time()):reverse():sub(1, 6))
    AirSupportConfig.airSupportPosition.x = math.random(self.safeAreaSize.x, self.safeAreaSize.x + self.safeAreaSize.size)
    math.randomseed(tostring(os.time()):reverse():sub(1, 7))
    AirSupportConfig.airSupportPosition.z = math.random(self.safeAreaSize.z, self.safeAreaSize.z + self.safeAreaSize.size)
    self.safeAreaSize.x = math.random(self.safeAreaSize.x, self.safeAreaSize.x + self.safeAreaSize.size - areaSize)
    self.safeAreaSize.z = math.random(self.safeAreaSize.z, self.safeAreaSize.z + self.safeAreaSize.size - areaSize)
    self.safeAreaSize.size = areaSize
end

function MapConfig:getPoisonArea(id)
    return self.poisonArea[id]
end

return PoisonConfig