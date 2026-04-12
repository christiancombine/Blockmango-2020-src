GamePacketSender = {}

function GamePacketSender:sendSkyBlockSignInData(rakssid, data)
    PacketSender:sendServerCommonData(rakssid, "SkyBlockMainData", "SignInData", data)
end

function GamePacketSender:sendSkyBlockRewardResult(rakssid, Id)
    local params = {
        id = Id
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(rakssid, "SkyBlockMainData", "SignInRewardResult", data)
end

function GamePacketSender:sendSkyBlockLoadingResult(rakssid, loading)
    local params = {
        loading = loading
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(rakssid, "MainControlData", "SkyBlockLoading", data)
end

function GamePacketSender:sendSkyBlockPartyData(rakssid, data)
    if rakssid == 0 then
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            PacketSender:sendLuaCommonData(player.rakssid, "PartyListData", data)
        end
    else
        PacketSender:sendLuaCommonData(rakssid, "PartyListData", data)
    end
end

function GamePacketSender:sendCanReciveNewPri(rakssid, isShow, idList)
    local data = {
        isShowIcon = isShow,
        needRecivePriId = idList or {}
    }
    PacketSender:sendServerCommonData(rakssid, "SkyBlockMainData", "ReviveNewPri", json.encode(data))
end

function GamePacketSender:sendPartyData(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "PartyData", data)
end

function GamePacketSender:sendPlayerPartyData(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "playerGameData", data)
end

function GamePacketSender:sendUpDateLoveNum(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "UpdateLoveNum", data)
end

function GamePacketSender:sendSignPostData(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "EditSignPost", data)
end

function GamePacketSender:sendMultiBuildGroupData(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "MultiBuildGroupData", data)
end

function GamePacketSender:sendSkyBlockRankData(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "SkyBlockRankData", data)
end

function GamePacketSender:sendFriendOperation(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "SkyBlockFriendOperation", data)
end

function GamePacketSender:sendMultiBuildUpdate(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "MultiBuildUpdate", data)
end

function GamePacketSender:sendSkyBlockPriImg(rakssid, info)
    local data = {
        priList = info
    }
    PacketSender:sendServerCommonData(rakssid, "SkyBlockMainData", "SupemePrivilegeImg", json.encode(data))
end

function GamePacketSender:sendWelcomeVisitorTip(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "WelcomeTip", data)
end

function GamePacketSender:sendBulletinData(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "BulletinData", json.encode(data))
end

function GamePacketSender:sendShowLackMoney(rakssid)
    local data = {}
    PacketSender:sendLuaCommonData(rakssid, "ShowLackMoney", json.encode(data))
end

function GamePacketSender:sendShowLackDiamond(rakssid)
    PacketSender:sendServerCommonData(rakssid, "SkyBlockMainData", "ShowLackDiamondPanel", json.encode(data))
end

function GamePacketSender:sendShowChristmastree(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "showChristmastreeUI", json.encode(data))
end

function GamePacketSender:sendChristmasReward(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "ChristmasReward", json.encode(data))
end

function GamePacketSender:sendGiftNumList(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "SendGiftNumList", json.encode(data))
end

function GamePacketSender:sendOpenGiftReward(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "SendOpenGiftReward", json.encode(data))
end

function GamePacketSender:sendChristUseBoxItem(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "ChristUseBoxItem", json.encode(data))
end

function GamePacketSender:sendShowSwitchRed(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "SwitchShowRedPoint", json.encode(data))
end

function GamePacketSender:sendShowRichRed(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "RichShowRedPoint", json.encode(data))
end

function GamePacketSender:sendBuildGroupInfo(player)
    local builder = DataBuilder.new()
    local targetUserId = GameMatch:getTargetByUid(player.userId)
    if targetUserId and targetUserId == player.userId then --如果在自己家里，只强行发送自己的数据
        builder:fromTable({ BuildGroupId = player.userId, TargetUserId = player.userId })
    else --在别人的领地
        local targetPlayer = PlayerManager:getPlayerByUserId(targetUserId)
        if targetPlayer then --领主在线，发送共同建造信息
            builder:fromTable({ BuildGroupId = player.buildGroupId, TargetUserId = targetUserId })
        else --领主不在线，发送不同的建造信息
            builder:fromTable({ BuildGroupId = player.buildGroupId, TargetUserId = 0 })
        end
    end
    PacketSender:sendLuaCommonData(player.rakssid, "BuildGroupInfo", builder:getData())
end

function GamePacketSender:sendFlyRemainingTime(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "FlyRemainingTime", json.encode(data))
end

return GamePacketSender