require "util.Tools"
StoreHouseItems = {}
local function NewItemConfig(config)
    local ItemId = tonumber(config.ItemId)
    local tab = {
        id = tonumber(config.id),
        ItemId = ItemId,
        actorModelName = config.actorModelName or "",
        actorBody = config.actorBody or "",
        actorBodyId = config.actorBodyId or "",
        icon = config.icon,
        ItemName = config.ItemName or "",
        yaw = config.yaw or 0,
        pos = Tools.CastToVector3(config.x, config.y, config.z),
        price = tonumber(config.ItemPrice),
        currencyType = tonumber(config.MoneyType),
        ItemType = tonumber(config.ItemType),
        ItemDesc = config.ItemDesc or "",
        actorName = config.actorName or "",
        actorId = tonumber(config.actorId),
        BindStoreId = tonumber(config.BindStoreId),
        iconNew = config.iconNew
    }
    local priceTab = StringUtil.split(config.ItemPrice, "#")
    local moneyTypeTab = StringUtil.split(config.MoneyType, "#")
    if #priceTab ~= #moneyTypeTab then
        print("StoreHouseConfig  :price and money type config error")
        return
    end
    return ItemId, tab
end

function StoreHouseItems:Init(configs)
    self.itemConfigs = {}
    for _, config in pairs(configs) do
        local id, tab = NewItemConfig(config)
        if type(id) == "number" and type(tab) == "table" then
            self.itemConfigs[id] = tab
        end
    end
end

function StoreHouseItems:getItemInfo(itemId)
    local itemInfo = self.itemConfigs[itemId]
    return itemInfo
end

function StoreHouseItems:GetItemInfoByStoreId(storeId)
    local tab = {}
    for id, item in pairs(self.itemConfigs) do
        if item.BindStoreId == storeId then
            tab[id] = item
        end
    end
    return tab
end
