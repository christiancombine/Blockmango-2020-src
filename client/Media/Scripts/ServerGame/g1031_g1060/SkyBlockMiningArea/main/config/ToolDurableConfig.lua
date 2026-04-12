
ToolDurableConfig = {}
ToolDurableConfig.BlockDurable = {}

function ToolDurableConfig:initConfig(config)
    for _, config in pairs(config) do
        local curTool = {}
        curTool.ToolId = tonumber(config.ToolId)
        curTool.Durable = tonumber(config.Durable)
        table.insert(self.BlockDurable, curTool)
    end
end


function ToolDurableConfig:prepareToolDurable()
    for k, tool in pairs(self.BlockDurable) do
        HostApi.setToolDurability(tool.ToolId, tool.Durable)
    end
end

return ToolDurableConfig