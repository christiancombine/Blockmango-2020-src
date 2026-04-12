local UISkyBlockMiningAreaMoneyDeatil = class("UISkyBlockMiningAreaMoneyDeatil", IGUILayout)

function UISkyBlockMiningAreaMoneyDeatil:onLoad()
	print("UISkyBlockMiningAreaMoneyDeatil:onLoad()")
	self.moneyIconImg = self:getChildWindow("SkyBlockMiningAreaMoneyDeatil-Money-Icon", GUIType.StaticImage)
    self.moneyIconImg:SetImage(PayHelper.getMoneyIcon(MoneyType.Currency))
    self.stMoneyNum = self:getChildWindow("SkyBlockMiningAreaMoneyDeatil-Money-Num")

    self.addMoneyBtn = self:getChildWindow("SkyBlockMiningAreaMoneyDeatil-Add-Money", GUIType.Button)
    self.addMoneyBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickAddMoneyButton()
	end)

    self.daimondIconImg = self:getChildWindow("SkyBlockMiningAreaMoneyDeatil-Diamond-Icon", GUIType.StaticImage)
    self.daimondIconImg:SetImage(PayHelper.getMoneyIcon(MoneyType.Diamonds))
    self.stDiamondNum = self:getChildWindow("SkyBlockMiningAreaMoneyDeatil-Diamond-Num")

    self.addDiamondBtn = self:getChildWindow("SkyBlockMiningAreaMoneyDeatil-Add-Diamond", GUIType.Button)
    self.addDiamondBtn:registerEvent(GUIEvent.ButtonClick, function()
        self:onClickAddDiamondButton()
    end)
end

function UISkyBlockMiningAreaMoneyDeatil:onClickAddMoneyButton()
    GUIBuyMoneyTips:onShowLackMoney()
    local data =
    {
        type = 11
    }
    GamePacketSender:sendCommonClick(data)
end

function UISkyBlockMiningAreaMoneyDeatil:onClickAddDiamondButton()
    ClientHelper.onRecharge(1)
    local data =
    {
        type = 12
    }
    GamePacketSender:sendCommonClick(data)
end

function UISkyBlockMiningAreaMoneyDeatil:onTick(frameTime)
    self.stMoneyNum:SetText(PlayerWallet.getMoneyByCoinId(MoneyType.Currency))
    self.stDiamondNum:SetText(PlayerWallet.getMoneyByCoinId(MoneyType.Diamonds))
end

GUISkyBlockMiningAreaMoneyDeatil = UISkyBlockMiningAreaMoneyDeatil.new("SkyBlockMiningAreaMoneyDeatil.json")
GUISkyBlockMiningAreaMoneyDeatil:hide()