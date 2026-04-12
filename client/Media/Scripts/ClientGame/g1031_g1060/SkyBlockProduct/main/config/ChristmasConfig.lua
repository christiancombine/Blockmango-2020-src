ChristmasConfig = {}
ChristmasConfig.List = {}
function ChristmasConfig:initConfig(configs)
    for _, config in pairs(configs) do
        local item = {
            Id = tonumber(config.Id) or 1,
            GiftType = tonumber(config.GiftType) or 1,
            ItemId = tonumber(config.ItemId) or 0,
            GiftBlockId = tonumber(config.GiftBlockId) or 0,
            GiftBlockMeta = tonumber(config.GiftBlockMeta) or 0,
            Num = tonumber(config.Num) or 1,
            Name = tostring(config.Name) or "",
            Img = tostring(config.Img) or "",
            BigImg = tostring(config.BigImg) or "",
            BuyPriceType = tonumber(config.BuyPriceType) or 0,
            BuyPriceNum = tonumber(config.BuyPriceNum) or 0,
            OpenType = tonumber(config.OpenType) or 0,
            OpenMoneyNum = tonumber(config.OpenMoneyNum) or 0,
            OpenDiamondNum = tonumber(config.OpenDiamondNum) or 0,
            Reward = StringUtil.split(tostring(config.Reward), "#") or {},
            Weights = StringUtil.split(tostring(config.Weights), "#") or {},
        }
        table.insert(self.List, item)
    end
end

function ChristmasConfig:getGiftdata()
    return self.List or {}
end

function ChristmasConfig:getGiftDataSize()
    return #self.List
end

function ChristmasConfig:getGiftdataBytype(giftType)
    for _, v in pairs(self.List) do
        if tonumber(giftType) == tonumber(v.GiftType) then
            return v
        end
    end
    return nil
end

function ChristmasConfig:getGiftdataByIndex(index)
    for k, v in pairs(self.List) do
        if tonumber(k) == tonumber(index) then
            return v
        end
    end
    return nil
end

function ChristmasConfig:getGiftdataByItemId(itemId)
    for k, v in pairs(self.List) do
        if tonumber(v.ItemId) == tonumber(itemId) then
            return v
        end
    end
    return nil
end
return ChristmasConfig