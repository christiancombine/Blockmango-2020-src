PlayerInitItemsConfig = {}
PlayerInitItemsConfig.items = {}
PlayerInitItemsConfig.inventory = {}
function PlayerInitItemsConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.itemId = tonumber(v.itemId)
        data.num = tonumber(v.num)
        self.items[data.id] = data
    end
end

function PlayerInitItemsConfig:initInventory(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.num = tonumber(v.num)
        table.insert(self.inventory, data)
    end
end

