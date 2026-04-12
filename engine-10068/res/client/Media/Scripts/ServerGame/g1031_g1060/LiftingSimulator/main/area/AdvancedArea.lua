AdvancedArea = class("AdvancedArea", BaseArea)

function AdvancedArea:onPlayerIn(player)
    GamePacketSender:SendPlayerIsAllowWorkout(player.rakssid, false)
    GamePacketSender:SendPlayerIsAllowAttack(player.rakssid, true)
end

function AdvancedArea:onPlayerStay(player)

end

function AdvancedArea:onPlayerOut(player)
    player.curArea = Define.Area.CommonArea
    GamePacketSender:SendPlayerIsAllowWorkout(player.rakssid, true)
end