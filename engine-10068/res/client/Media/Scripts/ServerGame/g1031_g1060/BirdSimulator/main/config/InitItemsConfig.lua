InitItemsConfig = {}
InitItemsConfig.items = {}

InitItemsConfig.ITEMS = 1
InitItemsConfig.BACKPACKS = 2
InitItemsConfig.TOOLS = 3
InitItemsConfig.BIRDS = 4

function InitItemsConfig:initItems(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.type = tonumber(v.type)
        data.itemId = tonumber(v.itemId)
        data.meta = tonumber(v.meta)
        data.num = tonumber(v.num)
        self.items[data.id] = data
    end
end

return InitItemsConfig