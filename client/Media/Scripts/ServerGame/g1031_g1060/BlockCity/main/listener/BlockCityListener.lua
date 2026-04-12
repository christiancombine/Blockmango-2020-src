require "manager.TigerLotteryManager"
BlockCityListener = {}

function BlockCityListener:init()
    BlockCityStorageOperationEvent.registerCallBack(self.onBlockCityStorageOperation)
    BlockCityManorPraiseEvent.registerCallBack(self.onBlockCityManorPraise)
end

function BlockCityListener.onBlockCityStorageOperation(userId, isAdd, itemId, meta, hotBarIndex)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return false
    end

    player:removeItem(itemId, 1, meta)
    player:removeTempHotBar(itemId, meta)

    if not isAdd  then
        return true
    end

    if player:getEditMode() then
        player:addTempHotBar(itemId, meta, player.hotBar.edit, hotBarIndex)
    else
        player:addTempHotBar(itemId, meta, player.hotBar.normal, hotBarIndex)
    end
    return true
end

function BlockCityListener.onBlockCityManorPraise(userId, targetId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    if tostring(userId) == tostring(targetId) then
        HostApi.showBlockCityCommonTip(player.rakssid, {}, 0, "can_not_praise_you_self")
        return
    end

    if not player:checkCanPraiseByTargetId(targetId) then
        HostApi.showBlockCityCommonTip(player.rakssid, {}, 0, "already_praise_today")
        return
    end

    if not player:checkHasPraiseNum() then
        local roseInfo = RoseConfig:getDefaultRoseInfo()
        local shopInfo = ShopHouseManager:getItemInfosByItemId(roseInfo.itemId)
        if shopInfo then
            local lackItems = {
                id = roseInfo.uniqueId,
                itemId = roseInfo.itemId,
                needNum = 1,
                haveNum = 0,
                itemMeta = -1,
                price = shopInfo.itemPrice
            }
            local lackItemsInfo = {}
            table.insert(lackItemsInfo, lackItems)
            local items = {
                price = shopInfo.itemPrice,
                type = 1,
                items = lackItemsInfo,
                priceCube = math.ceil(shopInfo.itemPrice / 100)
            }
            player:setBuyLackItems(items)
            local itemInfo = {
                type = 5,       -- 5是玫瑰花
                master = player,
                items = lackItemsInfo,
                moneyPrice = items.price,
            }
            player:setLackItems(itemInfo)
        end

        return
    end

    if player:praiseManor(targetId, 0) then
        HostApi.sendPlayingPraiseAction(player.rakssid, 3000)
    end
end

return BlockCityListener
