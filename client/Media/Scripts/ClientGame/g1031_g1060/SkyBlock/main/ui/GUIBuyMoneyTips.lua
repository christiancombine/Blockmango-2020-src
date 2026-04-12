local UIBuyMoneyTips = class("UIBuyMoneyTips", IGUILayout)

function UIBuyMoneyTips:onLoad()
	print("UIBuyMoneyTips:onLoad()")

	self.stTitle = self:getChildWindow("SkyBlockBuyMoneyTips-Titile-Name")
	self.stTitle:SetText(Lang:getString("gui_dialog_tip_title_tip"))


	self.stMessage = self:getChildWindow("SkyBlockBuyMoneyTips-Message")
	self.stMessage:SetText(Lang:getString("gui_buy_money_tips_left"))

	self.stSmallTips = self:getChildWindow("SkyBlockBuyMoneyTips-Small-Tips")

	self.stSelectText = self:getChildWindow("SkyBlockBuyMoneyTips-Money-Num-Text")
	self.stSelectText:SetText(Lang:getString("gui_select_buy_money_num"))

	self.stMoneyNum = self:getChildWindow("SkyBlockBuyMoneyTips-Select-Money-Num")
	self.moneySlider = self:getChildWindow("SkyBlockBuyMoneyTips-Money-Slider", GUIType.Slider)
	self.moneySlider:registerEvent(GUIEvent.TouchDown, function()
    	self:onSliderMove()
	end)
	self.moneySlider:registerEvent(GUIEvent.TouchMove, function()
    	self:onSliderMove()
	end)
	self.moneySlider:registerEvent(GUIEvent.TouchUp, function()
    	self:onSliderMove()
	end)

	self.reduceBtn = self:getChildWindow("SkyBlockBuyMoneyTips-Reduce-Btn", GUIType.Button)
	self.reduceBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onReduceProgress()
	end)

	self.addBtn = self:getChildWindow("SkyBlockBuyMoneyTips-Add-Btn", GUIType.Button)
	self.addBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onAddProgress()
	end)

	self.maxBtn = self:getChildWindow("SkyBlockBuyMoneyTips-Max-Btn", GUIType.Button)
	self.maxBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onMaxProgress()
	end)

	self.closeBtn = self:getChildWindow("SkyBlockBuyMoneyTips-Close-Btn", GUIType.Button)
    self.closeBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickClose()
	end)

	self.cancleBtn = self:getChildWindow("SkyBlockBuyMoneyTips-Cancel", GUIType.Button)
	self.cancleBtn:SetText(Lang:getString("gui_dialog_tip_btn_cancel"))
    self.cancleBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickClose()
	end)

	self.sureBtn = self:getChildWindow("SkyBlockBuyMoneyTips-Sure", GUIType.Button)
	self.sureBtn:SetText(Lang:getString("gui_dialog_tip_btn_sure"))
    self.sureBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickSure()
	end)

	self.level = 1
	self:refreshProgress()
	self:refreshCostNum()

	self:registerDataCallBack("ShowLackMoney", function(data)
        self:onShowLackMoney(data)
    end, JsonBuilderProcessor)
end

function UIBuyMoneyTips:onShowLackMoney()
	self:show()
end

function UIBuyMoneyTips:onReduceProgress()
	self.level = self.level - 1
	if self.level <= 0 then
		self.level = 1
	end
	self:refreshProgress()
end

function UIBuyMoneyTips:onAddProgress()
	self.level = self.level + 1
	if self.level >= 10 then
		self.level = 10
	end
	self:refreshProgress()
end

function UIBuyMoneyTips:onMaxProgress()
	self.level = 10
	self:refreshProgress()
end

function UIBuyMoneyTips:onSliderMove()
	self:refreshCostNum()
end

function UIBuyMoneyTips:refreshCostNum()
	local progress = self.moneySlider:GetProgress()
	self.level = math.ceil(progress / 0.1)
	if self.level <= 0 then
		self.level = 1
	end
	self:updatePanel()
end

function UIBuyMoneyTips:updatePanel()
	self.diamond = self.level * GameConfig.onceBuyMoneyCost
	self.money = self.level * GameConfig.onceMoneyNum
    local smallTipsText = string.format(Lang:getString("gui_buy_money_tips_right"), tostring(self.diamond), tostring(self.money))
    self.stSmallTips:SetText(smallTipsText)

    local costText = tostring(self.diamond).."/"..tostring(GameConfig.maxBuyMoneyCost)
    self.stMoneyNum:SetText(costText)
end

function UIBuyMoneyTips:refreshProgress()
    self.moneySlider:SetProgress(self.level / 10)
    self:updatePanel()
end

function UIBuyMoneyTips:onClickClose()
	self:hide()
end

function UIBuyMoneyTips:onClickSure()
	if self.diamond <= 0 then
		return
	end

	GameConfig:checkMoney(MoneyType.Diamonds, self.diamond, function(isCan)
		if isCan then
			local data = 
			{
				level = self.level
			}
			GamePacketSender:sendBuyMoneyUseDiamond(data)
		end
	end)
	self:hide()
end

GUIBuyMoneyTips = UIBuyMoneyTips.new("SkyBlockBuyMoneyTips.json")
GUIBuyMoneyTips:hide()