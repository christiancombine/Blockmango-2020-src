require "config.Area"

BlockConfig = {}
BlockConfig.block = {}
BlockConfig.blockHp = {}
BlockConfig.area = {}
BlockConfig.mine = {}
BlockConfig.items = {}

function BlockConfig:initBlock(block)
    for _, v in pairs(block) do
        local data = {}
        data.id = tonumber(v.id)
        data.meta = tonumber(v.meta or "0")
        data.hardness = tonumber(v.hardness or "0.5")
        data.hp = tonumber(v.hp or "5")
        self.block[data.id] = data
    end
end

function BlockConfig:initArea(area)
    for _, v in pairs(area) do
        local index = tonumber(v.id)
        local position = {}
        position[1] = {}
        position[1][1] = tonumber(v.startX)
        position[1][2] = tonumber(v.startY)
        position[1][3] = tonumber(v.startZ)

        position[2] = {}
        position[2][1] = tonumber(v.endX)
        position[2][2] = tonumber(v.endY)
        position[2][3] = tonumber(v.endZ)

        self.area[index] = Area.new(position)
    end
end

function BlockConfig:initMine(mine)
    for i, v in pairs(mine) do
        local data = {}
        data.id = tonumber(v.id)

        data.money = {}
        data.money[1] = tonumber(v.minMoney)
        data.money[2] = tonumber(v.maxMoney)

        data.x = {}
        data.x[1] = tonumber(math.min(v.minX, v.maxX))
        data.x[2] = tonumber(math.max(v.minX, v.maxX))

        data.y = {}
        data.y[1] = tonumber(math.min(v.minY, v.maxY))
        data.y[2] = tonumber(math.max(v.minY, v.maxY))

        data.z = {}
        data.z[1] = tonumber(math.min(v.minZ, v.maxZ))
        data.z[2] = tonumber(math.max(v.minZ, v.maxZ))

        data.count = {}
        data.count[1] = tonumber(v.minCount)
        data.count[2] = tonumber(v.maxCount)
        self.mine[i] = data
    end
end

function BlockConfig:initItem(config)
    for i, v in pairs(config) do
        local data = {}
        data.blockId = tonumber(v.blockId)
        data.itemId = tonumber(v.itemId)
        data.exchange = tonumber(v.exchange)
        data.teamResource = tonumber(v.teamResource)
        self.items[i] = data
    end
end

function BlockConfig:prepareBlock()
    for _, v in pairs(self.block) do
        HostApi.setBlockAttr(v.id, v.hardness)
    end
end

function BlockConfig:prepareMine()
    for _, v in pairs(self.mine) do
        math.randomseed(os.clock() * 1000000)
        local count = math.random(v.count[1], v.count[2])
        self:generateBlock(count, v.x, v.y, v.z, v.id)
    end
end

function BlockConfig:generateBlock(count, x, y, z, id)
    for i = 1, count do
        local pos = BlockConfig:randomPosition(x, y, z)
        EngineWorld:setBlock(pos, id, 0, 0)
    end
end

function BlockConfig:randomPosition(x, y, z)
    math.randomseed(os.clock() * 314159)
    local pos = VectorUtil.newVector3i(math.random(x[1], x[2]), math.random(y[1], y[2]), math.random(z[1], z[2]))
    local blockId = EngineWorld:getBlockId(pos)
    if blockId ~= GameConfig.fillBlockId then
        return BlockConfig:randomPosition(x, y, z)
    else
        return pos
    end
end

function BlockConfig:getMineMoneyById(id)
    for _, v in pairs(self.mine) do
        if v.id == id then
            local money = math.random(v.money[1], v.money[2])
            return money
        end
    end

    return 0
end

function BlockConfig:getItemByBlockId(id)
    for _, v in pairs(self.items) do
        if v.blockId == id then
            return v.itemId
        end
    end

    return nil
end

function BlockConfig:getExchangeByItemId(id)
    for _, v in pairs(self.items) do
        if v.itemId == id then
            return v.exchange
        end
    end
    return 0
end

function BlockConfig:getTeamResourceByItemId(id)
    for _, v in pairs(self.items) do
        if v.itemId == id then
            return v.teamResource
        end
    end
    return 0
end

return BlockConfig