
DataListener = {}

function DataListener:init()
    Listener.registerCallBack(SkyBlockOpenRankEvent, self.onOpenRankPanel)
    Listener.registerCallBack(UpdateUserListEvent, self.onUpdateUserList)
    Listener.registerCallBack(SellItemEvent, self.onSellItem)
    Listener.registerCallBack(DropItemEvent, self.onDropItem)
    Listener.registerCallBack(SkyBlockShowPriDetailEvent, self.onShowPriDetail)
    Listener.registerCallBack(SkyBlockShowLackMoney, self.onSkyBlockShowLackMoney)
end

function DataListener.onOpenRankPanel()
    GUIRank:show()
end

function DataListener.onUpdateUserList(type)
    -- type
    -- 1 addUser
    -- 2 removeUser
    -- 3 friend operator
    -- 4 user info change

    print("onUpdateUserList " .. type)
    local userIds = UserManager.Instance():getAllUser()
    MultiBuildData.NearPlayerData = {}
    for k, v in pairs(userIds) do
        local user = UserManager.Instance():findUser(v)
        if user then
            local data = {}
            data.isFriend = user.isFriend
            data.sex = user.sex
            data.userId = user.userId
            data.nickName = user.userName
            data.picUrl = user.picUrl
            table.insert(MultiBuildData.NearPlayerData, data)
        end
    end
    if GUIFriend and GUIFriend.nearPlayersBtn and GUIFriend.nearPlayersBtn:IsSelected() then
        GUIFriend:onUpdateUserNearFriend(MultiBuildData.NearPlayerData)
    end
end

function DataListener.onShowPriDetail(name, detail, icon)
    local data = GameConfig:getItemIconData()
    data.Name = name
    data.Detail = detail
    data.Icon = icon
    if GUISkyBlockItemDetail then
        GUISkyBlockItemDetail:onSetData(data)
        GUISkyBlockItemDetail:show()
    end
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
        if GUISellPreciousTip then
            GUISellPreciousTip:onShowPreciousTipDialog(itemId, itemMeta, type, callBack)
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
        if GUISellPreciousTip then
            GUISellPreciousTip:onShowPreciousTipDialog(itemId, itemMeta, type, callBack)
        end

        return false
    end
    return true
end

function DataListener.onSkyBlockShowLackMoney(isShow)
    if GUIBuyMoneyTips then
        GUIBuyMoneyTips:onShowLackMoney(isShow)
    end
end

return DataListener