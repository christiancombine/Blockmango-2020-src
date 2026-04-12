local UIRankTip = class("UIRankTip", IGUILayout)

function UIRankTip:onLoad()
	print("UITipDialog:onLoad()")

	self.btnCloseBtn = self:getChildWindow("SkyBlockRankTip-Close-Btn", GUIType.Button)
    self.btnCloseBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickClose()
	end)

	self.stMessage = self:getChildWindow("SkyBlockRankTip-Titile-Name")
	self.stTitle = self:getChildWindow("SkyBlockRankTip-Message")
	self.stMessage:SetText("")
	self.stTitle:SetText("")
end

function UIRankTip:onClickClose()
	self:hide()
end

function UIRankTip:onShowTipDialog(type)
	self.stMessage:SetText(Lang:getString("gui_rank_tip_title" .. tostring(type)))
	self.stTitle:SetText(Lang:getString("gui_rank_message" .. tostring(type)))
	self:show()
end

GUIRankTip = UIRankTip.new("SkyBlockRankTip.json")
GUIRankTip:hide()