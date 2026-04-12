SupplyUtil = {}
SupplyUtil.TYPE = {
    Bullet = 1,
    Defence = 2,
    Health = 3,
    Key = 4,
    Gold = 5,
    Block = 6,
    Gun = 7
}

function SupplyUtil:onPlayerPickup(type, value, player)
    player.taskController:packingProgressData(player, Define.TaskType.Fright.PickSupply, GameMatch.gameType, type)
    if type == SupplyUtil.TYPE.Bullet then
        self:onPickupBullet(player, value)
        return
    end
    if type == SupplyUtil.TYPE.Defence then
        self:onPickupDefence(player, value)
        return
    end
    if type == SupplyUtil.TYPE.Health then
        self:onPickupHealth(player, value)
        return
    end
    if type == SupplyUtil.TYPE.Key then
        self:onPickupKey(player, value)
        return
    end
    if type == SupplyUtil.TYPE.Gold then
        self:onPickupGold(player, value)
        return
    end
    if type == SupplyUtil.TYPE.Block then
        self:onPickupBlock(player, value)
        return
    end
    if type == SupplyUtil.TYPE.Gun then
        self:onPickupGun(player, value)
        return
    end
end

function SupplyUtil:onPickupBullet(player, value)
    player:onPickUpBullet(value)
    player.supplys.bullet = player.supplys.bullet + 1
    HostApi.sendSupplyTip(player.rakssid, 3, "set:items.json image:bullet05", value)
end

function SupplyUtil:onPickupDefence(player, value)
    player:addDefense(value)
    player.supplys.defense = player.supplys.defense + 1
    HostApi.sendSupplyTip(player.rakssid, 2, "set:items.json image:iron_chestplate", value)
end

function SupplyUtil:onPickupHealth(player, value)
    player:addHealth(value)
    player.supplys.health = player.supplys.health + 1
    HostApi.sendSupplyTip(player.rakssid, 1, "set:items.json image:potion_bottle_fill", value)
end

function SupplyUtil:onPickupKey(player, value)
    player:addKey(value)
    HostApi.notifyGetGoods(player.rakssid, "set:pixelgungameresult.json image:key", value)
end

function SupplyUtil:onPickupGold(player, value)
    player:addCurrency(value)
end

function SupplyUtil:onPickupBlock(player, value)
    player:onPickUpBlock(value)
    player.supplys.block = player.supplys.block + 1
    HostApi.sendSupplyTip(player.rakssid, 3, "set:game_props.json image:prop_13", value)
end

function SupplyUtil:onPickupGun(player, value)
    player:onPickupGun(value)
end