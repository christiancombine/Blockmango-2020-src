local GUIBuyHallGoldContent = class("GUIBuyHallGoldContent", IGUILayout)

function GUIBuyHallGoldContent:onLoad()
    self.stLackNum = self:getChildWindow("BuyHallGoldContent-lack_num", GUIType.StaticText)
    self.stDiamondsNum = self:getChildWindow("BuyHallGoldContent-diamonds_num", GUIType.StaticText)
    self.stHallGoldNum = self:getChildWindow("BuyHallGoldContent-hallgold_num", GUIType.StaticText)

    self.stLackTip = self:getChildWindow("BuyHallGoldContent-lack_tip", GUIType.StaticText)
    self.stExchangeTip = self:getChildWindow("BuyHallGoldContent-exchange_tip", GUIType.StaticText)

    self.stLackTip:SetText(Lang:getString("gui_lack_tip"))
    self.stExchangeTip:SetText(Lang:getString("gui_exchange_tip"))
end

BuyHallGoldContent = buildCustomDialogContent(GUIBuyHallGoldContent, "BuyHallGoldContent.json")

function BuyHallGoldContent:setCurrencyInfo(LackNum, DiamondsNum, HallGoldNum)
    self.stLackNum:SetText(LackNum)
    self.stDiamondsNum:SetText(DiamondsNum)
    self.stHallGoldNum:SetText(HallGoldNum)
end