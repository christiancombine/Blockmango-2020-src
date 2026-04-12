GamePacketHandler = {}

function GamePacketHandler:init()
    C2SBuyShopItem:registerCallBack(self.onBuyShopItem)
    C2SOpenBookDetail:registerCallBack(self.onOpenBookDetail)
    C2SExchangeHallGold:registerCallBack(self.exchangeHallGold)
    C2SBuySkinItem:registerCallBack(self.onBuySkinItem)
    C2SBChangeSkin:registerCallBack(self.onChangeSkin)
end

function GamePacketHandler.onBuyShopItem(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local builder = DataBuilder.new():fromData(data)
    local itemId = builder:getNumberParam("itemId")
    TowerStore:onBuyPayItem(player, itemId)
end

function GamePacketHandler.onOpenBookDetail(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local builder = DataBuilder.new():fromData(data)
    local towerId = builder:getNumberParam("towerId")
    player:onOpenBookDetail(towerId)
end

function GamePacketHandler.exchangeHallGold(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local builder = DataBuilder.new():fromData(data)
    local extraDiamondsNum = builder:getNumberParam("extraDiamondsNum")
    local itemId = builder:getNumberParam("itemId")
    TowerStore:onNeedBuyHallGold(player, extraDiamondsNum, itemId)
end

function GamePacketHandler.onBuySkinItem(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local builder = DataBuilder.new():fromData(data)
    local skinId = builder:getNumberParam("skinId")
    SkinStore:onBuyPaySkin(player, skinId)
end

function GamePacketHandler.onChangeSkin(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local builder = DataBuilder.new():fromData(data)
    local skinId = builder:getNumberParam("skinId")
    player:equipSkin(skinId)
end

return GamePacketSender