UIPartyItem = class("UIPartyItem", IGUIWindow)

function UIPartyItem:onLoad()
	print("UIPartyItem:onLoad()")
	self.data = {}
	self.index = 1

	self.selectImg = self:getChildWindow(0, GUIType.StaticImage)
	self.selectImg:SetVisible(false)

	self.playerName = self:getChildWindow(1)
	self.partyName = self:getChildWindow(2)
	self.playerNum = self:getChildWindow(3)
	self.LoveNum = self:getChildWindow(5)

	self.RankNum = self:getChildWindow(6):GetChildByIndex(0)
	self.icon = self:getChildWindow(7, GUIType.UrlImage)
    self.icon:setDefaultImage("set:userItem.json image:icon")
	self.root:registerEvent(GUIEvent.TouchUp, function()
    	self:onClickItem()
	end)
end

function UIPartyItem:onClickItem()
	GUIParty:onSelectParty(self.data.Index)
end

function UIPartyItem:setItemData(data, index)
	self.data = data
	self:setDataUpdate()
end

function UIPartyItem:setDataUpdate()
	self.playerName:SetText(self.data.userName)
	self.partyName:SetText(self.data.partyName)
	self.LoveNum:SetText(self.data.likeNum)
	local playerNumText = string.format("%d/%d", self.data.curPlayerNum, self.data.maxPlayerNum)
	self.playerNum:SetText(playerNumText)
	self.RankNum:SetText(tostring(self.data.Index))
	if self.data.picUrl then
    	self.icon:setUrl(self.data.picUrl)
	end
end

