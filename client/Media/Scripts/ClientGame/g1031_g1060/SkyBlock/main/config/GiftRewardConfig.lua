GiftRewardConfig = {}
GiftRewardConfig.List = {}
function GiftRewardConfig:initConfig(configs)
    for _, config in pairs(configs) do
        local item = {
            Id = tonumber(config.Id) or 1,
            ItemId = tonumber(config.ItemId) or 0,
            Meta = tonumber(config.Meta) or 0,
            Num = StringUtil.split(tostring(config.Num), "#") or {},
            Icon = tostring(config.Icon) or "",
            Type = tonumber(config.Type) or "",
            Name = tostring(config.Name) or "",
            Detail = tostring(config.Detail) or "",
        }
        self.List[item.Id] = item
    end
end

function GiftRewardConfig:getRewardById(id)
    for k, v in pairs(self.List) do
        if tonumber(v.Id) == tonumber(id) then
            return v
        end
    end
    return nil
end

return GiftRewardConfig