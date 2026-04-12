UILotteryCard2 = class("BedWarLotteryCard2", IGUIWindow)

function UILotteryCard2:onLoad()

    self.llCard = self:getChildWindow(1, GUIType.Layout)
    self.ivPriceBackGround = self:getChildWindow(0, GUIType.StaticImage, self.llCard);
    self.ivFreeBackGround = self:getChildWindow(1, GUIType.StaticImage, self.llCard);
    -- self.priceKey = self:getChildWindow(0, GUIType.StaticText, self:getChildWindow(0, GUIType.StaticImage, self.llCard));
    self.tvFree = self:getChildWindow(0, GUIType.StaticText, self:getChildWindow(1, GUIType.StaticImage, self.llCard));
    self.tvPrice = self:getChildWindow(1, GUIType.StaticText, self:getChildWindow(0, GUIType.StaticImage, self.llCard));
    self.btnButton = self:getChildWindow(2, GUIType.Button, self.llCard);
    self.awActor = self:getChildWindow(0, GUIType.ActorWindow)
    self.ivImage = self:getChildWindow(2, GUIType.EffectViewOfObtainment);
    self.tvRewardNum = self:getChildWindow(3, GUIType.StaticText);

    self.tvFree:SetText(Lang:getString("gui_bed_war_lottery_free"))

    self.awActor:SetActor1("g1008_3kapai.actor")
    self.awActor:PlaySkill("idle")
    self.awActor:SetActorScale(self.awActor:GetPixelSize().x / 230.0)

    self.btnButton:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickCard()
    end)

end

function UILotteryCard2:init(i)
    self.index = i
    self.CardInfo = {}
    self.CardInfo.isPress = false
    self.CardInfo.nowPrice = 0
end

function UILotteryCard2:onClickCard()
    if not self.CardInfo.isPress then
        PayHelper.checkMoney(Define.Money.KEY, tonumber(self.CardInfo.nowPrice), function(canBuy)
            if canBuy then
                GamePacketSender:sendBedWarLottery(self.index)
            else
                CustomDialog.hide()
                GUIStore:show(StoreId.CubeStore, 8)
            end
        end)
        self.awActor:PlaySkill("touch")
    end
end

function UILotteryCard2:updatePrice(price)
    self.CardInfo.nowPrice = price
    if self.CardInfo.nowPrice == 0 then
        self.ivFreeBackGround:SetVisible(true)
        self.tvFree:SetText(Lang:getString("gui_bed_war_lottery_free"))
    else
        self.ivFreeBackGround:SetVisible(false)
        self.ivPriceBackGround:SetVisible(true)
        self.tvPrice:SetText(self.CardInfo.nowPrice)
    end
end

function UILotteryCard2:showCard()

    self.CardInfo.isPress = false
    self.awActor:PlaySkill("close")

    self.ivImage:SetVisible(false)
    self.llCard:SetVisible(true)
    self.tvRewardNum:SetVisible(false)
    -- delay = 1;
    self.ivPriceBackGround:SetVisible(false)
    self.ivFreeBackGround:SetVisible(false)
    LuaTimer:cancel(self.showBackTimer)
    self.showBackTimer = LuaTimer:schedule(function()
        if tonumber(self.CardInfo.nowPrice) > 0 then
            self.ivPriceBackGround:SetVisible(true)
        else
            self.ivFreeBackGround:SetVisible(true)
        end
    end, 800)
end

function UILotteryCard2:onOpenCard(rewardId)
    self.CardInfo.isPress = true;
    local reward = LotteryConfig.findRewardById(rewardId)
    if #reward then

        self.llCard:SetVisible(false)
        self.awActor:PlaySkill("open")
        --放置图片
        --放置数量
        LuaTimer:cancel(self.showOpenCardInfoTimer)
        self.showOpenCardInfoTimer = LuaTimer:schedule(function()
            self.ivImage:SetImage(tostring(reward.Image))
            self.tvRewardNum:SetText(Lang:getString(tostring(reward.CountText)))
            self.ivImage:SetVisible(true)
            self.tvRewardNum:SetVisible(true)

        end, 500)

    end
end

return UILotteryCard2
