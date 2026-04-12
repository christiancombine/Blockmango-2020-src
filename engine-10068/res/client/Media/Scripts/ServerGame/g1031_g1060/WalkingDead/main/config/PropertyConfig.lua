PropertyConfig = {}
PropertyConfig.PropertyType = {
    AddAttack = "1",             --攻击
    AddDefense = "2",            --装备时增加护甲值（效果值在ArmorConfig里配置）
    AddHealth =  "3",            --使用时恢复血量
    AddMaxHealth = "4",          --装备/使用时增加生命上限
    RemoveBuff = "5",            --使用时移除buff
    OnEquipAddBuff = "6",        --装备时获得buff
    OnUsedAddBuff = "7",         --使用时获得buff
    AddFoodLevel = "8",          --使用时恢复饱食度
    AddWaterLevel = "9",         --使用时恢复水分
}
PropertyConfig.PropertyMapping = {
    [PropertyConfig.PropertyType.AddAttack] = AddAttackProperty,
    [PropertyConfig.PropertyType.AddDefense] = AddDefenseProperty,
    [PropertyConfig.PropertyType.AddHealth] = AddHealthProperty,
    [PropertyConfig.PropertyType.AddMaxHealth] = AddMaxHealthProperty,
    [PropertyConfig.PropertyType.RemoveBuff] = RemoveBuffProperty,
    [PropertyConfig.PropertyType.OnEquipAddBuff] = OnEquipAddBuffProperty,
    [PropertyConfig.PropertyType.OnUsedAddBuff] = OnUsedAddBuffProperty,
    [PropertyConfig.PropertyType.AddFoodLevel] = AddFoodLevelProperty,
    [PropertyConfig.PropertyType.AddWaterLevel] = AddWaterLevelProperty,
}
PropertyConfig.properties = {}

function PropertyConfig:initConfig(properties)

    for _, property in pairs(properties) do
        local config = {
            id = property.id or "",
            type = property.type or "",
            targets = tonumber(property.targets) or 0,
            PropertyEffect = tonumber(property.PropertyEffect) or 0,
            duration = tonumber(property.duration) or 0,
            cdTime = tonumber(property.cdTime) or 0,
            value = StringUtil.split(property.value, "#") ,
            probability = tonumber(property.probability) or 0
        }
        local class = PropertyConfig.PropertyMapping[property.type]
        if class then
            self.properties[property.id] = class.new(config,property.type)
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