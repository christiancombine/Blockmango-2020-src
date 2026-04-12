local UISkyBlockMiningAreaItemDetail = class("UISkyBlockMiningAreaItemDetail", IGUILayout)

function UISkyBlockMiningAreaItemDetail:onLoad()
	print("UISkyBlockMiningAreaItemDetail:onLoad()")

	self.stName = self:getChildWindow("SkyBlockMiningAreaItemDetail-Name")
	self.stDetail = self:getChildWindow("SkyBlockMiningAreaItemDetail-Desc")
	self.imgIcon = self:getChildWindow("SkyBlockMiningAreaItemDetail-Image", GUIType.StaticImage)
	self.llBg = self:getChildWindow("SkyBlockMiningAreaItemDetail", GUIType.Layout)
	self.llBg:registerEvent(GUIEvent.TouchUp, function()
    	self:onClosePanel()
	end)

	self:registerEvent(SumRechargeShowItemDetailEvent, function (reward)
        self:onRechargeShowItemDetail(reward)
    end)
    
	self.data = {}
end

function UISkyBlockMiningAreaItemDetail:onClosePanel()
	self:hide()
end

function UISkyBlockMiningAreaItemDetail:onSetData(data)
	self.data = data
	self:upDateItem()
end


function UISkyBlockMiningAreaItemDetail:upDateItem()
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

function UISkyBlockMiningAreaItemDetail:onRechargeShowItemDetail(reward)
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

GUISkyBlockMiningAreaItemDetail = UISkyBlockMiningAreaItemDetail.new("SkyBlockMiningAreaItemDetail.json")
GUISkyBlockMiningAreaItemDetail:hide()