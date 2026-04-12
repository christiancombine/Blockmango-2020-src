
GamePacketSender = {}

function GamePacketSender:sendNotifySellItemFail(rakssid)
    PacketSender:sendServerCommonData(rakssid, "SkyBlockMiningAreaData", "NotifySellItemFail", "")
end

function GamePacketSender:sendCanReciveNewPri(rakssid, isShow)
    local params = {
        isShowIcon = isShow
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(rakssid, "MainControlData", "ReviveNewPri", data)
end

function GamePacketSender:sendShowGuideArrowEvent(rakssid, sendData)
    local data = DataBuilder.new():fromTable(sendData):getData()
    PacketSender:sendServerCommonData(rakssid, "SkyBlockProductData", "GuideArrow", data)
end

function GamePacketSender:sendSkyBlockPriImg(rakssid, info)
    local data = {
        priList = info
    }
    PacketSender:sendServerCommonData(rakssid, "SkyBlockMainData", "SupemePrivilegeImg", json.encode(data))
end

function GamePacketSender:sendShowGuideArrowBtn(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "showGuideArrowBtn", data)
end

function GamePacketSender:sendShowLackMoney(rakssid)
    local data = {}
    PacketSender:sendLuaCommonData(rakssid, "ShowLackMoney", json.encode(data))
end

function GamePacketSender:sendShowLackDiamond(rakssid)
    PacketSender:sendServerCommonData(rakssid, "SkyBlockMainData", "ShowLackDiamondPanel", json.encode(data))
end

function GamePacketSender:sendChristUseBoxItem(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "ChristUseBoxItem", json.encode(data))
end

function GamePacketSender:sendGiftNumList(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "SendGiftNumList", json.encode(data))
end

function GamePacketSender:sendOpenGiftReward(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "SendOpenGiftReward", json.encode(data))
end

function GamePacketSender:sendShowSwitchRed(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "SwitchShowRedPoint", json.encode(data))
end

function GamePacketSender:sendShowRichRed(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "RichShowRedPoint", json.encode(data))
end

return GamePacketSender