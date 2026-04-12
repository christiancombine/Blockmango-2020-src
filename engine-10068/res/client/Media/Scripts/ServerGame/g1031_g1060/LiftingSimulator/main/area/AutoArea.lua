AutoArea = class("AutoArea", BaseArea)

function AutoArea:onPlayerIn(player)
    GamePacketSender:SendStayAreaInfo(player.rakssid, self.Type, true)
    player.property:addInvincibleState(Utility.dayTime, true)

    player.property.inAutoArea = true
    GamePacketSender:SendAutoAreaInfo(player.userId, player.property.inAutoArea)
end

function AutoArea:onPlayerStay(player)

end

function AutoArea:onPlayerOut(player)
    player.curArea = Define.Area.CommonArea
    GamePacketSender:sendAutoWorkoutBack(player.rakssid, false)
    AdvertConfig:backAutoWork(player)
    GamePacketSender:SendStayAreaInfo(player.rakssid, self.Type, false)
    player.property:removeInvincibleState()

    player.property.inAutoArea = false
    GamePacketSender:SendAutoAreaInfo(player.userId, player.property.inAutoArea)
end