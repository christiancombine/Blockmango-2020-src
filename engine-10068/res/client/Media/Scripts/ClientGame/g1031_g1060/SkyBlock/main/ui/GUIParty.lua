require "ui.GUIPartyItem"
local UIParty = class("UIParty", IGUILayout)

UIParty.ClickType =
{
	CLOSE_PANEL = 1,
	LEFT_PAGE = 2,
	RIGHT_PAGE = 3,
	MY_PARTY = 4,
	JOIN_PARTY = 5,
	GIVE_LOVE = 6,
}

function UIParty:onLoad()
	print("UIParty:onLoad()")
	self:initPartyVariable()
	self:onLoadInitUI()
end

function UIParty:initPartyVariable()
	self.partyData = {}
	self.myPartyData = {}
 	self.curPageIndex = 1
 	self.curTabIndex = 1
 	self.curSelectindex = 1
 	self.curMaxPage = 1
 	self.curParData = {}
 	--self.upDataTime = os.time()
 	self.curLastTime = 0
 	self.isGiveLoveToday = false
 	self.lastGiveLoveNum = 0
 	self.level = 0
 	self.haveParty = false



 	self:registerDataCallBack("PartyListData", function(data)
        self:onPartyInfoUpdate(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("playerGameData", function(data)
        self:onPlayerGameDataUpdate(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("PartyData", function(data)
        self:onCreatePartyInfoUpdate(data)
    end, JsonBuilderProcessor)

    self:registerDataCallBack("UpdateLoveNum", function(data)
        self:onLoveNumUpdate(data)
    end, JsonBuilderProcessor)
end

function UIParty:onLoadInitUI()
	self.closeBtn = self:getChildWindow("SkyBlockParty-BtnClose", GUIType.Button)
    self.closeBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickButton(UIParty.ClickType.CLOSE_PANEL)
	end)

	self.rightBtn = self:getChildWindow("SkyBlockParty-Right-Btn", GUIType.Button)
    self.rightBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickButton(UIParty.ClickType.RIGHT_PAGE)
	end)

	self.leftBtn = self:getChildWindow("SkyBlockParty-Left-Btn", GUIType.Button)
    self.leftBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickButton(UIParty.ClickType.LEFT_PAGE)
	end)

	self.createPartyBtn = self:getChildWindow("SkyBlockParty-My-Party", GUIType.Button)
	self.createPartyBtn:SetText(Lang:getString("gui_create_party"))
    self.createPartyBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickButton(UIParty.ClickType.MY_PARTY)
	end)

	self.joinPartyBtn = self:getChildWindow("SkyBlockParty-Join-Btn", GUIType.Button)
	self.joinPartyBtn:SetText(Lang:getString("gui_join_party"))
    self.joinPartyBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickButton(UIParty.ClickType.JOIN_PARTY)
	end)

	self.giveLoveBtn = self:getChildWindow("SkyBlockParty-Give-Love-Btn", GUIType.Button) 
	self.giveLoveBtn:registerEvent(GUIEvent.ButtonClick, function()
    	self:onClickButton(UIParty.ClickType.GIVE_LOVE)
	end)

	self.PartyName = self:getChildWindow("SkyBlockParty-Item-Name") 
	self.PartyLoveNum = self:getChildWindow("SkyBlockParty-Love-Num")
	self.PartyPlayerNum = self:getChildWindow("SkyBlockParty-Player-Num") 
	self.Partydes = self:getChildWindow("SkyBlockParty-Destext")
	self.PartyLastTime = self:getChildWindow("SkyBlockParty-Time-Text")
	self.CurPageText = self:getChildWindow("SkyBlockParty-Cur-Page")
	self.stComeOnTips = self:getChildWindow("SkyBlockParty-Come-On-Tips")
	self.stComeOnTips:SetText(Lang:getString("gui_party_come_on_tips"))


	self.curPlayerNumText= self:getChildWindow("SkyBlockParty-Player-Num-Text")
	self.curPlayerNumText:SetText(Lang:getString("gui_my_player_num"))
	self.curLastTimeText = self:getChildWindow("SkyBlockParty-Last-Time")
	self.curLastTimeText:SetText(Lang:getString("gui_party_last_time"))

	local title = self:getChildWindow("SkyBlockParty-TitleName")
	title:SetText(Lang:getString("gui_party_hall"))

    self.rightLayout = self:getChildWindow("SkyBlockParty-Detail-Panel", GUIType.Layout)
    self.partyLayout = self:getChildWindow("SkyBlockParty-Content-ItemList", GUIType.Layout)
 	self.partyGridView = IGUIGridView.new("skyblock-party-grid-view", self.partyLayout)
 	self.partyGridView:setConfig(10, 5, 2)

 	for i = 1, 4 do
 		local radioBtn = self:getChildWindow("SkyBlockParty-Tab_"..tostring(i), GUIType.RadioButton)
 		radioBtn:SetText(Lang:getString("Party_Tab_Name_"..tostring(i)))
 		radioBtn:registerEvent(GUIEvent.RadioStateChanged, function()
    		self:onRadioStateChanged(radioBtn, i)
		end)
		if i == 1 then
			radioBtn:SetSelected(true)
		end
 	end

	LuaTimer:schedule(function(userId)
		self:upTimeTextTick()
	end, 1000, 1000)
end

function UIParty:upTimeTextTick()
	if self.curLastTime > 0 then
		self.curLastTime = self.curLastTime - 1
		self:upTimeText()
	end

	self:timeToOverUpData()
	self:myPartyTimeToOverUpData()
end

function UIParty:upTimeText()
	local second = self.curLastTime % 60
	local minute = math.floor(self.curLastTime % 3600 / 60)
	local hour = math.floor(self.curLastTime / 3600)
	if second >= 0 or minute >= 0 or hour >= 0 then
		local time_text = string.format("%s:%s:%s",
			hour < 10 and string.format("0%d", hour) or tostring(hour),
			minute < 10 and string.format("0%d", minute) or tostring(minute),
			second < 10 and string.format("0%d", second) or tostring(second))
		self.PartyLastTime:SetText(time_text)
	end
end

function UIParty:myPartyTimeToOverUpData()
	if self.haveParty then
		local endTime = self.myPartyData.leftTime / 1000
		local curTime = endTime - os.time()
	    if curTime <= 0 then
	    	self:onCreatePartyInfoUpdate()
			GUIApplyParty:hide()
			GUICreateParty:hide()
	    end 
	end
end

function UIParty:timeToOverUpData()
 	local isRemove = false
	for k, partyList in pairs(self.partyData) do
    	for i, party in ipairs(partyList) do
			local endTime = party.leftTime / 1000
			local curTime = endTime - os.time()
		    if curTime <= 0 then
		    	table.remove(partyList, i)
		    	isRemove = true
		    	break
		    end
    	end
    end

    if isRemove then
		local index = 0 
		for k, partyList in pairs(self.partyData) do
			index = 0
	    	for i, party in ipairs(partyList) do
	    		index = index + 1
	    		local page = math.ceil(i / 6 )
	    		party.TabId = k
	    		party.PageId = page
	    		party.Index = index
	    	end
	    end
    	self:updataCurGridView()
    end

end

function UIParty:isMainLayout()
    return false
end

function UIParty:checkPartyRedPoint(oldList, newLList)
	local old_list = oldList or {}
	local new_list = newLList or {}
	for _, newParty in pairs(new_list) do
		for _, oldParty in pairs(old_list) do
			if newParty.partyId == oldParty.partyId then
				break
			end
		end

		return true
	end
	return false
end

function UIParty:onPartyInfoUpdate(curTable)
    if not self:isLoad() then
        return
    end
    print("UIParty:onPartyInfoUpdate")
    for k, v in pairs(curTable) do
    	if v.listType then
    		self.partyData[v.listType] = v.data
    	end
    end

    if #curTable > 0 then
		GUIToolBar:SetPartyRedPoint(true)
    end

    --self.upDataTime = os.time()
	local index = 0 
    for k, partyList in pairs(self.partyData) do
    	index = 0
    	for i, party in ipairs(partyList) do

    		index = index + 1
    		local page = math.ceil(i / 6 )
    		party.TabId = k
    		party.PageId = page
    		party.Index = index
    	end
    end

    self:updataCurGridView()
end

function UIParty:updataCurGridView(curSelectindex)
	local curData = self:getCurGirdViewDataByTabAndPage(self.curTabIndex, self.curPageIndex)
	-- if #curData <= 0 then
	-- 	return
	-- end
	print("self.curTabIndex == "..self.curTabIndex.."  self.curPageIndex == "..self.curPageIndex)
	self.CurPageText:SetText(tostring(self.curPageIndex))
    self.partyGridView:clearItems()
	for i, v in ipairs(curData) do
    	local curItem = UIPartyItem.new("SkyBlockPartyItem.json")
    	curItem:setItemData(v)
    	curItem:setWidth({ 0.49, 0 })
    	curItem:setHeight({ 0.325, 0 })
    	self.partyGridView:addItem(curItem)
	end

	if #curData > 0 then
		self.rightLayout:SetVisible(true)
		if curSelectindex then
   			self:onSelectParty(curSelectindex)
		else
   			self:onSelectParty(1)
		end
	else
		self.rightLayout:SetVisible(false)
	end
	self:setButtonStatus()
end

function UIParty:setButtonStatus()

	local curtabData = self:getCurGirdViewDataByTab(self.curTabIndex)
	self.curMaxPage = math.ceil(#curtabData / 6)

	self.leftBtn:SetVisible(true)
	self.rightBtn:SetVisible(true)
	if self.curPageIndex <= 1 then
		self.leftBtn:SetVisible(false)
	end

	if self.curPageIndex >= self.curMaxPage then
		self.rightBtn:SetVisible(false)
	end
end

function UIParty:onSelectParty(index)
	self.curSelectindex = index
	for _, item in pairs(self.partyGridView.items) do
        if item.data.Index == index then
        	self.curParData = item.data
            item.selectImg:SetVisible(true)
        else
            item.selectImg:SetVisible(false)
        end

    end

    self.PartyName:SetText(self.curParData.partyName)
	self.PartyLoveNum:SetText(self.curParData.likeNum)
	self.Partydes:SetText(self.curParData.partyDesc)

	local playerNumText = string.format("%d/%d", self.curParData.curPlayerNum, self.curParData.maxPlayerNum)
	self.PartyPlayerNum:SetText(playerNumText)

	local endTime = self.curParData.leftTime / 1000
	local curTime = endTime - os.time()

    if curTime <= 0 then
    	curTime = 0
    end

	self.curLastTime = curTime
	self:upTimeText()
end

function UIParty:getCurGirdViewDataByTabAndPage(tabId, pageId)
	local data = {}
	for k, partyList in pairs(self.partyData) do
		if k == tabId then
			for _, party in pairs(partyList) do
				if party.PageId == pageId then
					table.insert(data, party)
				end
			end
		end
	end

	return data
end

function UIParty:getCurGirdViewDataByTab(tabId)
	local data = {}
	for k, partyList in pairs(self.partyData) do
		if k == tabId then
			for _, party in pairs(partyList) do
				table.insert(data, party)
			end
		end
	end

	return data
end

function UIParty:onRadioStateChanged(radioBtn, index)
	print("UIParty:onRadioStateChanged")
	radioBtn:SetWidth({1, 0})
	if radioBtn:IsSelected() then
		radioBtn:SetWidth({1.2, 0})
		self.curTabIndex = index
		self.curPageIndex = 1
		self:updataCurGridView()
	end
end

function UIParty:onClickButton(clickType)
	if clickType == UIParty.ClickType.CLOSE_PANEL then
		self:hide()
		return
	end

	if clickType == UIParty.ClickType.LEFT_PAGE then
		self.curPageIndex = self.curPageIndex - 1
		if self.curPageIndex <= 1 then
			self.curPageIndex = 1
		end
		self:updataCurGridView()
		return
	end

	if clickType == UIParty.ClickType.RIGHT_PAGE then
		self.curPageIndex = self.curPageIndex + 1
		if self.curPageIndex >= self.curMaxPage then
			self.curPageIndex = self.curMaxPage
		end
		self:updataCurGridView()
		return
	end

	if clickType == UIParty.ClickType.MY_PARTY then
		if self.haveParty then
			GUIApplyParty:show()
		else
			GUICreateParty:show()
		end
		
		return
	end

	if clickType == UIParty.ClickType.JOIN_PARTY then
    	PacketSender:sendCallOnManor(self.curParData.userId, 1000 + self.curTabIndex, true, 0)
		-- GamePacketSender:sendEnterParty(self.curParData.userId)
		return
	end

	if clickType == UIParty.ClickType.GIVE_LOVE then
		if self.lastGiveLoveNum <= 0 then
			GUITipDialog:onShowTipDialog(Lang:getString("gui_give_love_is_max"), GUITipDialog.TipType.COMMON)
			return
		end

		if self.level < GameConfig.giveLoveNumLevel then
			GUITipDialog:onShowTipDialog(Lang:getString("gui_level_too_low"), GUITipDialog.TipType.COMMON)
			return
		end

		GamePacketSender:sendGiveLove(self.curParData.userId)
		return
	end
end

function UIParty:onPlayerGameDataUpdate(curTable)
    if not self:isLoad() then
        return
    end

    print("UIParty:onPlayerGameDataUpdate")
    -- local curTable = json.decode(data)
    if curTable == nil then
    	return
	end
    self.lastGiveLoveNum = curTable.giveLoveLastNum
 	self.level = curTable.level
 	GUICreateParty:setFreeCreatePartyTimes(curTable.freeCreatePartyTimes)
 	GUIToolBar:setLoveNum(curTable.loveNum, curTable.loveNumType)
end

function UIParty:onCreatePartyInfoUpdate(curTable)
	print("UIParty onCreatePartyInfoUpdate")
	if not self:isLoad() then
        return
    end

    -- local curTable = json.decode(data)
    if curTable then
    	self.haveParty = true
		self.createPartyBtn:SetText(Lang:getString("gui_my_party"))
    	self.myPartyData = curTable
    else
    	self.haveParty = false
		self.createPartyBtn:SetText(Lang:getString("gui_create_party"))
    	self.myPartyData = {}
		print("UIParty onCreatePartyInfoUpdate 222")
    end

	GUIApplyParty:onCreatePartyInfoUpdate(curTable)
	GUICreateParty:onCreatePartyInfoUpdate(curTable)
	self:onMyPartyUpdate(curTable)
end

function UIParty:onMyPartyUpdate(curTable)
    print("onMyPartyUpdate")
    if curTable then
    	local myPartyName = curTable.partyName or ""
    	local myPartyDesc = curTable.partyDesc or ""

	   	for k, partyList in pairs(self.partyData) do
	    	for i, party in ipairs(partyList) do
	    		if tostring(party.userId) == tostring(GameData.userId) then
	    			party.partyName = myPartyName
	    			party.partyDesc = myPartyDesc
	    		end
	    	end
	    end
	   self:updataCurGridView(self.curSelectindex)
    end
end

function UIParty:onLoveNumUpdate(curTable)
	if not self:isLoad() then
        return
    end
    if curTable then
    	local targetUserId = curTable.targetUserId
    	local loveNum = curTable.loveNum

	   	for k, partyList in pairs(self.partyData) do
	    	for i, party in ipairs(partyList) do
	    		if tostring(party.userId) == tostring(targetUserId) then
	    			party.likeNum = loveNum
	    		end
	    	end
	    end
	   self:updataCurGridView(self.curSelectindex)
    end
end

GUIParty = UIParty.new("SkyBlockParty.json")
GUIParty:hide()