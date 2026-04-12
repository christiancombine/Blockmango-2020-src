require "config.BlockConfig"

GameJewel = class()

GameJewel.id = 0
GameJewel.name = ""
GameJewel.actor = ""
GameJewel.itemId = ""
GameJewel.position = {}

function GameJewel:ctor(config, isRecreate)
    self.id = config.id
    self.name = config.name or ""
    self.actor = config.actor
    self.itemId = config.itemId
    self.entityId = self:addJewelToWorld(isRecreate)
end

function GameJewel:randomPosition(x, y, z)
    math.randomseed(os.clock() * 1000000)
    local pos = VectorUtil.newVector3i(math.random(x[1], x[2]), math.random(y[1], y[2]), math.random(z[1], z[2]))
    return pos
end

function GameJewel:addJewelToWorld(isRecreate)
    math.randomseed(os.clock() * 1000000)
    local index = math.random(1, #BlockConfig.area or 1)
    local area = BlockConfig.area[index]
    local x = {}
    x[1] = area.minX
    x[2] = area.maxX
    local y = {}
    y[1] = area.minY
    y[2] = area.maxY
    local z = {}
    z[1] = area.minZ
    z[2] = area.maxZ
    if isRecreate == false then
        self.position = BlockConfig:randomPosition(x, y, z)
    else
        self.position = self:randomPosition(x, y, z)
    end

    HostApi.log("======== LuaLog : addJewelToWorld : position.x = " .. self.position.x .. ", y = " .. self.position.y .. ", z = " .. self.position.z)

    EngineWorld:setBlockToAir(self.position)

    local position = VectorUtil.newVector3(self.position.x + 0.5, self.position.y, self.position.z + 0.5)
    self.position = position
    local entityId = EngineWorld:addActorNpc(position, 0, self.actor, function(entity)
        entity:setCanObstruct(false)
        if JewelConfig.isResourceFull == true or isRecreate == true then
            entity:setHeadName(self.name)
        end
    end)
    return entityId
end

