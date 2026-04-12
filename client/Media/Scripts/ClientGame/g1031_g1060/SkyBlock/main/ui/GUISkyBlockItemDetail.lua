local UISkyBlockItemDetail = class("UISkyBlockItemDetail", IGUILayout)

function UISkyBlockItemDetail:onLoad()
	print("UISkyBlockItemDetail:onLoad()")

	self.stName = self:getChildWindow("SkyBlockItemDetail-Name")
	self.stDetail = self:getChildWindow("SkyBlockItemDetail-Desc")
	self.imgIcon = self:getChildWindow("SkyBlockItemDetail-Image", GUIType.StaticImage)
	self.llBg = self:getChildWindow("SkyBlockItemDetail", GUIType.Layout)
	self.llBg:registerEvent(GUIEvent.TouchUp, function()
    	self:onClosePanel()
	end)
	self.data = {}

	self:registerEvent(SumRechargeShowItemDetailEvent, function (reward)
        self:onRechargeShowItemDetail(reward)
    end)
end

function UISkyBlockItemDetail:onClosePanel()
	self:hide()
end

function UISkyBlockItemDetail:onSetData(data)
	self.data = data
	self:upDateItem()
end


function UISkyBlockItemDetail:upDateItem()
	self.stName:SetText(Lang:getString(self.data.Name))
	self.stDetail:SetText(Lang:getString(self.data.Detail))

	-- if self.data.DataType == Define.ItemIcon.Common then
	local leng = string.len(self.data.Icon)
	print("leng == "..leng)
	if string.len(self.data.Icon) > 2 then
		GuiItemStack.showItemIcon(self.imgIcon, self.data.ItemId, self.data.Meta, self.data.Icon)
	else
		GuiItemStack.showItemIcon(self.imgIcon, self.data.ItemId, self.data.Meta, "")
	end
	return
	-- end
end

function UISkyBlockItemDetail:onRechargeShowItemDetail(reward)
	local data = GameConfig:getItemIconData()
    data.ItemId = reward.RewardId
    data.Meta = reward.Meta
    data.Num = reward.Count
    data.Name = reward.NameLang
    data.Detail = reward.DescLang
    data.Icon = reward.Icon

    self:onSetData(data)
    self:show()
end

GUISkyBlockItemDetail = UISkyBlockItemDetail.new("SkyBlockItemDetail.json")
GUISkyBlockItemDetail:hide()