---
--- Created by Jimmy.
--- DateTime: 2018/3/6 0006 11:30
---
ToolConfig = {}
ToolConfig.tool = {}

function ToolConfig:init(config)
    self:initTool(config.tool)
end

function ToolConfig:initTool(config)
    for i, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.id)
        item.distance = tonumber(v.distance)
        item.efficiency = tonumber(v.efficiency)
        item.damage = tonumber(v.damage)
        self.tool[item.id] = item
    end
end

function ToolConfig:prepareTool()
    for i, v in pairs(self.tool) do
        HostApi.setToolAttr(v.id, v.distance, v.efficiency)
    end
end

function ToolConfig:getDamageByToolId(id)
    if self.tool[id] ~= nil then
        return self.tool[id].damage
    end

    return 1
end

return ToolConfig