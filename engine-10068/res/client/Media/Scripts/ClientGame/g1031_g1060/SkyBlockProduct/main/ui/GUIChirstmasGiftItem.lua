UIChirstmasGiftItem = class("UIChirstmasGiftItem", IGUIWindow)

function UIChirstmasGiftItem:onLoad()
    print("UIChirstmasGiftItem:onLoad()")
    self.giftIconImg = self:getChildWindow(1, GUIType.StaticImage)
    self.buyBtn = self:getChildWindow(2):GetChildByIndex(0)
    self.buyBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onBuyButtonEvent()
    end)
    self.stPriceNum = self:getChildWindow(2):GetChildByIndex(1)
    self.priceIconImg = self:getChildWindow(2, GUIType.StaticImage, self:getChildWindow(2))
    self.data = {}

end

function UIChirstmasGiftItem:onBuyButtonEvent()
    print("self.data.GiftType == "..self.data.GiftType)
    local player = Game:getPlayer()
    local inv = player:getInventory()
    if not inv:isCanAddItem(self.data.ItemId, 0, self.data.Num) then
        GUITipDialog:onShowTipDialog(Lang:getString("gui_inventory_not_enough"), GUITipDialog.TipType.COMMON)
        return
    end

    local data = {
        giftType = self.data.GiftType
    }

    GameConfig:checkMoney(self.data.BuyPriceType, self.data.BuyPriceNum, function(isCan)
        if isCan then
            GamePacketSender:sendBuyChirstmasGift(data)
        end
    end)
end

function UIChirstmasGiftItem:setItemData(data)
    print("UIChirstmasGiftItem:setItemData")
    self.data = data
    self.stPriceNum:SetText(tostring(self.data.BuyPriceNum))
    self.priceIconImg:SetImage(PayHelper.getMoneyIcon(self.data.BuyPriceType))
    self.giftIconImg:SetImage(self.data.BigImg)
end
