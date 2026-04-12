UISkyBlockItemIcon = class("UISkyBlockItemIcon", IGUIWindow)

function UISkyBlockItemIcon:onLoad()
	self.data = {}
	self.bgImg = self:getChildWindow(0, GUIType.StaticImage)
	self.IconImg = self:getChildWindow(1, GUIType.StaticImage)
	self.stNum = self:getChildWindow(2)
	self.stNum:SetVisible(false)

	self.bgImg:registerEvent(GUIEvent.TouchUp, function()
    	self:onClickItems()
	end)
end

function UISkyBlockItemIcon:onClickItems()
	GUISkyBlockItemDetail:onSetData(self.data)
	GUISkyBlockItemDetail:show()
end

function UISkyBlockItemIcon:setItemData(data)
	self.data = data
	self:upDateItem()
end

function UISkyBlockItemIcon:upDateItem()
	local textLeng = string.len(self.data.Icon)
	if textLeng > 2 then
		GuiItemStack.showItemIcon(self.IconImg, self.data.ItemId, self.data.Meta, self.data.Icon)
	else
		GuiItemStack.showItemIcon(self.IconImg, self.data.ItemId, self.data.Meta, "")
	end
	local str = "x"..tostring(self.data.Num)
	self.stNum:SetText(str)

	if self.data.DataType == Define.ItemIcon.Common then
		self.stNum:SetVisible(false)
	end

	if self.data.DataType == Define.ItemIcon.Gift_reward then
		self.stNum:SetVisible(true)
	end
end