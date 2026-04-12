
GamePacketSender = {}

function GamePacketSender:sendCreateParty(data)
    PacketSender:sendLuaCommonData("CreateParty", data)
end

function GamePacketSender:sendApplyParty(data)
    PacketSender:sendLuaCommonData("ApplyParty", data)
end

function GamePacketSender:sendGiveLove(targetUserId)
    local params = {
        targetUserId =  tostring(targetUserId)
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData("GiveLove", data)
end

function GamePacketSender:sendOnToolBarGiveLove()
    PacketSender:sendLuaCommonData("ToolBarGiveLove", "{}")
end

function GamePacketSender:sendApplySignPost(data)
    PacketSender:sendLuaCommonData("ApplySignPost", data)
end

function GamePacketSender:sendGetTogetherBuildData()
    PacketSender:sendLuaCommonData("GetTogetherBuildData", "{}")
end

function GamePacketSender:sendOtherPlayerUpdateData(targetUserId, typeList)
    local data = {
        targetUserId = tostring(targetUserId),
        typeList = typeList
    }
    
    PacketSender:sendLuaCommonData("OtherPlayerUpdateData", json.encode(data))
end

function GamePacketSender:kickOutPlayer(targetUserId, msg)
    local data = {
        targetUserId = tostring(targetUserId),
        tips = tostring(msg)
    }
    PacketSender:sendLuaCommonData("KickOutPlayer", json.encode(data))
end

function GamePacketSender:sendGotoHall()
    
    PacketSender:sendLuaCommonData("SkyBlockGotoHall", "{}")
end

function GamePacketSender:sendAskTargetShowTip(userId, tips)
    local data = {
        targetUserId = tostring(userId),
        tips = tostring(tips)
    }
    PacketSender:sendLuaCommonData("SkyBlockShowTips", json.encode(data))
end

function GamePacketSender:sendClickBulletinRed(data)
    PacketSender:sendLuaCommonData("ClickBulletin", data)
end

function GamePacketSender:sendBuyMoneyUseDiamond(data)
    PacketSender:sendLuaCommonData("BuyMoney", json.encode(data))
end

function GamePacketSender:sendBuyChirstmasGift(data)
    PacketSender:sendLuaCommonData("BuyChirstmasGift", json.encode(data))
end

function GamePacketSender:sendRecycleChristmastree(data)
    PacketSender:sendLuaCommonData("RecycleChristmastree", json.encode(data))
end

function GamePacketSender:sendGetChristmasReward(data)
    PacketSender:sendLuaCommonData("GetChristmasReward", data)
end

function GamePacketSender:sendGetGiftNum()
    local data = {}
    PacketSender:sendLuaCommonData("GetGiftNum", json.encode(data))
end

function GamePacketSender:sendOpenGift(data)
    PacketSender:sendLuaCommonData("OpenGift", json.encode(data))
end

function GamePacketSender:sendCommonClick(data)
    PacketSender:sendLuaCommonData("SkyBlockCommonClick", json.encode(data))
end

function GamePacketSender:sendRedSwitchClick(data)
    PacketSender:sendLuaCommonData("SkyBlockRedSwitchClick", "{}")
end

function GamePacketSender:sendRedRichBtnClick(data)
    PacketSender:sendLuaCommonData("SkyBlockRedRichBtnClick", "{}")
end

return GamePacketSender