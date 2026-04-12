SellArea = class("SellArea", BaseArea)

function SellArea:onPlayerIn(player)
    GamePacketSender:SendPlayerIsAllowWorkout(player.rakssid, false)
    GamePacketSender:SendPlayerIsAllowAttack(player.rakssid, false)
    player.property:addInvincibleState(Utility.dayTime)

    HostApi.sendPlaySound(player.rakssid, 414)
    player:sellMuscleMass()
end

function SellArea:onPlayerStay(player)

end

function SellArea:onPlayerOut(player)
    player.curArea = Define.Area.CommonArea

    GamePacketSender:SendPlayerIsAllowWorkout(player.rakssid, true)
    GamePacketSender:SendPlayerIsAllowAttack(player.rakssid, true)
    player.property:removeInvincibleState()
end

return SellArea