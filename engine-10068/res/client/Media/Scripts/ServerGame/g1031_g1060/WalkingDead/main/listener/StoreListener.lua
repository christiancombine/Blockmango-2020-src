StoreListener = {}
function StoreListener:init()
    BuyWalkingDeadStoreGoodsEvent:registerCallBack(self.onPlayerBuyWalkingDeadStoreGoods)
    BuyWalkingDeadStoreSupplyEvent:registerCallBack(self.onPlayerBuyWalkingDeadStoreSupply)
end

function StoreListener.onPlayerBuyWalkingDeadStoreGoods(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local goodsId = DataBuilder.new():fromData(data):getNumberParam("goodsId")
    GameStore:onPlayerShopping(player, goodsId)
end

function StoreListener.onPlayerBuyWalkingDeadStoreSupply(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local supplyId = DataBuilder.new():fromData(data):getNumberParam("supplyId")
    GameStore:onPlayerBuySupplyBox(player, supplyId)
end