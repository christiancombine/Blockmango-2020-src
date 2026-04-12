ShopArea = class("ShopArea", BaseArea)

function ShopArea:onPlayerIn(player)

    GamePacketSender:SendLiftingSimulatorShopRegion(player.rakssid, true)
    GamePacketSender:SendPlayerIsAllowWorkout(player.rakssid, false)
    GamePacketSender:SendPlayerIsAllowAttack(player.rakssid, false)

    player.property:addInvincibleState(Utility.dayTime)
end

function ShopArea:onPlayerStay(player)

end

function ShopArea:onPlayerOut(player)
    player.curArea = Define.Area.CommonArea

    GamePacketSender:SendLiftingSimulatorShopRegion(player.rakssid, false)
    GamePacketSender:SendPlayerIsAllowWorkout(player.rakssid, true)
    GamePacketSender:SendPlayerIsAllowAttack(player.rakssid, true)

    player.property:removeInvincibleState()
end