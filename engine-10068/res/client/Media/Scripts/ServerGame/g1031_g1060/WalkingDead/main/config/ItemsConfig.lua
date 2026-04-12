ItemsConfig = {}

local ItemsConfigs = {}

function ItemsConfig:init(items)
    self:initItems(items)
end

function ItemsConfig:initItems(items)
    for _, item in pairs(items) do
        local Item = {
            id = tonumber(item.id) or 0,
            itemId = tonumber(item.itemId) or 0,
            itemMaxStackSize = tonumber(item.itemMaxStackSize) or 1,
            properties = self:parseProperties(item.property),
            name = item.name or "",
            dropActorName = item.dropActorName or "",
            effectName = item.effectName or "",
            quality = tonumber(item.quality) or 1
        }
        if Item.dropActorName == "#" then
            Item.dropActorName = ""
        end
        if Item.effectName == "#" then
            Item.effectName = ""
        end
        ItemsConfig:setItemMaxStackSize(Item.itemId, Item.itemMaxStackSize)
        if (Item.itemId >= 3307 and Item.itemId <= 3310) then
            HostApi.setBulletClipSetting(Item.itemId, Item.itemMaxStackSize)
        end
        ItemsConfigs[Item.itemId] = Item
    end

end

function ItemsConfig:setItemMaxStackSize(itemId, itemMaxStackSize)
    local itemStack = Item.getItemById(itemId)
    if itemStack ~= nil then
        itemStack:setMaxStackSize(itemMaxStackSize)
    end
end

function ItemsConfig:parseProperties(Propertys)
    return StringUtil.split(Propertys, "#")
end

function ItemsConfig:getItemsByItemId(id)
    local Item = ItemsConfigs[tonumber(id)]
    if Item ~= nil then
        return Item
    end
    return nil
end

function ItemsConfig:getItemTypeByItemId(id)
    local type = ItemsConfigs[tonumber(id)].type
    return type
end

function ItemsConfig:getItemPropertyByItemId(id)
    local Item = ItemsConfigs[tonumber(id)]
    if Item ~= nil then
        return Item.properties
    end
    return
end

function ItemsConfig:isLongRangeWeapon(id)
    local Item = ItemsConfigs[tonumber(id)]
    if Item ~= nil and Item.id > 30100 and Item.id < 30500  then
        return true
    end
    return false

end

return itemConfig