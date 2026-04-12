--require "data.GameData"
MultiBuildData = {}

MultiBuildData.MyFriendData = {}
MultiBuildData.FriendApplyData = {}
MultiBuildData.NearPlayerData = {}
MultiBuildData.TogetherBuildData = {}
MultiBuildData.BuildApplyData = {}
MultiBuildData.BuildSendApplyData = {}
MultiBuildData.getIsPlayGameList = {}

MultiBuildData.haveAddNearPlayer = {}

MultiBuildData.dataType = 
{
	FRIEND_APPLY_LIST = 1,
	FRIEND_LIST = 2,
	BUILD_FRIEND_LIST = 3,
	BUILD_FRIEND_APPLY_LIST = 4,
	SEND_BUILD_APPLY_LIST = 5,
}	

function MultiBuildData:getAllNeedList(isCheckRedPoint)
	self:getFriendList()
	self:getFriendRequestList(isCheckRedPoint)
	self:getInvitationList(isCheckRedPoint)
	self:getSendInvitationList()
	MultiBuildData:sendGetTogetherBuildData()
end

---获取邀请我共同建造的申请列表
function MultiBuildData:getInvitationList(isCheckRedPoint)
	print("getInvitationList") 
	SkyBlockWeb:getInvitationList(GameData.userId, function (info, code)
		print("getInvitationList back code == "..code)
		if code == 1 then
			MultiBuildData.BuildApplyData = {}
			if info then
				MultiBuildData.BuildApplyData = info
			end
			GUIFriend:updateCurGridView()
			if isCheckRedPoint then
				GUIFriend:refreshBuildApplyRed()
			end
		end
	end)
end

---获取我发出共同建造的邀请列表
function MultiBuildData:getSendInvitationList()
	print("getSendInvitationList req")
	SkyBlockWeb:getSendInvitationList(GameData.userId, function (info, code)
		print("getSendInvitationList back code == "..code)
		if code == 1 then
			if info then
				MultiBuildData.BuildSendApplyData = info
			end
    		GUIFriendInvitation:onUpdateUserFriendInvitation()
		end
	end)
end

---获取玩家有没有玩过某个游戏
function MultiBuildData:getIsPlayGame(userId, userIdList)
	print("getIsPlayGame req")
	SkyBlockWeb:getIsPlayGame(userId, userIdList, function (info, code)
		print("getIsPlayGame back code == "..code)
		if code == 1 then
            if info then
                for id, isPlay in pairs(info) do
            	    for k, cur_player in pairs(MultiBuildData.MyFriendData) do
            	    	if tostring(id) == tostring(cur_player.userId) then
            	    		cur_player.isPlayCurGame = isPlay
            	    	end
            		end
                end
				GUIFriend:updateCurGridView()
            end
        end
	end) 
end

---获取好友列表
function MultiBuildData:getFriendList()
	print("getFriendList req")
	SkyBlockWeb:getFriendList(function (info, code)
		print("getFriendList back code == "..code)
		if code == 1 then
			MultiBuildData.MyFriendData = {}
			if info then
				MultiBuildData.MyFriendData = info.data or {}
				for _, friend in pairs(MultiBuildData.MyFriendData) do
					friend.isPlayCurGame = 0
					friend.onLineStatus = 0
					if friend.status == 20 then
				        if friend.gameId == BaseMain:getGameType() then
				        	friend.onLineStatus = 1 
				        end
    				end
				end
			end

			local userIdList = ""
			for k, v in pairs(info.data) do
				if userIdList == "" then
					userIdList = userIdList..tostring(v.userId)
				else
					userIdList = userIdList ..","..tostring(v.userId)
				end
			end

			MultiBuildData:getIsPlayGame(GameData.userId, userIdList)
			-- GUIFriend:updateCurGridView()
		end
	end)
end

---获取好友申请列表
function MultiBuildData:getFriendRequestList(isCheckRedPoint)
	print("getFriendRequestList req") 
	SkyBlockWeb:getFriendRequestList(function (info, code)
		print("getFriendRequestList back code == "..code)
		if code == 1 then
			MultiBuildData.FriendApplyData = {}
			if info then
				for k, v in pairs(info.data) do
					if v.status == 0 then
						table.insert(MultiBuildData.FriendApplyData, v)
					end
				end
			end

			GUIFriend:updateCurGridView()
			if isCheckRedPoint then
				GUIFriend:refreshFrientApplyRed()
			end
		end
	end)
end

---接受共同建造邀请
function MultiBuildData:postAgreeInvite(userId, invitationId, targetUserId)
	print("postAgreeInvite")
	SkyBlockWeb:postAgreeInvite(userId, invitationId, function (info, code)
		print("postAgreeInvite back code == "..code.."  targetUserId == "..tostring(targetUserId))
        if code == 1 then
			MultiBuildData:sendGetTogetherBuildData()

			local data  = {MultiBuildData.dataType.BUILD_FRIEND_LIST, MultiBuildData.dataType.SEND_BUILD_APPLY_LIST, MultiBuildData.dataType.FRIEND_LIST}

			local master = self:getBuildMaster()
			if master then
				GamePacketSender:sendOtherPlayerUpdateData(master.masterId, data)
			end

			GamePacketSender:sendOtherPlayerUpdateData(targetUserId, data)
    		PacketSender:sendCallOnManor(targetUserId, 2000, true, 0)

		elseif code == 2032 then
			GUITipDialog:onShowTipDialog(Lang:getString("other_build_group_is_max"), GUITipDialog.TipType.COMMON)
		elseif code == 2033 then
			GUITipDialog:onShowTipDialog(Lang:getString("you_build_group_is_max"), GUITipDialog.TipType.COMMON)
		elseif code == 2038 then
			GUITipDialog:onShowTipDialog(Lang:getString("this_apply_is_fail"), GUITipDialog.TipType.COMMON)
		elseif code == 3002 then
			GUITipDialog:onShowTipDialog(Lang:getString("my_firend_list_full"), GUITipDialog.TipType.COMMON)
		elseif code == 3005 then
			GUITipDialog:onShowTipDialog(Lang:getString("he_firend_list_full"), GUITipDialog.TipType.COMMON)
         end

		MultiBuildData:getInvitationList(false)
		MultiBuildData:getFriendList()
    end)
end

---拒绝共同建造邀请
function MultiBuildData:postRefuseInvite(userId, invitationId, targetUserId)
	print("postRefuseInvite req") 
	SkyBlockWeb:postRefuseInvite(userId, invitationId, function (info, code)
		print("postRefuseInvite back code == "..code.."  targetUserId == "..tostring(targetUserId))
        if code == 1 then
			local data  = {MultiBuildData.dataType.SEND_BUILD_APPLY_LIST}
			GamePacketSender:sendOtherPlayerUpdateData(targetUserId, data)
        end

		MultiBuildData:getInvitationList(false)
    end)
end

---踢出玩家共同建造权限
function MultiBuildData:postKickOut(userId, kickedId, kickType, isBackHome)
	print("postKickOut") 
	SkyBlockWeb:postKickOut(userId, kickedId, kickType, function (info, code)
		print("postKickOut back code == "..code.." masterId == "..tostring(userId).." kickedId == "..tostring(kickedId)
				.." kickType == "..kickType)
		if code == 1 then
			local data  = {MultiBuildData.dataType.BUILD_FRIEND_LIST}
			for k, v in pairs(MultiBuildData.TogetherBuildData) do
				GamePacketSender:sendOtherPlayerUpdateData(v.memberId, data)
			end

			if isBackHome then
			    local msg = string.format(Lang:getString("other_expel_you"), tostring(GameData.userName))
				GamePacketSender:kickOutPlayer(kickedId, msg)
			elseif kickType == SkyBlockWeb.KickOutType.MASTER_KICK_OUT then
				local tips = "master_kickout_you"
				GamePacketSender:sendAskTargetShowTip(kickedId, tips)
			end
		end
	end) 
end

---邀请玩家共同建造
function MultiBuildData:postInvitationOther(userId, inviteeId)
	print("postInvitationOther") 
	SkyBlockWeb:postInvitationOther(userId, inviteeId, function (info, code)
		print("postRefuseInvite back code == "..code.. "  inviteeId == "..tostring(inviteeId))
		if code == 1 then
			MultiBuildData:getSendInvitationList()
			local data  = {MultiBuildData.dataType.BUILD_FRIEND_APPLY_LIST}
			GamePacketSender:sendOtherPlayerUpdateData(inviteeId, data)
	        GUICenterTips:setTextandTime(Lang:getString("gui_player_list_item_add_friend_msg_sent"))
		elseif code == 2032 then
			GUITipDialog:onShowTipDialog(Lang:getString("other_build_group_is_max"), GUITipDialog.TipType.COMMON)
		elseif code == 2033 then
			GUITipDialog:onShowTipDialog(Lang:getString("target_have_build_group"), GUITipDialog.TipType.COMMON)
		elseif code == 3002 then
			GUITipDialog:onShowTipDialog(Lang:getString("my_firend_list_full"), GUITipDialog.TipType.COMMON)
		elseif code == 3005 then
			GUITipDialog:onShowTipDialog(Lang:getString("he_firend_list_full"), GUITipDialog.TipType.COMMON)
		end
	end) 
end

---同意好友申请
function MultiBuildData:postAgreementFriend(targetUserId)
	print("postAgreementFriend") 
	SkyBlockWeb:postAgreementFriend(targetUserId, function (info, code)
		print("postAgreementFriend back code == "..code.." targetUserId == "..tostring(targetUserId))
        if code == 1 then
        	MultiBuildData:getFriendList()
			local data  = {MultiBuildData.dataType.FRIEND_LIST}
			GamePacketSender:sendOtherPlayerUpdateData(targetUserId, data)        
        end

        MultiBuildData:getFriendRequestList(false)
    end)
end

---拒绝好友申请
function MultiBuildData:postRejectionFriend(targetUserId)
	print("postRejectionFriend") 
	SkyBlockWeb:postRejectionFriend(targetUserId, function (info, code)
		print("postRejectionFriend back code == "..code.." targetUserId == "..tostring(targetUserId))
        MultiBuildData:getFriendRequestList(false)
    end)
end

---删除好友
function MultiBuildData:postDeleteFriend(targetUserId)
	print("postDeleteFriend") 
	SkyBlockWeb:postDeleteFriend(targetUserId, function (info, code)
		print("postDeleteFriend back code == "..code.." targetUserId == "..tostring(targetUserId))
        if code == 1 then
        	MultiBuildData:getFriendList()
			local data  = {MultiBuildData.dataType.FRIEND_LIST}
			GamePacketSender:sendOtherPlayerUpdateData(targetUserId, data)    
        end
    end)
end

---添加好友
function MultiBuildData:postAddFriend(targetUserId)  
	print("postAddFriend") 
	SkyBlockWeb:postAddFriend(targetUserId, function (info, code)
		print("postAddFriend back code == "..code.."  targetUserId == "..tostring(targetUserId))
        if code == 1 then
			local data  = {MultiBuildData.dataType.FRIEND_APPLY_LIST}
			GamePacketSender:sendOtherPlayerUpdateData(targetUserId, data)
		elseif code == 3001 then
			GUICenterTips:setTextandTime(Lang:getString("is_friend_already"))
		elseif code == 3002 then
			GUICenterTips:setTextandTime(Lang:getString("exceed_max_friend_number"))
        end
    end)
end

---获取建造权限数据
function MultiBuildData:sendGetTogetherBuildData()
	print("sendGetTogetherBuildData")
	GamePacketSender:sendGetTogetherBuildData()
end

---收到建造权限数据
function MultiBuildData:UpdateTogetherBuildData(data)
	MultiBuildData.TogetherBuildData = data or {}
	GUIFriend:updateCurGridView()
end

-------
function MultiBuildData:checkIsBuildMember(userId)
	for k, v in pairs(MultiBuildData.TogetherBuildData) do
		if tostring(userId) == tostring(v.masterId) or tostring(userId) == tostring(v.memberId)then
			return true
		end
	end

	return false
end

function MultiBuildData:getBuildMaster()
	for k, v in pairs(MultiBuildData.TogetherBuildData) do
		if tostring(v.masterId) == tostring(v.memberId) then
			return v
		end
	end
	return nil
end

function MultiBuildData:checkIsBuildMaster(userId)
	for k, v in pairs(MultiBuildData.TogetherBuildData) do
		if tostring(userId) == tostring(v.masterId)then
			return true
		end
	end

	return false
end

function MultiBuildData:getMemberDataByIndex(index)
	local cur_id = 2
	for k, v in ipairs(MultiBuildData.TogetherBuildData) do
		if index == 1 then
			if tostring(v.memberId) == tostring(v.masterId) then
				return v
			end
		elseif tostring(v.memberId) ~= tostring(v.masterId) then
			if cur_id == index then
				return v
			end
			cur_id = cur_id + 1
		end
	end

	return nil
end

function MultiBuildData.checkIsSendApply(userId)
	for k, v in pairs(MultiBuildData.BuildSendApplyData) do
		if tostring(v.userId) == tostring(userId) then
			return true
		end
	end

	return false
end

function MultiBuildData:checkMeIsIslandMaster()
	for k, v in pairs(MultiBuildData.TogetherBuildData) do
		if tostring(v.masterId) == tostring(GameData.userId)  then
			return true
		end
	end
	return false
end

function MultiBuildData:UpdateMulyiBuildData(data)
	for k, v in pairs(data.typeList) do
		if v == MultiBuildData.dataType.FRIEND_APPLY_LIST then
			MultiBuildData:getFriendRequestList(true)
		end

		if v == MultiBuildData.dataType.FRIEND_LIST then
			MultiBuildData:getFriendList()
		end

		if v == MultiBuildData.dataType.BUILD_FRIEND_LIST then
			MultiBuildData:sendGetTogetherBuildData()
		end

		if v == MultiBuildData.dataType.BUILD_FRIEND_APPLY_LIST then
			MultiBuildData:getInvitationList(true)
		end

		if v == MultiBuildData.dataType.SEND_BUILD_APPLY_LIST then
			MultiBuildData:getSendInvitationList()
		end
	end
end


function MultiBuildData:checkCanAddFriend(userId)
	for k, v in pairs(MultiBuildData.haveAddNearPlayer) do
		if tostring(v) == tostring(userId) then
			return false
		end
	end

	return true
end

function MultiBuildData:addNearFriend(userId)
	for k, v in pairs(MultiBuildData.haveAddNearPlayer) do
		if tostring(v) == tostring(userId) then
			return
		end
	end

	table.insert(MultiBuildData.haveAddNearPlayer, tostring(userId))
end


function MultiBuildData:RemoveNearFriend(userId)
	for k, v in pairs(MultiBuildData.haveAddNearPlayer) do
		if tostring(v) == tostring(userId) then
			table.remove(MultiBuildData.haveAddNearPlayer, k)
			return
		end
	end
end

function MultiBuildData:checkIsMyFriend(userId)
	for k, v in pairs(MultiBuildData.MyFriendData) do
		if tostring(v.userId) == tostring(userId) then
			return true
		end
	end
	return false
end

function MultiBuildData:checkCanAddBuildFriend(userId)
	for k, v in pairs(MultiBuildData.TogetherBuildData) do
		if tostring(v.memberId) == tostring(userId) then
			return false
		end
	end

	return true
end

return MultiBuildData