GUILotteryDetailItem = class("GUILotteryDetailItem", IGUIWindow)

function GUILotteryDetailItem:onLoad()
    self.siCardBg = self:getChildWindowByName("BedWarLotteryDetailItem-bg", GUIType.StaticImage)
    self.siItem = self:getChildWindowByName("BedWarLotteryDetailItem-item", GUIType.StaticImage)
    self.siEnchant = self:getChildWindowByName("BedWarLotteryDetailItem-enchant_bg", GUIType.StaticImage)
    self.stNum = self:getChildWindowByName("BedWarLotteryDetailItem-num", GUIType.StaticText)
end

function GUILotteryDetailItem:onDataChanged(data)
    self.siItem:SetImage(data.Image)
    if data.IsCore == "1" then
        self.siCardBg:SetImage("set:bed_war_lottery.json image:item_bg_small")
    else
        self.siCardBg:SetImage("set:bed_war_lottery.json image:item_precious_bg_small")
    end

    if data.IsEnchant == "1" then
        self.siEnchant:SetVisible(true)
    else
        self.siEnchant:SetVisible(false)
    end

    self.stNum:SetText("x" .. tostring(data.CountText))
end

return GUILotteryDetailItem