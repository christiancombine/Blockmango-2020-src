local UISkyBlockMoneyDeatil = class("UISkyBlockMoneyDeatil", IGUILayout)

function UISkyBlockMoneyDeatil:onLoad()
	print("UISkyBlockMoneyDeatil:onLoad()")
	self.moneyIconImg = self:getChildWindow("SkyBlockMoneyDeatil-Money-Icon", GUIType.StaticImage)
    self.moneyIconImg:SetImage(PayHelper.getMoneyIcon(MoneyType.Currency))
    self.stMoneyNum = self:getChildWindow("SkyBlockMoneyDeatil-Money-Num")

    self.addMoneyBtn = self:getChildWindow("SkyBlockMoneyDeatil-Add-Money", GUIType.Button)
    self.addMoneyBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickAddMoneyButton()
	end)

    self.daimondIconImg = self:getChildWindow("SkyBlockMoneyDeatil-Diamond-Icon", GUIType.StaticImage)
    self.daimondIconImg:SetImage(PayHelper.getMoneyIcon(MoneyType.Diamonds))
    self.stDiamondNum = self:getChildWindow("SkyBlockMoneyDeatil-Diamond-Num")

    self.addDiamondBtn = self:getChildWindow("SkyBlockMoneyDeatil-Add-Diamond", GUIType.Button)
    self.addDiamondBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickAddDiamondButton()
    end)
end

function UISkyBlockMoneyDeatil:onClickAddMoneyButton()
    GUIBuyMoneyTips:onShowLackMoney()
    local data =
    {
        type = 11
    }
    GamePacketSender:sendCommonClick(data)
end

function UISkyBlockMoneyDeatil:onClickAddDiamondButton()
    ClientHelper.onRecharge(1)
    local data =
    {
        type = 12
    }
    GamePacketSender:sendCommonClick(data)
end

function UISkyBlockMoneyDeatil:onTick(frameTime)
    self.stMoneyNum:SetText(PlayerWallet.getMoneyByCoinId(MoneyType.Currency))
    self.stDiamondNum:SetText(PlayerWallet.getMoneyByCoinId(MoneyType.Diamonds))
end

GUISkyBlockMoneyDeatil = UISkyBlockMoneyDeatil.new("SkyBlockMoneyDeatil.json")
GUISkyBlockMoneyDeatil:hide()