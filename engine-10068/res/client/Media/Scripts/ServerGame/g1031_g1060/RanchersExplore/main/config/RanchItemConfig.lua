RanchItemConfig = {}
RanchItemConfig.items = {}

function RanchItemConfig:init(config)
    for i, item in pairs(config) do
        local data = {}
        data.id = tonumber(item.Id)
        data.icon = tostring(item.Icon)
        self.items[data.id] = data
    end
end

function RanchItemConfig:getRanchItemImgById(id)
    if self.items[id] ~= nil then
        return self.items[id].icon
    end

    return ""
end

return RanchItemConfig