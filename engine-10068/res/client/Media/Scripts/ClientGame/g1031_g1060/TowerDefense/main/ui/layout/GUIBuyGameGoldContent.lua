local GUIBuyGameGoldContent = class("GUIBuyGameGoldContent", IGUILayout)

GUIBuyGameGoldContent.useDiamondNum = 0

function GUIBuyGameGoldContent:onLoad()

    self.llBackground = self:getChildWindow("BuyGameGoldContent", GUIType.Layout)

    self.stDiamondsNum = self:getChildWindow("BuyGameGoldContent-diamonds_num", GUIType.StaticText)
    self.stGameGoldNum = self:getChildWindow("BuyGameGoldContent-gamegold_num", GUIType.StaticText)
    self.stLimitTip = self:getChildWindow("BuyGameGoldContent-limit_tip", GUIType.StaticText)

    self.Slider = self:getChildWindow("BuyGameGoldContent-slider", GUIType.Slider)

    self.btnSub = self:getChildWindow("BuyGameGoldContent-sub_bt", GUIType.Button)
    self.btnAdd = self:getChildWindow("BuyGameGoldContent-add_bt", GUIType.Button)
    self.btnMax = self:getChildWindow("BuyGameGoldContent-max_bt", GUIType.Button)

    self.Slider:registerEvent(GUIEvent.TouchMove, function()
        self:setCurrencyInfo()
    end)

    self.Slider:registerEvent(GUIEvent.TouchUp, function()
        self:setCurrencyInfo()
    end)

    self.Slider:registerEvent(GUIEvent.TouchDown, function()
        self:setCurrencyInfo()
    end)

    self.btnSub:registerEvent(GUIEvent.TouchDown, function()
        self:subProgress()
    end)

    self.btnAdd:registerEvent(GUIEvent.TouchDown, function()
        self:addProgress()
    end)

    self.btnMax:registerEvent(GUIEvent.TouchDown, function()
        self:maxProgress()
    end)

    self.llBackground:registerEvent(GUIEvent.TouchMove, function()
        self:cancelLuaTimer()
    end)

    self.btnAdd:registerEvent(GUIEvent.LongTouchStart, function()
        self:startAddProgress()
    end)

    self.btnAdd:registerEvent(GUIEvent.LongTouchEnd, function()
        self:stopAddProgress()
    end)

    self.btnSub:registerEvent(GUIEvent.LongTouchStart, function()
        self:startSubProgress()
    end)

    self.btnSub:registerEvent(GUIEvent.LongTouchEnd, function()
        self:stopSubProgress()
    end)
end

BuyGameGoldContent = buildCustomDialogContent(GUIBuyGameGoldContent, "BuyGameGoldContent.json")

function GUIBuyGameGoldContent:setCurrencyInfo()
    local progress = self.Slider:GetProgress()
    progress = self:precisionCheck(progress, 10000)

    self.useDiamondNum = self:precisionCheck(progress * GamePlayer.Instance().exchangeLimit / GamePlayer.Instance().gameMoneyRate, 1)

    self.stDiamondsNum:SetText(self.useDiamondNum)
    self.stGameGoldNum:SetText(self.useDiamondNum * GamePlayer.Instance().gameMoneyRate)
    self.stLimitTip:SetText(self.useDiamondNum * GamePlayer.Instance().gameMoneyRate .. " / " .. GamePlayer.Instance().exchangeLimit)

end

function GUIBuyGameGoldContent:subProgress()
    local progress = self.Slider:GetProgress()
    self.useDiamondNum = self:precisionCheck(progress * GamePlayer.Instance().exchangeLimit / GamePlayer.Instance().gameMoneyRate, 1)
    if self.useDiamondNum <= 0 then
        self:setCurrencyInfo()
        return
    end

    self.useDiamondNum = self.useDiamondNum - 1
    progress = self.useDiamondNum * GamePlayer.Instance().gameMoneyRate / GamePlayer.Instance().exchangeLimit

    self.Slider:SetProgress(progress)
    self:setCurrencyInfo()
end

function GUIBuyGameGoldContent:addProgress()
    local progress = self.Slider:GetProgress()
    self.useDiamondNum = self:precisionCheck(progress * GamePlayer.Instance().exchangeLimit / GamePlayer.Instance().gameMoneyRate, 1)
    if self.useDiamondNum >= GamePlayer.Instance().exchangeLimit then
        self:setCurrencyInfo()
        return
    end

    self.useDiamondNum = self.useDiamondNum + 1

    progress = self.useDiamondNum * GamePlayer.Instance().gameMoneyRate / GamePlayer.Instance().exchangeLimit

    self.Slider:SetProgress(progress)
    self:setCurrencyInfo()
end

function GUIBuyGameGoldContent:maxProgress()
    self.Slider:SetProgress(1)
    self:setCurrencyInfo()
end

function GUIBuyGameGoldContent:precisionCheck(num, time)
    if math.abs(num - math.ceil(num * time) / time) < 0.0001 then
        return math.ceil(num * time) / time
    else
        return math.floor(num * time) / time
    end
end

function GUIBuyGameGoldContent:onClickBuy()
    local isNotCloseTip = false
    if self.useDiamondNum ~= 0 then
        PayHelper.checkMoney(MoneyType.Diamonds, self.useDiamondNum, function(isEnough)
            if isEnough then
                GamePacketSender:sendBuyPayItem(2003, self.useDiamondNum)
                isNotCloseTip = false
            else
                isNotCloseTip = true
            end
        end)
    end
    return isNotCloseTip
end

function GUIBuyGameGoldContent:startAddProgress()
    LuaTimer:cancel(self.addProgressLuaTimer)
    self.addProgressLuaTimer = LuaTimer:schedule(function()
        self:addProgress()
    end, 100, 50)
end

function GUIBuyGameGoldContent:stopAddProgress()
    LuaTimer:cancel(self.addProgressLuaTimer)
end

function GUIBuyGameGoldContent:startSubProgress()
    LuaTimer:cancel(self.subProgressLuaTimer)
    self.subProgressLuaTimer = LuaTimer:schedule(function()
        self:subProgress()
    end, 100, 50)
end

function GUIBuyGameGoldContent:stopSubProgress()
    LuaTimer:cancel(self.subProgressLuaTimer)
end

function GUIBuyGameGoldContent:cancelLuaTimer()
    LuaTimer:cancel(self.addProgressLuaTimer)
    LuaTimer:cancel(self.subProgressLuaTimer)
end

function GUIBuyGameGoldContent:onHide()
    self:cancelLuaTimer()
end