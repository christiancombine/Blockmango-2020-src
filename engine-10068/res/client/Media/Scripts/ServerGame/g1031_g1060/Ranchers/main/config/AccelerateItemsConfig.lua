AccelerateItemsConfig = {}
AccelerateItemsConfig.items = {}

function AccelerateItemsConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.itemId = tonumber(v.itemId)
        data.num = tonumber(v.num)
        self.items[data.id] = data
    end
end

return AccelerateItemsConfig