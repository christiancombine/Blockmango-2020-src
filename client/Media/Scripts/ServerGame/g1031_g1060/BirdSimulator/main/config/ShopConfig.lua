ShopConfig = {}
ShopConfig.labels = {}
ShopConfig.teams = {}
ShopConfig.items = {}

function ShopConfig:initLabels(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.name = v.name
        data.pic = v.pic
        self.labels[data.id] = data
    end
end

function ShopConfig:initTeams(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.teamId = tonumber(v.teamId)
        data.name = v.name
        data.pic = v.pic
        data.bgPic = v.bgPic
        data.desc = v.desc
        data.moneyType = tonumber(v.moneyType)
        data.price = tonumber(v.price)
        data.limit = tonumber(v.limit)
        data.discount = v.discount == "@@@" and "" or v.discount
        data.topLeftBg = v.topLeftBg
        data.topLeftDesc = v.topLeftDesc
        data.topRightBg = v.topRightBg
        data.topRightDesc = v.topRightDesc
        data.timeLeft = tonumber(v.timeLeft)
        data.shopLabel = tonumber(v.shopLabel)
        self.teams[data.id] = data
    end
end

function ShopConfig:GetgiftbagInfo(lableId)
    local tab = {}
    for k, data in pairs(self.teams) do
        if data.shopLabel == lableId then
            table.insert(tab,data )
        end
    end
    return tab
end

function ShopConfig:getPackageById(grouId)
    for k, data in pairs(self.teams) do
        if data.teamId == grouId then
            return data
        end
    end
    return nil
end

function ShopConfig:initItems(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.type = tonumber(v.type)
        data.itemId = tonumber(v.itemId)
        data.meta = tonumber(v.meta)
        data.num = tonumber(v.num)
        data.teamId = tonumber(v.teamId)
        data.autoEquip = tonumber(v.autoEquip)
        data.icon = v.icon
        data.name = v.name
        self.items[data.id] = data
    end
end

function ShopConfig:GetItemInfoByGiftbagid(id)
    local tab = {}
    for _, v in pairs(self.items) do
        if v.teamId == id then
            table.insert(tab, v)
        end
    end
    return tab
end

function ShopConfig:GetStoreLableInfo()
    return self.labels
end

return ShopConfig
