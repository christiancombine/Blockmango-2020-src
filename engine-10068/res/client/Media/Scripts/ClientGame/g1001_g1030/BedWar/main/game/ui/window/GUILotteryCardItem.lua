GUILotteryCardItem = class("GUILotteryCardItem", IGUIWindow)

function GUILotteryCardItem:onLoad()
    self.siCardBg = self:getChildWindowByName("BedWarLotteryCard-bg", GUIType.StaticImage)
    self.siItem = self:getChildWindowByName("BedWarLotteryCard-item", GUIType.StaticImage)
    self.siEnchant = self:getChildWindowByName("BedWarLotteryCard-enchant", GUIType.StaticImage)

    self.llMask = self:getChildWindowByName("BedWarLotteryCard-mask", GUIType.Layout)

    self.stNum = self:getChildWindowByName("BedWarLotteryCard-num", GUIType.StaticText)
    self.stName = self:getChildWindowByName("BedWarLotteryCard-name", GUIType.StaticText)
    self.stDesc = self:getChildWindowByName("BedWarLotteryCard-desc", GUIType.StaticText)
end

function GUILotteryCardItem:onDataChanged(data)
    self.siItem:SetImage(data.Image)
    self.stName:SetText(Lang:getString(data.Name))
    if data.IsCore == "1" then
        self.siCardBg:SetImage("set:bed_war_lottery.json image:item_bg")
    else
        self.siCardBg:SetImage("set:bed_war_lottery.json image:item_precious_bg")
    end

    if data.isGet == false then
        self.llMask:SetVisible(false)
    else
        LuaTimer:schedule(function()
            self.llMask:SetVisible(true)
        end, 300)
    end

    if data.IsEnchant == "1" then
        self.siEnchant:SetVisible(true)
    else
        self.siEnchant:SetVisible(false)
    end

    if data.Desc ~= "#" then
        self.stDesc:SetText(Lang:getString(data.Desc))
        self.stDesc:SetVisible(true)
        self.stNum:SetVisible(false)
    else
        self.stDesc:SetText("")
        self.stNum:SetText("x" .. tostring(data.CountText))

        self.stDesc:SetVisible(false)
        self.stNum:SetVisible(true)
    end

end

return GUILotteryCardItem
