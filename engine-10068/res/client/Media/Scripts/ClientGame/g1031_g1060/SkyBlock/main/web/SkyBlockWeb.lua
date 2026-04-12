SkyBlockWeb = {}

---buildGet
SkyBlockWeb.invitationListApi = "/gameaide/api/v1/game/build/invitation"
SkyBlockWeb.sendInvitationListApi = "/gameaide/api/v1/game/build/send/invitation"
SkyBlockWeb.isPlayGameApi = "/gameaide/api/v1/game/whether/play"

---friendGet
SkyBlockWeb.getFriendListApi = "/friend/api/v1/friends"
SkyBlockWeb.getFriendRequestApi = "/friend/api/v2/friends/requests"

---buildPost 
SkyBlockWeb.dealWithInviteApi = "/gameaide/api/v1/game/build/invitation/handle"
SkyBlockWeb.kickOutApi = "/gameaide/api/v1/game/build/leave"
SkyBlockWeb.invitationOtherApi = "/gameaide/api/v1/game/build/invitation"

---friendPost
SkyBlockWeb.agreeFriendApi = "/friend/api/v1/friends/{otherId}/agreement"
SkyBlockWeb.rejectionFriendApi = "/friend/api/v1/friends/{otherId}/rejection"
SkyBlockWeb.deleteFriendApi = "/friend/api/v1/friends"
SkyBlockWeb.addFriendApi = "/friend/api/v1/friends"

SkyBlockWeb.BuildFriend = 
{
    AGREE = 1,
    REFUSE = 2,
}

SkyBlockWeb.FriendOperation =
{
    AGREE = 1,
    REFUSE = 2,
    DELETE_FRIENT = 3,
    ADD_FRIENT = 4
} 

SkyBlockWeb.KickOutType =
{
    SELF_LEAVE = 0,
    MASTER_KICK_OUT = 1
} 

---获取邀请我共同建造的申请列表
function SkyBlockWeb:getInvitationList(userId, callback)
	--print("getInvitationList")
    local params = {
        { "gameId", Game.GameType },
        { "userId", tostring(userId) }
    }

    WebService.asyncGet(SkyBlockWeb.invitationListApi, params, callback)
end

---获取我发出共同建造的邀请列表
function SkyBlockWeb:getSendInvitationList(userId, callback)   
    --print("getMyBuildGroup")
    local params = {
        { "gameId", Game.GameType },
        { "userId", tostring(userId) }
    }

    WebService.asyncGet(SkyBlockWeb.sendInvitationListApi, params, callback)
end

---获取玩家有没有玩过某个游戏
function SkyBlockWeb:getIsPlayGame(userId, userIdList, callback)  
	--print("getIsPlayGame ")
    local params = {
        { "userId", tostring(userId) },
        { "gameId", Game.GameType },
        { "userIdList", userIdList }
    }

    WebService.asyncGet(SkyBlockWeb.isPlayGameApi, params, callback)
end

---获取好友列表
function SkyBlockWeb:getFriendList(callback)
    --print("getFriendList")
    local params = {
        { "pageNo","0" },
        { "pageSize","50" }
    }

    WebService.asyncGet(SkyBlockWeb.getFriendListApi, params, callback)
end

---获取好友申请列表
function SkyBlockWeb:getFriendRequestList(callback)
    --print("getFriendRequestList")
    local params = {
        { "pageNo","0"},
        { "pageSize","50" }
    }

    WebService.asyncGet(SkyBlockWeb.getFriendRequestApi, params, callback)
end

---接受共同建造邀请
function SkyBlockWeb:postAgreeInvite(userId, invitationId, callback)   
	--print("postDealWithInvite")
    local body = {}
    local params = {
        { "gameId", Game.GameType },
        { "userId", tostring(userId) },
        { "invitationId", tostring(invitationId) },
        { "status", tostring(SkyBlockWeb.BuildFriend.AGREE) }
    }

    WebService.asyncPost(SkyBlockWeb.dealWithInviteApi, params, json.encode(body), callback)
end

---拒绝共同建造邀请
function SkyBlockWeb:postRefuseInvite(userId, invitationId, callback)   
	--print("postDealWithInvite")
    local body = {}
    local params = {
        { "gameId", Game.GameType },
        { "userId", tostring(userId) },
        { "invitationId", tostring(invitationId) },
        { "status", tostring(SkyBlockWeb.BuildFriend.REFUSE) }
    }

    WebService.asyncPost(SkyBlockWeb.dealWithInviteApi, params, json.encode(body), callback)
end

---踢出玩家共同建造权限
function SkyBlockWeb:postKickOut(masterId, kickedId, kickType, callback)   
	print("SkyBlockWeb postKickOut")
    local body = {}
    local params = {
        { "gameId", Game.GameType },
        { "userId", tostring(Game.userId) },
        { "masterId", tostring(masterId) },
        { "memberId", tostring(kickedId) },
        { "type", tostring(kickType) }
    }

    WebService.asyncPost(SkyBlockWeb.kickOutApi, params, json.encode(body), callback)
end

---邀请玩家共同建造
function SkyBlockWeb:postInvitationOther(userId, inviteeId, callback)   
	--print("postInvitationOther")
    local body = {}
    local params = {
        { "gameId", Game.GameType},
        { "userId", tostring(userId) },
        { "inviteeId", tostring(inviteeId) }
    }

    WebService.asyncPost(SkyBlockWeb.invitationOtherApi, params, json.encode(body), callback)
end

---同意好友申请
function SkyBlockWeb:postAgreementFriend(targetUserId, callback)   
    --print("postInvitationOther")
    local path = SkyBlockWeb.agreeFriendApi
    path = string.gsub(path, "{otherId}", tostring(targetUserId))

    local body = {}
    local params = {}

    WebService.asyncPut(path, params, json.encode(body), callback)
end

---拒绝好友申请
function SkyBlockWeb:postRejectionFriend(targetUserId, callback)   
    -- print("postRejectionFriend") 
    local path = SkyBlockWeb.rejectionFriendApi
    path = string.gsub(path, "{otherId}", tostring(targetUserId))
    
    local body = {}
    local params = {}

    WebService.asyncPut(path, params, json.encode(body), callback)
end

---删除好友
function SkyBlockWeb:postDeleteFriend(targetUserId, callback)   
    --print("postDeleteFriend")
    local body = {}
    local params = {
        { "friendId" , tostring(targetUserId) }
    }

    WebService.asyncDelete(SkyBlockWeb.deleteFriendApi, params, json.encode(body), callback)
end

---添加好友
function SkyBlockWeb:postAddFriend(targetUserId, callback)   
    --print("postAddFriend")
    local body = {
        friendId = tostring(targetUserId),
        msg = ""
    }

    local params = {
        { "friendId" , tostring(targetUserId) },
        { "msg", "" }
    }

    WebService.asyncPost(SkyBlockWeb.addFriendApi, params, body, callback)
end