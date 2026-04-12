XPlayerShopItems = {}
XPlayerShopItems.configList = {}
XPlayerShopItems.shopName = ""
XPlayerShopItems.normalShopItems = {}
function XPlayerShopItems:OnInit(configs, shopName)

    for _, config in pairs(configs) do
        local index = config.GroupId
        if self.configList[index] == nil then
            self.configList[index] = {}
        end
        local tab = {}
        tab.ItemId = tonumber(config.ItemId)
        tab.ItemLv = tonumber(config.ItemLv)
        tab.ItemName = config.ItemName
        tab.ItemImage = config.ItemImage == '#' and '' or config.ItemImage
        tab.ItemDesc = config.ItemDesc == '#' and '' or config.ItemDesc
        tab.ItemPrice = tonumber(config.ItemPrice)
        tab.TeamId = tonumber(config.TeamId)
        tab.ItemMateId = tonumber(config.ItemMateId) or 1
        tab.LimitNumber = tonumber(config.LimitNumber)
        tab.FumoId = config.FumoId ~= "#" and tonumber(config.FumoId) or nil
        tab.FumoLv = config.FumoLv ~= '#' and tonumber(config.FumoLv) or nil
        tab.HitValue = tonumber(config.HitValue)
        if index == "0" then
            table.insert(self.normalShopItems, tab)
        else
            table.insert(self.configList[index], tab)
        end
    end
    for key, tab in pairs(self.configList) do
        table.sort(tab, function(a, b)
            return a.ItemLv < b.ItemLv
        end)
    end
    self.shopName = shopName
end

function XPlayerShopItems:getWeaponHitById(itemId)
    if type(itemId) == 'number' then
        for _, config in pairs(self.configList) do
            for index = 1, #config do
                local item = config[index]
                if itemId == item.ItemId then
                    return item.HitValue
                end
            end
        end
    end
    return -1
end

function XPlayerShopItems:getNormalItemLimit(itemid)
    local itemIdNumber = tonumber(itemid)
    if type(itemIdNumber) == 'number' then
        for _, item in pairs(self.normalShopItems) do
            if item.ItemId == itemIdNumber then
                return item.LimitNumber
            end
        end
    end
    return nil
end

function XPlayerShopItems:getFumoInfo(itemId)
    for _, config in pairs(self.configList) do
        for index = 1, #config do
            local item = config[index]
            if itemId == item.ItemId then
                return item.FumoId, item.FumoLv
            end
        end
    end
    return nil
end

--  获取指定Id物品的 价格  返回价格和下一级物品的Id
function XPlayerShopItems:getPriceTyIteamId(itemId, itemLv)
    for _, config in pairs(self.configList) do
        for index = 1, #config do
            local item = config[index]
            if itemId == item.ItemId then
                local nextItem = config[index + itemLv]
                local curItem = config[index + itemLv - 1]
                if nextItem ~= nil and curItem ~= nil then
                    return curItem.ItemPrice, nextItem.ItemId
                end
            end
        end
    end
    return nil
end

function XPlayerShopItems:getMaxLevel(itemId)
    for _, config in pairs(self.configList) do
        for index = 1, #config do
            local item = config[index]
            if item and item.ItemId == itemId then
                return #config
            end
        end
    end
    return nil
end

function XPlayerShopItems:getItemInfoByItemId(itemId)
    for _, config in pairs(self.configList) do
        for _, item in pairs(config) do
            if item.ItemId == itemId then
                return {
                    id = tostring(item.ItemId),
                    name = item.ItemName,
                    image = item.ItemImage,
                    desc = item.ItemDesc,
                    price = item.ItemPrice
                }
            end
        end
    end
    return nil
end

-- 获取初始化的 物品
function XPlayerShopItems:getPrimaryWeapon(teamId)
    local data = {}
    for _, config in pairs(self.configList) do
        if type(config) == "table" and #config > 0 then
            local prop = config[1]
            if prop.TeamId == 0 or prop.TeamId == teamId then
                table.insert(data, prop.ItemId)
            end
        end
    end
    return data
end

function XPlayerShopItems:getInitWeaponId(itemId)
    for _, config in pairs(self.configList) do
        for index = 1, #config do
            local item = config[index]
            if item and item.ItemId == itemId then
                return config[1].ItemId
            end
        end
    end
    return nil
end

function XPlayerShopItems:HasNormalShopItem(itemid)
    local itemIdNumber = tonumber(itemid)
    if type(itemIdNumber) == 'number' then
        for _, item in pairs(self.normalShopItems) do
            if item.ItemId == itemIdNumber then
                return true
            end
        end
    end
    return false
end

function XPlayerShopItems:GetNormalItemFumoInfo(itemid)
    local itemIdNumber = tonumber(itemid)
    if type(itemIdNumber) == 'number' then
        for _, item in pairs(self.normalShopItems) do
            if item.ItemId == itemIdNumber then
                return item.FumoId, item.FumoLv
            end
        end
    end
    return false
end

function XPlayerShopItems:GetNormalItemPrice(itemid)
    local itemIdNumber = tonumber(itemid)
    if type(itemIdNumber) == 'number' then
        for _, item in pairs(self.normalShopItems) do
            if item.ItemId == itemIdNumber then
                return item.ItemPrice
            end
        end
    end
    return nil
end

function XPlayerShopItems:UpdateItemBuyState(itemid)
    local itemIdNumber = tonumber(itemid)
    if type(itemIdNumber) ~= 'number' then
        return nil
    end
    for _, prop in pairs(self.normalShopItems) do
        if itemIdNumber == prop.ItemId then
            return
            {{
                id = tostring(prop.ItemId),
                name = prop.ItemName,
                image = prop.ItemImage,
                desc = prop.ItemDesc,
                groupId = "1",
                price = 0,
                status = 2
            }}
        end
    end
    return nil
end

function XPlayerShopItems:getPrimaryItemByTeamId(teamId)
    assert(teamId)

    local data = {}
    local item = {}
    item.id = "1"
    item.name = self.shopName
    item.props = {}

    for index, config in pairs(self.configList) do
        if type(config) == 'table' and #config >= 1 then
            local prop = config[1]
            if prop.TeamId == 0 or prop.TeamId == teamId then
                table.insert(item.props,
                {
                    id = tostring(prop.ItemId),
                    name = prop.ItemName,
                    image = prop.ItemImage,
                    desc = prop.ItemDesc,
                    price = prop.ItemPrice,
                    status = 5
                })
            end
        end
    end
    if type(self.normalShopItems) == 'table' then
        for _, prop in pairs(self.normalShopItems) do
            if prop.TeamId == 0 or prop.TeamId == teamId then
                table.insert(item.props,
                {
                    id = tostring(prop.ItemId),
                    name = prop.ItemName,
                    image = prop.ItemImage,
                    desc = prop.ItemDesc,
                    price = prop.ItemPrice,
                    status = 6
                })
            end
        end
    end
    table.insert(data, item)
    return data
end