ChestConfig = {}
ChestConfig.chest = {}

function ChestConfig:init(config)
    self:initChest(config.chest)
end

function ChestConfig:initChest(chest)
    for i, v in pairs(chest) do
        local item = {}
        item.id = tonumber(v.id)
        item.name = tonumber(v.name)
        item.money = {}
        item.money[1] = tonumber(v.money[1])
        item.money[2] = tonumber(v.money[2])
        item.x = {}
        item.x[1] = tonumber(math.min(v.x[1], v.x[2]))
        item.x[2] = tonumber(math.max(v.x[1], v.x[2]))
        item.y = {}
        item.y[1] = tonumber(math.min(v.y[1], v.y[2]))
        item.y[2] = tonumber(math.max(v.y[2], v.y[2]))
        item.z = {}
        item.z[1] = tonumber(math.min(v.z[1], v.z[2]))
        item.z[2] = tonumber(math.max(v.z[1], v.z[2]))
        item.count = {}
        item.count[1] = tonumber(v.count[1])
        item.count[2] = tonumber(v.count[2])
        self.chest[i] = item
    end
end

function ChestConfig:prepareChest()
    for i, v in pairs(self.chest) do
        local count = math.random(v.count[1], v.count[2])
        self:generateChest(count, v.x, v.y, v.z, v.id)
    end
end

function ChestConfig:generateChest(count, x, y, z, id)
    for i = 1, count do
        local pos = VectorUtil.newVector3i(math.random(x[1], x[2]), math.random(y[1], y[2]), math.random(z[1], z[2]))
        EngineWorld:setBlock(pos, id, 0, 0)
    end
end

function ChestConfig:getChestMoneyById(id)
    for i, v in pairs(self.chest) do
        if v.id == id then
            return math.random(v.money[1], v.money[2]), v.name
        end
    end

    return 0
end

return ChestConfig