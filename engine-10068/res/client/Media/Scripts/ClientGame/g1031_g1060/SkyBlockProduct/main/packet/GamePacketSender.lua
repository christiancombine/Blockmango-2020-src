
GamePacketSender = {}

function GamePacketSender:sendBuyMoneyUseDiamond(data)
    PacketSender:sendLuaCommonData("BuyMoney", json.encode(data))
end

function GamePacketSender:sendGetGiftNum()
    local data = {}
    PacketSender:sendLuaCommonData("GetGiftNum", json.encode(data))
end

function GamePacketSender:sendOpenGift(data)
    PacketSender:sendLuaCommonData("OpenGift", json.encode(data))
end

function GamePacketSender:sendBuyChirstmasGift(data)
    PacketSender:sendLuaCommonData("BuyChirstmasGift", json.encode(data))
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