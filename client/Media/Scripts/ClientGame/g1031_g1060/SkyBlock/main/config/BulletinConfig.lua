BulletinConfig = {}
BulletinConfig.List = {}
function BulletinConfig:initConfig(configs)
    for _, config in pairs(configs) do
        local item = {
            Id = tonumber(config.Id) or 1,
            TabId = tonumber(config.TabId) or 0,
            GroupId = tonumber(config.GroupId) or 0,
            GroupName = tostring(config.GroupName) or "",
            Img = tostring(config.Img) or "",
            Title = tostring(config.Title) or "",
            Detail = tostring(config.Detail) or "",
            UpdateTime = tostring(config.UpdateTime) or ""
        }
        self.List[item.Id] = item
    end
end

function BulletinConfig:getTabDataByTabId(id)
    local tabData = {}
    for _, v in pairs(self.List) do
        if v.TabId == id then
            local isHave = false
            for _, groupId in pairs(tabData) do
                if groupId == v.GroupId then
                    isHave = true
                    break
                end
            end

            if isHave == false then
                table.insert(tabData, v)
            end
        end
    end
    return tabData
end

function BulletinConfig:getDataByTabIdandGroupId(tabId, groupId)
    for _, v in pairs(self.List) do
        if v.TabId == tabId and v.GroupId == groupId then
            return v
        end
    end
    return nil
end

return BulletinConfig