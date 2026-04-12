---
--- Created by Jimmy.
--- DateTime: 2018/6/7 0007 11:27
---
InventoryConfig = {}
InventoryConfig.inventorys = {}

function InventoryConfig:init(inventorys)
    self:initInventorys(inventorys)
end

function InventoryConfig:initInventorys(inventorys)
    for index, c_item in pairs(inventorys) do
        local inventory = {}
        local inventoryId = c_item.inventoryId
        if self.inventorys[inventoryId] ~= nil then
            inventory = self.inventorys[inventoryId]
        end
        local item = {}
        item.index = tonumber(index)
        item.id = tonumber(c_item.id)
        item.meta = tonumber(c_item.meta)
        item.num = tonumber(c_item.num)
        item.isEquip = tonumber(c_item.isEquip) == 1
        item.isReget = tonumber(c_item.isReget) == 1
        inventory[#inventory + 1] = item
        self.inventorys[inventoryId] = inventory
    end
    for inventoryId, inventory in pairs(self.inventorys) do
        table.sort(inventory, function(a, b)
            return a.index < b.index
        end)
    end
end

function InventoryConfig:getInventory(inventoryId)
    local inventory = self.inventorys[inventoryId]
    if inventory then
        return inventory
    end
    for inventoryId, inventory in pairs(self.inventorys) do
        return inventory
    end
end

return InventoryConfig