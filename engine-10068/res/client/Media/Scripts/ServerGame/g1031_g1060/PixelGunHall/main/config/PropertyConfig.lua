---
--- Created by Jimmy.
--- DateTime: 2018/11/9  16:15
---
require "property.MaxHealthProperty"
require "property.MaxDefenseProperty"

PropertyConfig = {}
PropertyConfig.PropertyType = {
    MaxHealth = "8", --血量上限
    MaxDefense = "9" --防御上限
}
PropertyConfig.PropertyMapping = {
    [PropertyConfig.PropertyType.MaxHealth] = MaxHealthProperty,
    [PropertyConfig.PropertyType.MaxDefense] = MaxDefenseProperty,
}
PropertyConfig.properties = {}

function PropertyConfig:initConfig(properties)
    for _, property in pairs(properties) do
        local config = {
            id = property.id,
            duration = tonumber(property.duration),
            cdTime = tonumber(property.cdTime),
            value = tonumber(property.value),
            type = property.type
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