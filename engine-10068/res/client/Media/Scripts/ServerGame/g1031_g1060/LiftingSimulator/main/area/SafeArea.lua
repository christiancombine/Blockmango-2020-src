SafeArea = class("SafeArea", BaseArea)

function SafeArea:onPlayerIn(player)
    GamePacketSender:SendPlayerIsAllowWorkout(player.rakssid, false)
    GamePacketSender:SendPlayerIsAllowAttack(player.rakssid, false)
    player.property:addInvincibleState(Utility.dayTime)
end

function SafeArea:onPlayerStay(player)

end

function SafeArea:onPlayerOut(player)
    player.curArea = Define.Area.CommonArea

    GamePacketSender:SendPlayerIsAllowWorkout(player.rakssid, true)
    GamePacketSender:SendPlayerIsAllowAttack(player.rakssid, true)

    player.property:removeInvincibleState()
end