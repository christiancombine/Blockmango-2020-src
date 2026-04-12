GameRank = class()

function GameRank:ctor(data)
    self:init(data)
end

function GameRank:init(data)
    self.id = tonumber(data.id)
    self.pos = VectorUtil.newVector3(data.x, data.y, data.z)
    local minX = math.min(tonumber(data.x1), tonumber(data.x2))
    local maxX = math.max(tonumber(data.x1), tonumber(data.x2))
    local minY = math.min(tonumber(data.y1), tonumber(data.y2))
    local maxY = math.max(tonumber(data.y1), tonumber(data.y2))
    local minZ = math.min(tonumber(data.z1), tonumber(data.z2))
    local maxZ = math.max(tonumber(data.z1), tonumber(data.z2))
    self.startPos = VectorUtil.newVector3i(minX, minY, minZ)
    self.endPos = VectorUtil.newVector3i(maxX, maxY, maxZ)
    self.yaw = tonumber(data.yaw)
    self.name = data.name
    self.bulletinId = data.bulletinId
    self.entityId = EngineWorld:getWorld():addEntityBulletin(self.pos, self.yaw, self.bulletinId, self.name)
end