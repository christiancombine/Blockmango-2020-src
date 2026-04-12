local UICreateParty = class("UICreateParty", IGUILayout)

UICreateParty.ClickType =
{
	CLOSE_PANEL = 1,
	CREATE_PARTY = 2,
	EDIT_NAME = 3,
	EDIT_DES = 4,
	--APPLY = 5
}

UICreateParty.FREE_PARTY_TIMES = 5

UICreateParty.createType = 
{
    FREE = 1,
    NOT_FREE = 2,
}
function UICreateParty:onLoad()
	print("UICreateParty:onLoad()")
	self:initPartyVariable()
	self:onLoadInitUI()
end

function UICreateParty:initPartyVariable()
	self.selectItem = {}
	self.selectIndex = 1
	self.createPartyData = {}
	self.curLastTime = 0
	self.freeCreatePartyTimes = -1
end

function UICreateParty:onLoadInitUI()
	local partyTitle = self:getChildWindow("SkyBlockCreateParty-Title-Text")
	partyTitle:SetText(Lang:getString("gui_create_party"))

	local paneName = self:getChildWindow("SkyBlockCreateParty-Title-Name")
	paneName:SetText(Lang:getString("gui_party_name"))

	local paneDes = self:getChildWindow("SkyBlockCreateParty-Des-Name")
	paneDes:SetText(Lang:getString("gui_party_des"))

	local createBtnText = self:getChildWindow("SkyBlockCreateParty-Btn-Text")
	createBtnText:SetText(Lang:getString("gui_create_party"))

	local freeCreateText = self:getChildWindow("SkyBlockCreateParty-Free-Text")
	freeCreateText:SetText(Lang:getString("gui_free_create_party"))

	self.freeCreatePanel = self:getChildWindow("SkyBlockCreateParty-Free-Bg", GUIType.StaticImage)
	self.freeCreatePanel:SetVisible(false)

	self.priceIcon = self:getChildWindow("SkyBlockCreateParty-Cost-Money-Icon", GUIType.StaticImage)
	self.priceIcon:SetVisible(true)



	self.closePanel = self:getChildWindow("SkyBlockCreateParty-Close-Panel", GUIType.Layout)
    self.closePanel:registerEvent(GUIEvent.TouchUp, function()
    	self:onClickButton(UICreateParty.ClickType.CLOSE_PANEL)
	end)

	self.createBtn = self:getChildWindow("SkyBlockCreateParty-Buy-Btn", GUIType.Button)
    self.createBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickButton(UICreateParty.ClickType.CREATE_PARTY)
	end)

	self.partyNameEdit = self:getChildWindow("SkyBlockCreateParty-Title-Edit", GUIType.Edit)
    self.partyNameEdit:registerEvent(GUIEvent.EditTextInput, function()
    	self:onEditChange(UICreateParty.ClickType.EDIT_NAME)
	end)

	self.partyDesEdit = self:getChildWindow("SkyBlockCreateParty-Des-Edit", GUIType.Edit)
    self.partyDesEdit:registerEvent(GUIEvent.EditTextInput, function()
    	self:onEditChange(UICreateParty.ClickType.EDIT_DES)
	end)

	self.createPartyName = self:getChildWindow("SkyBlockCreateParty-Title-Edit-Name")
	self.createPartyDes = self:getChildWindow("SkyBlockCreateParty-Des-Edit-Name")
	self.createPartyName:SetText(Lang:getString("please_enter_a_party_name"))
	self.createPartyDes:SetText(Lang:getString("please_enter_a_party_des"))

	self.cur_select_price = self:getChildWindow("SkyBlockCreateParty-Cost-Money-Num")
	self.createPartyPanle = self:getChildWindow("SkyBlockCreateParty-Create-Party-Panel", GUIType.Layout)
	self.offPrice = self:getChildWindow("SkyBlockCreateParty-Off-Price-Text")
	self.offPrice:SetText(Lang:getString("create_party_off_text"))
	
	local moneyLayout = self:getChildWindow("SkyBlockCreateParty-Select-Money", GUIType.Layout)
	local btn_name = ""
	for i = 1, 3 do
 		local cur_Img = self:getChildWindow("SkyBlockCreateParty-Money-Bg"..tostring(i), GUIType.StaticImage)
 		local cur_time_text = self:getChildWindow("SkyBlockCreateParty-Money-Num"..tostring(i))
 		local select_Img = self:getChildWindow("SkyBlockCreateParty-Select-Img"..tostring(i), GUIType.StaticImage)
		local price_data = PartyConfig:getPriceDataById(i)
		if price_data then
 			cur_time_text:SetText(Lang:getString(price_data.TimeText))
		end
	 	cur_Img:registerEvent(GUIEvent.TouchUp, function()
	    	self:onClickPriceItem(i)
		end)
		self.selectItem[i] = select_Img
	end
	self:onClickPriceItem(self.selectIndex)
end

function UICreateParty:onShow()
	self.createPartyName:SetText(Lang:getString("please_enter_a_party_name"))
	self.createPartyDes:SetText(Lang:getString("please_enter_a_party_des"))
end

function UICreateParty:onClickButton(clickType)
	if clickType == UICreateParty.ClickType.CLOSE_PANEL then
		self:hide()
		return
	end

	if clickType == UICreateParty.ClickType.CREATE_PARTY then
		local price_data = PartyConfig:getPriceDataById(self.selectIndex)
		if price_data then
			if self.selectIndex == 1 and self.freeCreatePartyTimes < UICreateParty.FREE_PARTY_TIMES and self.freeCreatePartyTimes >= 0 then
				self:sendCreatePartyInfo()
			else
				GameConfig:checkMoney(MoneyType.Diamonds, price_data.Price, function(isCan)
					if isCan then
						self:sendCreatePartyInfo()
					end
				end)
			end
		end
		return
	end
end

function UICreateParty:onEditChange(editType)
	local cur_text = ""
	if editType == UICreateParty.ClickType.EDIT_NAME then
		cur_text = self.partyNameEdit:GetText()
		if cur_text ~= "" then
			self.createPartyName:SetText(cur_text)
		else
			self.createPartyName:SetText(Lang:getString("please_enter_a_party_name"))
		end
	end

	if editType == UICreateParty.ClickType.EDIT_DES then
		cur_text = self.partyDesEdit:GetText()
		if cur_text ~= "" then
			self.createPartyDes:SetText(cur_text)
		else
			self.createPartyDes:SetText(Lang:getString("please_enter_a_party_des"))
		end
	end
end

function UICreateParty:onClickPriceItem(index)
	print("onClickPriceItem  index == "..index)
	self.selectIndex = index
	local price_data = PartyConfig:getPriceDataById(index)
	if price_data then
		self.priceIcon:SetVisible(true)
		self.cur_select_price:SetText(tostring(price_data.Price))
		if index == 1 and self.freeCreatePartyTimes >= 0  and self.freeCreatePartyTimes < UICreateParty.FREE_PARTY_TIMES then
    		local cur_text = string.format(Lang:getString("gui_free_create_party_times"), tostring(UICreateParty.FREE_PARTY_TIMES - self.freeCreatePartyTimes))
			self.priceIcon:SetVisible(false)
			self.cur_select_price:SetText(cur_text)
		end
	end

	for k,v in pairs(self.selectItem) do
		v:SetVisible(false)
		if k == index then
			v:SetVisible(true)
		end
	end
end

function UICreateParty:onCreatePartyInfoUpdate(data)
	print("onCreatePartyInfoUpdate")
	if not self:isLoad() then
        return
    end
	self:upDateCreateParty()
end

function UICreateParty:upDateCreateParty()
	self.createPartyName:SetText("")
	self.createPartyDes:SetText("")
end

function UICreateParty:sendCreatePartyInfo()

	local partyName = self.partyNameEdit:GetText()
	local PartyDes = self.partyDesEdit:GetText()
    local msg = string.format(Lang:getString("create_party_success"), tostring(GameData.userName))

	if partyName == "" then
		partyName = string.format(Lang:getString("who_of_island"), tostring(GameData.userName)) 
	end 
	local data = {
		partyName = partyName,
		partyDesc = PartyDes,
		index = self.selectIndex,
		msgTips = msg,
		createType = UICreateParty.createType.NOT_FREE
	}

	if self.selectIndex == 1 and self.freeCreatePartyTimes < UICreateParty.FREE_PARTY_TIMES and self.freeCreatePartyTimes >= 0 then
		data.createType = UICreateParty.createType.FREE
	end

	GamePacketSender:sendCreateParty(json.encode(data))
	self:hide()
	-- return
	-- GUITipDialog:onShowTipDialog(Lang:getString("gui_input_can_not_be_empty"), GUITipDialog.TipType.COMMON)
end

function UICreateParty:setFreeCreatePartyTimes(num)
	self.freeCreatePartyTimes = tonumber(num)
	self:updateFreePanel()
end


function UICreateParty:updateFreePanel()
	if self.freeCreatePartyTimes >= 0 and self.freeCreatePartyTimes < UICreateParty.FREE_PARTY_TIMES then
		self.freeCreatePanel:SetVisible(true)
	else
		self.freeCreatePanel:SetVisible(false)
	end

	self:onClickPriceItem(self.selectIndex)
end

GUICreateParty = UICreateParty.new(Lang:getString("SkyBlockCreateParty.json"), GUIParty.root)
GUICreateParty:hide()