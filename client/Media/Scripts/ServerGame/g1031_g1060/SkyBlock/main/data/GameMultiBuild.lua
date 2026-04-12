GameMultiBuild = {}
GameMultiBuild.dataType = 
{
	FRIEND_APPLY_LIST = 1,
	FRIEND_LIST = 2,
	BUILD_FRIEND_LIST = 3,
	BUILD_FRIEND_APPLY_LIST = 4,
	SEND_BUILD_APPLY_LIST = 5,
}	

function GameMultiBuild:onPlayerLogin(player)
	---领主进入，恢复领地内共同建造者的建造权限
	local players = PlayerManager:getPlayers()
	for _, c_player in pairs(players) do
		if tostring(c_player.buildGroupId) == tostring(player.userId) then
			GamePacketSender:sendBuildGroupInfo(c_player)
		end
	end
end

function GameMultiBuild:onPlayerLogout(player)
	---领主退出，移出领地内共同建造者的建造权限
	local players = PlayerManager:getPlayers()
	for _, c_player in pairs(players) do
		if tostring(c_player.buildGroupId) == tostring(player.userId) then
			GamePacketSender:sendBuildGroupInfo(c_player)
		end
	end
end

function GameMultiBuild:upDataBuildGroupData(userId, isShowTips)
	local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
	player:logPlayerBuildGroup("upDataBuildGroupData")
	player.buildGroupId = tostring(player.userId)
	player.buildGroupData = {}
	player:bucketStateChange(false)
    SkyBlockWeb:getbuildGroup(userId, isShowTips)
	GamePacketSender:sendBuildGroupInfo(player)
end

function GameMultiBuild:onBuildGroupUserId(userId, isShowTips,  data)
	local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
	player.buildGroupId = data.masterId
	if not player.buildGroupId then
		return
	end
	SkyBlockWeb:getbuildMember(userId, player.buildGroupId, isShowTips)
	player:logPlayerBuildGroup("onBuildGroupUserId")
	GamePacketSender:sendBuildGroupInfo(player)
end

function GameMultiBuild:onBuildGroupData(userId, isShowTips, data)
	local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
	player.buildGroupData = data
	player:checkAllChangeDataToMember()
	GameMultiBuild:askMemberUpdateBuildData(player)
	GamePacketSender:sendMultiBuildGroupData(player.rakssid, json.encode(data))

	print("onBuildGroupData player.userId == "..tostring(player.userId).. "player.target_user_id == "..tostring(player.target_user_id).. " player.buildGroupId == "..tostring(player.buildGroupId))

	if isShowTips == Define.visitMasterTips.onShow and tostring(player.target_user_id) == tostring(player.buildGroupId) then
		if GameMultiBuild:checkIsMember(userId, player.buildGroupId) then
			local masterPlayer = PlayerManager:getPlayerByUserId(player.buildGroupId)
		    if not masterPlayer then
        		HostApi.sendCommonTip(player.rakssid, "master_now_is_leave")
		    end
		end
	end

	player:logPlayerBuildGroup("onBuildGroupData")
	player:bucketStateChange()
	GameMultiBuild:checkMemberBucketState(player)
end

function GameMultiBuild:checkIsMember(userId, masterId)
	local player = PlayerManager:getPlayerByUserId(userId)
	if not player then
	    return
	end

	if tostring(player.buildGroupId) == tostring(masterId) then
		for k, v in pairs(player.buildGroupData) do
			if tostring(v.masterId) == tostring(masterId) then
				if tostring(v.memberId) == tostring(userId) then
					return true
				end
			end
		end
	end
end

function GameMultiBuild:checkMasterQuitTips(player)
	if tostring(player.userId) ~= tostring(player.buildGroupId) then
		return
	end

	for k, v in pairs(player.buildGroupData) do
		if tostring(v.masterId) ~= tostring(v.memberId) then
			local memberPlayer = PlayerManager:getPlayerByUserId(v.memberId)
			if memberPlayer and tostring(memberPlayer.target_user_id) == tostring(v.masterId) then
        		HostApi.sendCommonTip(memberPlayer.rakssid, "master_is_leave")
			end
		end
	end
end

function GameMultiBuild:checkMemberBucketState(player)
	if tostring(player.userId) ~= tostring(player.buildGroupId) then
		return
	end

	for k, v in pairs(player.buildGroupData) do
		if tostring(v.masterId) ~= tostring(v.memberId) then
			local memberPlayer = PlayerManager:getPlayerByUserId(v.memberId)
			if memberPlayer and tostring(memberPlayer.target_user_id) == tostring(v.masterId) then
				memberPlayer:bucketStateChange()
			end
		end
	end
end

function GameMultiBuild:isShowMasterLevel(userId, masterId)
	if tostring(userId) == tostring(masterId) then
		return false
	end

	local memberPlayer = PlayerManager:getPlayerByUserId(userId)
	if memberPlayer == nil then
		return false
	end

	local masterIdPlayer = PlayerManager:getPlayerByUserId(masterId)
	if masterIdPlayer == nil then
		return false
	end

	if tostring(memberPlayer.target_user_id) == tostring(masterId) and
		tostring(memberPlayer.target_user_id) == tostring(memberPlayer.buildGroupId) then
		return true
	end

	return false
end

function GameMultiBuild:clickPlayerisShowExpel(userId, targetUserId)
	local player = PlayerManager:getPlayerByUserId(userId)
	if player == nil then
		return false
	end

	local tarPlayer = PlayerManager:getPlayerByUserId(targetUserId)
	if tarPlayer == nil then
		return false
	end

	if not GameMatch:isMaster(userId) then
		return false
	end

	if tostring(tarPlayer.target_user_id) ~= tostring(userId) then
		return false
	end

	return true
end

function GameMultiBuild:askMemberUpdateBuildData(player)
	for k, v in pairs(player.buildGroupData) do
		if tostring(player.userId) == tostring(v.masterId) and tostring(player.userId) ~= tostring(v.memberId) then
			local tarPlayer = PlayerManager:getPlayerByUserId(v.memberId)
			local data = {
		        targetUserId = tostring(v.memberId),
		        typeList = {GameMultiBuild.dataType.BUILD_FRIEND_LIST}
		    }
	        if tarPlayer then
	            GamePacketSender:sendMultiBuildUpdate(tarPlayer.rakssid, json.encode(data))
	        else
	            WebService:SendMsg({ tostring(v.memberId) }, json.encode(data), 104803, "user", "g1048")
	        end
		end
	end
end
