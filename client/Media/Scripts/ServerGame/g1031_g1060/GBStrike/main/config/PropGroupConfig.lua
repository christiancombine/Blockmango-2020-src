---
--- Created by Jimmy.
--- DateTime: 2018/9/26 0026 10:42
---
PropGroupConfig = {}
PropGroupConfig.groups = {}

function PropGroupConfig:init(propGroups)
    self:initPropGroups(propGroups)
end

function PropGroupConfig:initPropGroups(propGroups)
    for _, group in pairs(propGroups) do
        local item = {}
        item.id = group.id
        item.name = group.name
        item.props = {}
        local propIds = StringUtil.split(group.propIds, "#")
        for _, id in pairs(propIds) do
            table.insert(item.props, PropsConfig:getProp(id))
        end
        table.insert(self.groups, item)
    end
    table.sort(self.groups, function(a, b)
        return tonumber(a.id) < tonumber(b.id)
    end)
end

function PropGroupConfig:getPropData(teamId)
    local data = {}
    for _, group in pairs(self.groups) do
        local item = {}
        item.id = group.id
        item.name = group.name
        item.props = {}
        for _, prop in pairs(group.props) do
            HostApi.log(type(prop.teamId) .."        " .. type(teamId) .. "        " ..  prop.teamId  .. "    " .. teamId, 2)
            if  prop.teamId == 0 or prop.teamId == teamId then
                table.insert(item.props, {
                    id =  prop.id,
                    name = prop.name,
                    image = prop.image,
                    desc = prop.desc,
                    price = prop.price,
                })
            end
        end
        table.insert(data, item)
    end
    return data
end

return PropGroupConfig