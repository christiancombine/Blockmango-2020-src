DataListener = {}
function DataListener:init()
	Listener.registerCallBack(SkyBlockShowPriDetailEvent, self.onShowPriDetail)
	Listener.registerCallBack(SellItemEvent, self.onSellItem)
	Listener.registerCallBack(DropItemEvent, self.onDropItem)
	Listener.registerCallBack(SkyBlockShowLackMoney, self.onSkyBlockShowLackMoney)
end

function DataListener.onShowPriDetail(name, detail, icon)
    local data = GameConfig:getItemIconData()
    data.Name = name
    data.Detail = detail
    data.Icon = icon
	GUISkyBlockMiningAreaItemDetail:onSetData(data)
	GUISkyBlockMiningAreaItemDetail:show()
end

function DataListener.onSellItem(itemId, itemMeta)
	if itemId == 2465 then
		return false
	end

	if SellPreciousTipConfig:checkIfCanPrecious(itemId, itemMeta) then
		local callBack = function(sure)
			if sure then
				RootGuiLayout.Instance():armorPanelSellItem()
			end
		end
		local type = 1
		if GUISkyBlockMiningAreaSellPreciousTip then
			GUISkyBlockMiningAreaSellPreciousTip:onShowPreciousTipDialog(itemId, itemMeta, type, callBack)
		end
		return false
	end
	return true
end

function DataListener.onDropItem(itemId, index, itemMeta)
	if SellPreciousTipConfig:checkIfCanPrecious(itemId, itemMeta) then
		local callBack = function(sure)
			if sure then
				RootGuiLayout.Instance():againDropItem(index)
			end
		end
		local type = 2
		if GUISkyBlockMiningAreaSellPreciousTip then
			GUISkyBlockMiningAreaSellPreciousTip:onShowPreciousTipDialog(itemId, itemMeta, type, callBack)
		end

		return false
	end
	return true
end

function DataListener.onSkyBlockShowLackMoney(isShow)
    GUIBuyMoneyTips:onShowLackMoney(isShow)
end

return DataListener