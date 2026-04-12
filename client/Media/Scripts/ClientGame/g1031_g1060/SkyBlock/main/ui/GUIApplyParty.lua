local UIApplyParty = class("UIApplyParty", IGUILayout)

UIApplyParty.ClickType =
{
	CLOSE_PANEL = 1,
	CREATE_PARTY = 2,
	EDIT_NAME = 3,
	EDIT_DES = 4,
	APPLY = 5
}

function UIApplyParty:onLoad()
	print("UIApplyParty:onLoad()")
	self:initPartyVariable()
	self:onLoadInitUI()
end

function UIApplyParty:initPartyVariable()
	self.applyTime = 0
	self.haveParty = false
	self.curLastTime = 0
end

function UIApplyParty:onLoadInitUI()
	local partyTitle = self:getChildWindow("SkyBlockApplyParty-Title-Text")
	partyTitle:SetText(Lang:getString("gui_party_setting"))

	local paneName = self:getChildWindow("SkyBlockApplyParty-Title-Name")
	paneName:SetText(Lang:getString("gui_party_name"))

	local paneDes = self:getChildWindow("SkyBlockApplyParty-Des-Name")
	paneDes:SetText(Lang:getString("gui_party_des"))

	self.closePanel = self:getChildWindow("SkyBlockApplyParty-Close-Panel", GUIType.Layout)
    self.closePanel:registerEvent(GUIEvent.TouchUp, function()
    	self:onClickButton(UIApplyParty.ClickType.CLOSE_PANEL)
	end)


	self.partyNameEdit = self:getChildWindow("SkyBlockApplyParty-Title-Edit", GUIType.Edit)
    self.partyNameEdit:registerEvent(GUIEvent.EditTextInput, function()
    	self:onEditChange(UIApplyParty.ClickType.EDIT_NAME)
	end)

	self.partyDesEdit = self:getChildWindow("SkyBlockApplyParty-Des-Edit", GUIType.Edit)
    self.partyDesEdit:registerEvent(GUIEvent.EditTextInput, function()
    	self:onEditChange(UIApplyParty.ClickType.EDIT_DES)
	end)

	self.applyBtn = self:getChildWindow("SkyBlockApplyParty-Apply-Btn", GUIType.Button)
	self.applyBtn:SetText(Lang:getString("gui_party_apply"))
    self.applyBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickButton(UIApplyParty.ClickType.APPLY)
	end)

	self.createPartyName = self:getChildWindow("SkyBlockApplyParty-Title-Edit-Name")
	self.createPartyDes = self:getChildWindow("SkyBlockApplyParty-Des-Edit-Name")

end

function UIApplyParty:onClickButton(clickType)
	if clickType == UIApplyParty.ClickType.CLOSE_PANEL then
		self:hide()
		return
	end

	if clickType == UIApplyParty.ClickType.CREATE_PARTY then
		self:sendCreatePartyInfo()
		return
	end

	if clickType == UIApplyParty.ClickType.APPLY then
		if os.time() - self.applyTime < 3 then
			GUITipDialog:onShowTipDialog(Lang:getString("gui_apply_too_fast"), GUITipDialog.TipType.COMMON)
			return
		end
		self.applyTime = os.time()
		self:sendApplyPartyInfo()
		return
	end
end

function UIApplyParty:onEditChange(editType)
	local cur_text = ""
	if editType == UIApplyParty.ClickType.EDIT_NAME then
		cur_text = self.partyNameEdit:GetText()
		if cur_text ~= "" then
			self.createPartyName:SetText(cur_text)
		else
			local partyName = string.format(Lang:getString("who_of_island"), tostring(GameData.userName))
			self.partyNameEdit:SetText(partyName)
			self.createPartyName:SetText(partyName)
		end
	end

	if editType == UIApplyParty.ClickType.EDIT_DES then
		cur_text = self.partyDesEdit:GetText()
		self.createPartyDes:SetText(cur_text)
	end
end

function UIApplyParty:onCreatePartyInfoUpdate(data)
	print("UIApplyParty onCreatePartyInfoUpdate")
	if not self:isLoad() then
        return
    end

    if data then
    	self.haveParty = true
    	self.createPartyData = data
    else
    	self.haveParty = false
    	self.createPartyData = {}
		print("UIApplyParty onCreatePartyInfoUpdate 222")
    end
	self:upDateCreateParty()
end

function UIApplyParty:upDateCreateParty()
	if self.haveParty then
		self.createPartyName:SetText(self.createPartyData.partyName)
		self.createPartyDes:SetText(self.createPartyData.partyDesc)

		self.partyNameEdit:SetText(self.createPartyData.partyName)
		self.partyDesEdit:SetText(self.createPartyData.partyDesc)

		local endTime = self.createPartyData.leftTime / 1000
		local curTime = endTime - os.time()

		print("endTime == "..endTime)
		print("curTime == "..curTime)

	    if curTime <= 0 then
	    	curTime = 0
	    end
	    self.curLastTime = curTime
	end
end

function UIApplyParty:changeStatus()
	if self.curLastTime > 0 then
    	self.haveParty = true
    else
    	self.haveParty = false
    end
	self:upDateCreateParty()
end

function UIApplyParty:sendApplyPartyInfo()
	local partyName = self.createPartyName:GetText()
	local PartyDes = self.createPartyDes:GetText()

	if partyName == "" then
		partyName = string.format(Lang:getString("who_of_island"), tostring(GameData.userName))
	end

	local data = {
		partyName = partyName,
		partyDesc = PartyDes,
	}
	GamePacketSender:sendApplyParty(json.encode(data))
	self:hide()
end

GUIApplyParty = UIApplyParty.new(Lang:getString("SkyBlockApplyParty.json"), GUIParty.root)
GUIApplyParty:hide()