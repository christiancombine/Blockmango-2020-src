ToolConfig = {}
ToolConfig.tool = {}

function ToolConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.itemId = tonumber(v.itemId)
        data.meta = tonumber(v.meta)
        data.layer = tonumber(v.layer)
        data.num = tonumber(v.num)
        data.blockId = StringUtil.split(v.blockId, "#")
        data.percent = StringUtil.split(v.percent, "#")
        self.tool[data.itemId] = data
    end
end

function ToolConfig:getToolByItemId(itemId)
    if self.tool[itemId] ~= nil then
        return self.tool[itemId]
    end

    return nil
end

function ToolConfig:getExtraPercent(itemId, blockId)
    if self.tool[itemId] ~= nil then
        for i, v in pairs(self.tool[itemId].blockId) do
            if tonumber(v) == tonumber(blockId) then
                if self.tool[itemId].percent[i] ~= nil then
                    return tonumber(self.tool[itemId].percent[i])
                end
            end
        end
    end

    return 0
end

return ToolConfig