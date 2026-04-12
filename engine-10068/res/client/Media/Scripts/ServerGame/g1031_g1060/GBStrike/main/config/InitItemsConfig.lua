---
--- Created by Jimmy.
--- DateTime: 2018/9/29 0029 9:56
---
InitItemsConfig = {}
InitItemsConfig.items = {}
InitItemsConfig.GAME_START = "1"
InitItemsConfig.GAME_END = "2"

function InitItemsConfig:init(items)
    self:initItems(items)
end

function InitItemsConfig:initItems(items)
    for _, c_item in pairs(items) do
        table.insert(self.items, {
            id = c_item.id,
            itemId = tonumber(c_item.itemId),
            num = tonumber(c_item.num),
            type = c_item.type
        })
    end
end

function InitItemsConfig:getGameStartItems()
    local items = {}
    for _, item in pairs(self.items) do
        if item.type == InitItemsConfig.GAME_START then
            table.insert(items, item)
        end
    end
    return items
end

function InitItemsConfig:getGameEndItems()
    local items = {}
    for _, item in pairs(self.items) do
        if item.type == InitItemsConfig.GAME_END then
            table.insert(items, item)
        end
    end
    return items
end

return InitItemsConfig