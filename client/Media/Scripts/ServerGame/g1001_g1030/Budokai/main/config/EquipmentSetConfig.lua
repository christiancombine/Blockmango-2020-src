---
--- Created by Jimmy.
--- DateTime: 2018/8/3 0003 17:11
---
EquipmentSetConfig = {}
EquipmentSetConfig.equipmentSets = {}

function EquipmentSetConfig:init(equipmentSets)
    self:initEquipmentSets(equipmentSets)
end

function EquipmentSetConfig:initEquipmentSets(equipmentSets)
    for _, equipmentSet in pairs(equipmentSets) do
        local item = {}
        item.itemId = tonumber(equipmentSet.itemId)
        local items = {}
        local c_items = StringUtil.split(equipmentSet.items, "#")
        for _, c_item in pairs(c_items) do
            local t_item = StringUtil.split(c_item, "=")
            items[#items + 1] = { itemId = tonumber(t_item[1]), num = tonumber(t_item[2]) }
        end
        item.items = items
        self.equipmentSets[#self.equipmentSets + 1] = item
    end
end

function EquipmentSetConfig:getEquipmentSet(itemId)
    for _, equipmentSet in pairs(self.equipmentSets) do
        if equipmentSet.itemId == itemId then
            return equipmentSet
        end
    end
    return nil
end

return EquipmentSetConfig