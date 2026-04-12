---
--- Created by Jimmy.
--- DateTime: 2018/11/9  16:15
---
require "property.FireProperty"
require "property.SubSpeedProperty"
require "property.PoisonProperty"
require "property.SpikeProperty"
require "property.BloodsuckingProperty"
require "property.DisarmProperty"
require "property.BleedProperty"
require "property.MaxHealthProperty"
require "property.MaxDefenseProperty"
require "property.AddSpeedProperty"
require "property.AddJumpProperty"
require "property.FrozenProperty"

PropertyConfig = {}
PropertyConfig.PropertyType = {
    Fire = "1", --燃烧
    SubSpeed = "2", --减速
    Poison = "3", --中毒
    Spike = "4", --致命一击
    Bloodsucking = "5", --吸血
    Disarm = "6", --缴械
    Bleed = "7", --流血
    MaxHealth = "8", --血量上限
    MaxDefense = "9", --防御上限
    AddSpeed = "10", --加速
    AddJump = "11", --加跳跃
    Frozen = "12", --冰冻
}
PropertyConfig.PropertyMapping = {
    [PropertyConfig.PropertyType.Fire] = FireProperty,
    [PropertyConfig.PropertyType.SubSpeed] = SubSpeedProperty,
    [PropertyConfig.PropertyType.Poison] = PoisonProperty,
    [PropertyConfig.PropertyType.Spike] = SpikeProperty,
    [PropertyConfig.PropertyType.Bloodsucking] = BloodsuckingProperty,
    [PropertyConfig.PropertyType.Disarm] = DisarmProperty,
    [PropertyConfig.PropertyType.Bleed] = BleedProperty,
    [PropertyConfig.PropertyType.MaxHealth] = MaxHealthProperty,
    [PropertyConfig.PropertyType.MaxDefense] = MaxDefenseProperty,
    [PropertyConfig.PropertyType.AddSpeed] = AddSpeedProperty,
    [PropertyConfig.PropertyType.AddJump] = AddJumpProperty,
    [PropertyConfig.PropertyType.Frozen] = FrozenProperty,
}
PropertyConfig.properties = {}

function PropertyConfig:initConfig(properties)
    for _, property in pairs(properties) do
        local config = {
            id = property.id,
            duration = tonumber(property.duration),
            cdTime = tonumber(property.cdTime),
            value = tonumber(property.value),
            type = property.type,
            name = property.name,
            icon = property.icon
        }
        local class = PropertyConfig.PropertyMapping[property.type]
        if class ~= nil then
            self.properties[property.id] = class.new(config)
        end
    end
end

function PropertyConfig:getPropertyById(id)
    return self.properties[id]
end

function PropertyConfig:getPropertyByIds(ids)
    local properties = {}
    if not ids then
        return properties
    end
    ids = StringUtil.split(ids, "#")
    for _, id in pairs(ids) do
        table.insert(properties, self:getPropertyById(id))
    end
    return properties
end

return PropertyConfig