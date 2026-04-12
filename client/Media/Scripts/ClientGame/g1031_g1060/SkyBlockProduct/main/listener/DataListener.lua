DataListener = {}
function DataListener:init()
	Listener.registerCallBack(SkyBlockShowPriDetailEvent, self.onShowPriDetail)
	Listener.registerCallBack(SellItemEvent, self.onSellItem)
	Listener.registerCallBack(SkyBlockShowLackMoney, self.onSkyBlockShowLackMoney)
end

function DataListener.onShowPriDetail(name, detail, icon)
    local data = GameConfig:getItemIconData()
    data.Name = name
    data.Detail = detail
    data.Icon = icon
	GUISkyBlockProductItemDetail:onSetData(data)
	GUISkyBlockProductItemDetail:show()
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
		if GUISkyBlockProductSellPreciousTip then
			GUISkyBlockProductSellPreciousTip:onShowPreciousTipDialog(itemId, itemMeta, type, callBack)
		end
		return false
	end
	return true
end

function DataListener.onSkyBlockShowLackMoney(isShow)
    GUISkyBlockProductBuyMoneyTips:onShowLackMoney(isShow)
end

return DataListener