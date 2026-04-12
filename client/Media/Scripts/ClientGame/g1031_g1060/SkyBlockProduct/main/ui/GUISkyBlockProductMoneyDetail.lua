local UISkyBlockProductMoneyDeatil = class("UISkyBlockProductMoneyDeatil", IGUILayout)

function UISkyBlockProductMoneyDeatil:onLoad()
	print("UISkyBlockProductMoneyDeatil:onLoad()")
	self.moneyIconImg = self:getChildWindow("SkyBlockProductMoneyDeatil-Money-Icon", GUIType.StaticImage)
    self.moneyIconImg:SetImage(PayHelper.getMoneyIcon(MoneyType.Currency))
    self.stMoneyNum = self:getChildWindow("SkyBlockProductMoneyDeatil-Money-Num")

    self.addMoneyBtn = self:getChildWindow("SkyBlockProductMoneyDeatil-Add-Money", GUIType.Button)
    self.addMoneyBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickAddMoneyButton()
	end)

    self.daimondIconImg = self:getChildWindow("SkyBlockProductMoneyDeatil-Diamond-Icon", GUIType.StaticImage)
    self.daimondIconImg:SetImage(PayHelper.getMoneyIcon(MoneyType.Diamonds))
    self.stDiamondNum = self:getChildWindow("SkyBlockProductMoneyDeatil-Diamond-Num")

    self.addDiamondBtn = self:getChildWindow("SkyBlockProductMoneyDeatil-Add-Diamond", GUIType.Button)
    self.addDiamondBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickAddDiamondButton()
    end)
end

function UISkyBlockProductMoneyDeatil:onClickAddMoneyButton()
    GUISkyBlockProductBuyMoneyTips:onShowLackMoney()
    local data =
    {
        type = 11
    }
    GamePacketSender:sendCommonClick(data)
end

function UISkyBlockProductMoneyDeatil:onClickAddDiamondButton()
    ClientHelper.onRecharge(1)
    local data =
    {
        type = 12
    }
    GamePacketSender:sendCommonClick(data)
end

function UISkyBlockProductMoneyDeatil:onTick(frameTime)
    self.stMoneyNum:SetText(PlayerWallet.getMoneyByCoinId(MoneyType.Currency))
    self.stDiamondNum:SetText(PlayerWallet.getMoneyByCoinId(MoneyType.Diamonds))
end

GUISkyBlockProductMoneyDeatil = UISkyBlockProductMoneyDeatil.new("SkyBlockProductMoneyDeatil.json")
GUISkyBlockProductMoneyDeatil:hide()