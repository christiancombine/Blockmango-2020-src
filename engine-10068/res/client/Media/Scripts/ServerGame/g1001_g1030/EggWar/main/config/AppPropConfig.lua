

AppPropConfig = {}
AppPropConfig.appPropConfigs = {}
function AppPropConfig:init(props)
    for _, prop in pairs(props) do
        local item = {}
        item.id = prop.id
        item.type = tonumber(prop.type)
        item.isRepeat = tonumber(prop.isRepeat or "1") == 1
        item.itemId = tonumber(prop.itemId)
        item.itemMeta = tonumber(prop.itemMeta)
        item.itemNum = tonumber(prop.itemNum)
        item.extraHp = tonumber(prop.extraHp)
        self.appPropConfigs[item.id] = item
    end
end

function AppPropConfig:getRewardHp(id)
    if self.appPropConfigs[id] then
        return self.appPropConfigs[id].extraHp or 0
    end
    return 0
end

return AppPropConfig