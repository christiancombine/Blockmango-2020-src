---
--- Created by Jimmy.
--- DateTime: 2018/4/27 0027 16:29
---

EquipmentConfig = {}
EquipmentConfig.equipments = {}

EquipmentConfig.PART_NAME_HELMET = "helmet"
EquipmentConfig.PART_NAME_PLATE = "plate"
EquipmentConfig.PART_NAME_LEGS = "legs"
EquipmentConfig.PART_NAME_BOOTS = "boots"

function EquipmentConfig:init(equipments)
    self:initEquipments(equipments)
end

function EquipmentConfig:initEquipments(equipments)
    for id, equipment in pairs(equipments) do
        local item = {}
        item.id = id
        item.part = equipment.part
        item.itemId = tonumber(equipment.itemId)
        item.level = tonumber(equipment.level)
        item.name = equipment.name
        item.image = equipment.image
        item.desc = equipment.description
        item.hp = tonumber(equipment.hp)
        item.attack = tonumber(equipment.attack)
        item.defense = tonumber(equipment.defense)
        item.speed = tonumber(equipment.speed)
        item.addHp = tonumber(equipment.addHp)
        item.price = tonumber(equipment.price)
        self.equipments[item.part .. "#" .. item.level] = item
    end
end

function EquipmentConfig:getEquipmentById(id)
    for i, equipment in pairs(self.equipments) do
        if equipment.id == id then
            return equipment
        end
    end
    return nil
end

function EquipmentConfig:getEquipment(part, level)
    return self.equipments[part .. "#" .. level]
end

return EquipmentConfig