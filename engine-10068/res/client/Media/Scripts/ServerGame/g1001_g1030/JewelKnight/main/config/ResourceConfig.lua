ResourceConfig = {}
ResourceConfig.resource = {}

function ResourceConfig:init(config)
    for i, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.id)
        item.itemId = tonumber(v.itemId)
        item.x = tonumber(v.x)
        item.y = tonumber(v.y)
        item.z = tonumber(v.z)
        item.time = tonumber(v.time)
        item.num = tonumber(v.num)
        item.life = tonumber(v.life)
        self.resource[item.id] = item
    end
end


return ResourceConfig