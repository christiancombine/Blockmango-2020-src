BackpackConfig = {}
BackpackConfig.backpack = {}

function BackpackConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.capacity = tonumber(v.capacity)
        self.backpack[data.id] = data
    end
end

function BackpackConfig:getCapacityById(id)
    if self.backpack[id] ~= nil then
        return self.backpack[id].capacity
    end
    print("backpack.csv[".. id .."] not found.")
    return 0
end

function BackpackConfig:getBackpackById(id)
    if self.backpack[id] ~= nil then
        return self.backpack[id]
    end
    print("backpack.csv[".. id .."] not found.")
    return nil
end


return BackpackConfig