ToolConfig = {}
ToolConfig.tool = {}

function ToolConfig:init(config)
    self:initTool(config)
end

function ToolConfig:initTool(config)
    for _, tool in pairs(config) do
        local data = {}
        data.id = tonumber(tool.id)
        data.efficiency = tonumber(tool.efficiency)
        data.distance = tonumber(tool.distance)
        data.damage = tonumber(tool.damage)
        data.hurtDamage = tonumber(tool.hurtDamage)
        self.tool[data.id] = data
    end
end

function ToolConfig:prepareTool()
    for _, v in pairs(self.tool) do
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