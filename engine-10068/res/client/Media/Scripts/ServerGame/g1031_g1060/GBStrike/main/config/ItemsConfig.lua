---
--- Created by Jimmy.
--- DateTime: 2018/9/26 0026 10:34
---
ItemsConfig = {}
ItemsConfig.items = {}

function ItemsConfig:init(items)
    self:initItems(items)
end

function ItemsConfig:initItems(items)
    for _, c_item in pairs(items) do
        local item = {}
        item.id = c_item.id
        item.itemId = tonumber(c_item.itemId)
        item.num = tonumber(c_item.num)
        item.isGun = tonumber(c_item.isGun) == 1
        item.bullets = tonumber(c_item.bullets)
        item.isArmor = tonumber(c_item.isArmor) == 1
        item.defense = tonumber(c_item.defense)
        item.isLast = tonumber(c_item.isLast) == 1
        self.items[item.id] = item
    end
end

function ItemsConfig:getItem(id)
    return self.items[id]
end

function ItemsConfig:getItemByItemId(itemId)
    for _, item in pairs(self.items) do
        if item.itemId == itemId then
            return item
        end
    end
    return nil
end

return ItemsConfig