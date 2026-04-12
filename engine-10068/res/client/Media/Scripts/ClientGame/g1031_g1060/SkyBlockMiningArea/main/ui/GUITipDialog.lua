local UITipDialog = class("UITipDialog", IGUILayout)

UITipDialog.TipType = 
{
	COMMON = 1,
	DELETE = 2,
	DELETE_PLUS = 3,
}

function UITipDialog:onLoad()
	print("UITipDialog:onLoad()")

	self.title = self:getChildWindow("SkyBlockMiningAreaTipDialog-Titile-Name")
	self.title:SetText(Lang:getString("gui_dialog_tip_title_tip"))

	self.cancelBtn = self:getChildWindow("SkyBlockMiningAreaTipDialog-Cancel", GUIType.Button)
	self.cancelBtn:SetText(Lang:getString("gui_dialog_tip_btn_cancel"))
    self.cancelBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickCancle()
	end)

	self.sureBtn = self:getChildWindow("SkyBlockMiningAreaTipDialog-Sure", GUIType.Button)
	self.sureBtn:SetText(Lang:getString("gui_dialog_tip_btn_sure"))
    self.sureBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickSure()
	end)

	self.closeBtn = self:getChildWindow("SkyBlockMiningAreaTipDialog-Close-Btn", GUIType.Button)
    self.closeBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickClose()
	end)

	self.pricePanel = self:getChildWindow("SkyBlockMiningAreaTipDialog-Price-Panel", GUIType.Layout)
	self.pricePanel:SetVisible(false)

	self.message = self:getChildWindow("SkyBlockMiningAreaTipDialog-Message")
	self.smallMessage = self:getChildWindow("SkyBlockMiningAreaTipDialog-Small-Tips")
	
	self:registerEvent(InventoryNotEnoughEvent, function ()
        RootGuiLayout.Instance():showMainControl()
		self:onShowTipDialog(Lang:getString("gui_inventory_not_enough"), GUITipDialog.TipType.COMMON)
    end)
    
	self.tipDialogType = UITipDialog.TipType.COMMON
	self.tipMsg = ""
	self.callBack = nil
	self.smallTips = ""
end

function UITipDialog:onClickCancle()
	self:hide()
	if self.callBack then
		self.callBack(false)
		self.callBack = nil
	end
end

function UITipDialog:onClickClose()
	self:hide()
	if self.callBack then
		self.callBack(false)
		self.callBack = nil
	end
end

function UITipDialog:onClickSure()
	self:hide()
	if self.callBack then
		self.callBack(true)
		self.callBack = nil
	end
end

function UITipDialog:onShowTipDialog(msg, tipType, smallTips, callBack)
	self.callBack = callBack
	self:show()
	self.tipDialogType = tipType or UITipDialog.TipType.COMMON
	self.tipMsg = msg
	self.smallTips = smallTips or ""

	self.message:SetText(self.tipMsg)
	self.smallMessage:SetText(self.smallTips)
	self:refresh()
end

function UITipDialog:refresh()
	if self.tipDialogType == UITipDialog.TipType.COMMON then
		self.pricePanel:SetVisible(false)
	end
end

GUITipDialog = UITipDialog.new("SkyBlockMiningAreaTipDialog.json")
GUITipDialog:hide()