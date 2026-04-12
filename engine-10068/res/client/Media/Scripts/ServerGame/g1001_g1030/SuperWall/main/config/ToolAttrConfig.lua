---
--- Created by Jimmy.
--- DateTime: 2018/8/24 0024 11:33
---
ToolAttrConfig = {}
ToolAttrConfig.tools = {}

function ToolAttrConfig:init(toolAttrs)
    self:initToolAttrs(toolAttrs)
end

function ToolAttrConfig:initToolAttrs(toolAttrs)
    for _, toolAttr in pairs(toolAttrs) do
        local item = {}
        item.id = toolAttr.id
        item.itemId = tonumber(toolAttr.itemId)
        item.distance = tonumber(toolAttr.distance)
        item.efficiency = tonumber(toolAttr.efficiency)
        item.durability = tonumber(toolAttr.durability)
        item.moneyPercent = tonumber(toolAttr.moneyPercent)
        item.digScore = tonumber(toolAttr.digScore)
        self.tools[toolAttr.itemId] = item
    end
    self:prepareTool()
end

function ToolAttrConfig:prepareTool()
    for _, tool in pairs(self.tools) do
        HostApi.setToolAttr(tool.itemId, tool.distance, tool.efficiency)
    end
end

function ToolAttrConfig:getDurability(itemId)
    local tool = self.tools[tostring(itemId)]
    if tool then
        return tool.durability
    end
    return -1
end

function ToolAttrConfig:getMoneyPercent(itemId)
    local tool = self.tools[tostring(itemId)]
    if tool then
        return tool.moneyPercent
    end
    return 1
end

function ToolAttrConfig:getDigScore(itemId)
    local tool = self.tools[tostring(itemId)]
    if tool then
        return tool.digScore
    end
    return 1
end

return ToolAttrConfig
