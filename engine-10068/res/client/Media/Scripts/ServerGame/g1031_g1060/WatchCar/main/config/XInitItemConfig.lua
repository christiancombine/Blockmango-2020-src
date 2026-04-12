XInitItemConfig = {}
XInitItemConfig.InitItems = {}
function XInitItemConfig:OnInit(configs)
    for _, config in pairs(configs) do
        local configTable = {}
        configTable.ItemId = tonumber(config.ItemId)
        configTable.ItemCount = tonumber(config.ItemCount)
        configTable.ItemMetaId = tonumber(config.ItemMetaId)
        configTable.BelongTeam = tonumber(config.BelongTeam)
        table.insert(self.InitItems, configTable)
    end
end

function XInitItemConfig:getInitItems(teamId)
    local initTable = {}
    for index = 1, #self.InitItems do
        local item = self.InitItems[index]
        if item.BelongTeam == 0 or item.BelongTeam == teamId then
            table.insert(initTable, item)
        end
    end
    return initTable
end
