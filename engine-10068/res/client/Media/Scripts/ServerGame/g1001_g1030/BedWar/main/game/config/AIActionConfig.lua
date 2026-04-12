AIActionConfig = {}
AIActionConfig.Config = {}

local Groups = {}

function AIActionConfig:init(config)
    for _, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.Id)
        item.aiPlayerId = tostring(v.aiPlayerId)
        item.defaultTree = tostring(v.defaultTree)
        item.idleTime = tonumber(v.idleTime)
        item.idleTree = tostring(v.idleTree)
        item.noBedTree = tostring(v.noBedTree)
        local actionTime = tonumber(v.actionTime)
        item.endTick = actionTime
        item.startTick = 0
        local group = Groups[item.aiPlayerId] or {}
        Groups[item.aiPlayerId] = group
        table.insert(Groups[item.aiPlayerId], item)
    end
    for _, group in pairs(Groups) do
        table.sort(group, function(a, b)
            return a.id < b.id
        end)
        for index, item in pairs(group) do
            if index > 1 then
                item.endTick = item.endTick + group[index - 1].endTick
                item.startTick = group[index - 1].endTick
            end
        end
    end
end

function AIActionConfig:getActionGroup(aiPlayerId)
    return Groups[tostring(aiPlayerId)]
end