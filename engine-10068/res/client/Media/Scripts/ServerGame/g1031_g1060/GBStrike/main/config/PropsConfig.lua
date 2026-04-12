---
--- Created by Jimmy.
--- DateTime: 2018/9/26 0026 9:46
---
PropsConfig = {}
PropsConfig.props = {}

function PropsConfig:init(props)
    self:initProps(props)
end

function PropsConfig:initProps(props)
    for _, prop in pairs(props) do
        local item = {}
        item.id = prop.id
        item.name = prop.name
        item.image = prop.image
        item.desc = prop.desc
        item.price = tonumber(prop.price)
        item.teamId = tonumber(prop.teamId)
        item.items = {}
        local itemIds = StringUtil.split(prop.itemIds, "#")
        for _, id in pairs(itemIds) do
            table.insert(item.items, ItemsConfig:getItem(id))
        end
        self.props[item.id] = item
    end
end

function PropsConfig:getProp(id)
    return self.props[id]
end

return PropsConfig