---
--- Created by Jimmy.
--- DateTime: 2018/7/4 0004 14:43
---
require "config.UpgradeEquipBoxConfig"

local utils = require "utils"
EquipBoxConfig = {}
EquipBoxConfig.equipBox = {}
EquipBoxConfig.equipboxGroups = {}

function EquipBoxConfig:init(equipboxs)
    self:initEquipboxs(equipboxs)
end

function EquipBoxConfig:initEquipboxs(equipboxs)
    for id, c_item in pairs(equipboxs) do
        local equipboxs = {}
        local groupId = c_item.groupId
        if self.equipboxGroups[groupId] ~= nil then
            equipboxs = self.equipboxGroups[groupId]
        end
        local item = {}
        item.id = id
        item.isBuilding = false
        item.type = tonumber(c_item.type)
        item.actor = c_item.actor
        item.builderActor = c_item.builderActor
        item.name = c_item.name
        item.position = VectorUtil.newVector3(c_item.x, c_item.y, c_item.z)
        item.yaw = tonumber(c_item.yaw)
        item.clothes = {}
        local c = StringUtil.split(c_item.clothesEffect, "#")
        for _, b in pairs(c) do
            local i = StringUtil.split(b, "=")
            local b_item = {}
            b_item.name = i[1]
            b_item.sex = i[2]
            table.insert(item.clothes, b_item)
        end
        item.clothesHp = tonumber(c_item.clothesHp)
        local effect = StringUtil.split(c_item.effect, "#")
        item.effect = {}
        item.effect.id = tonumber(effect[1])
        item.effect.time = tonumber(effect[2])
        item.effect.value = tonumber(effect[3])
        if #effect > 4 then
            item.effect.pos = VectorUtil.newVector3(effect[4], effect[5], effect[6])
        end
        item.equipEffect = c_item.equipEffect
        item.clothesActor = c_item.clothesActor
        item.armsId = tonumber(c_item.armsId)
        item.value = tonumber(c_item.value)
        item.price = tonumber(c_item.price)
        item.floor = tonumber(c_item.floor)
        item.damage = tonumber(c_item.damage)
        item.isShare = tonumber(c_item.isShare) == 1
        item.num = tonumber(c_item.num)
        local bullet = StringUtil.split(c_item.bullet, "#")
        item.bullet = {}
        item.bullet.id = tonumber(bullet[1])
        item.bullet.num = tonumber(bullet[2])
        item.buildingIds = StringUtil.split(c_item.buildingIds, "#")
        item.enchantments = StringUtil.split(c_item.enchantments, "#")
        --item.enchantment = {}
        --item.enchantment.id = tonumber(enchantments[1])
        --item.enchantment.lv = tonumber(enchantments[2])
        item.upgradeConfig = UpgradeEquipBoxConfig:getConfigById(tostring(item.id))
        equipboxs[#equipboxs + 1] = item
        self.equipBox[#self.equipBox + 1] = item
        self.equipboxGroups[groupId] = equipboxs

    end
end

function EquipBoxConfig:getEquipBoxsByGroupId(groupId)
    local equipboxs = self.equipboxGroups[groupId]
    if equipboxs then
        return equipboxs
    end
    error("EquipBox config error, can't find groupId:" .. groupId)
end

function EquipBoxConfig:getEquipBoxInfoById(id)
    for i, v in pairs(self.equipBox) do
        if tostring(v.id) == tostring(id) then
            return v
        end
    end

    return nil
end

return EquipBoxConfig