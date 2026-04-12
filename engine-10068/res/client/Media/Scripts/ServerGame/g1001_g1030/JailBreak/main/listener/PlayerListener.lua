--region *.lua
require "config.LocationConfig"
require "config.DamageConfig"
require "config.DoorConfig"
require "config.TransferGateConfig"
require "messages.Messages"
require "data.GamePlayer"
require "Match"

PlayerListener = {}

function PlayerListener:init()
    DBManager:setPlayerDBSaveFunction(function(player, immediate)
        GameMatch:savePlayerData(player, immediate)
    end)

    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(GetDataFromDBEvent, self.onGetDataFromDB)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerBuyGoods)
    PlayerBuyCommodityEvent.registerCallBack(self.onPlayerBuyCommodity)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerSelectRoleEvent.registerCallBack(self.onPlayerSelectRole)
    PlayerBuyVehicleSuccessEvent.registerCallBack(self.onPlayerBuyVehicleSuccess)
    PlayerOpenDoorEvent.registerCallBack(self.onPlayerOpenDoor)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    HostApi.sendPlaySound(player.rakssid, 10016)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    GameMatch:getDBDataRequire(player)
end

function PlayerListener.onPlayerRespawn(player)
    player:onRespawn()
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)

    if x == 0 and y == 0 and z == 0 then
        return true
    end

    local player = PlayerManager:getPlayerByPlayerMP(movePlayer)

    if player == nil then
        return true
    end

    local location = player.location

    for i, v in pairs(TransferGateConfig.transferGate) do
        if v.pos:inArea(VectorUtil.newVector3(x, y, z)) then
            if v.oldMap == 1 then
                if player.oldMapTeleport == false then
                    -- for the old map data , just avoid drop to the "void"
                    player:teleportPos(TransferGateConfig:getTransferPosition(v.transferPos))
                    player.oldMapTeleport = true
                end
            else
                player:teleportPos(TransferGateConfig:getTransferPosition(v.transferPos))
            end
        end
    end

    if location == nil then

        for i, location in pairs(LocationConfig.locations) do
            if location.area:inArea(VectorUtil.newVector3(x, y, z)) then
                player:enterLocation(location)
                player:onEnterJail()
                return true
            end
        end

        return true
    end

    if location ~= nil then

        if location.area:inArea(VectorUtil.newVector3(x, y, z)) == false then
            player:quitLocation()
        end

        return true
    end

    return true
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if (hurtFrom == nil) then
        return true
    end

    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    local fromer = PlayerManager:getPlayerByPlayerMP(hurtFrom)

    if hurter ~= nil and fromer ~= nil then

        if hurter.role == fromer.role then
            return false
        end

        if (hurter.role == GamePlayer.ROLE_PRISONER and fromer.role == GamePlayer.ROLE_CRIMINAL)
                or (hurter.role == GamePlayer.ROLE_CRIMINAL and fromer.role == GamePlayer.ROLE_PRISONER)
                or (hurter.role == GamePlayer.ROLE_PENDING)
                or (fromer.role == GamePlayer.ROLE_PENDING) then
            return false
        end

        local itemId = fromer:getHeldItemId()

        if itemId >= ItemID.HAND_CUFFS and itemId <= ItemID.HAND_CUFFS and hurter.role == GamePlayer.ROLE_PRISONER then
            return false
        end

        local weapon = DamageConfig:getDamage(itemId)

        if weapon == nil then
            return true
        end

        hurtValue.value = weapon.damage

    end

    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if not dier then
        return false
    end
    local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
    if not killer then
        dier:onDie()
        return true
    end
    if dier.role == GamePlayer.ROLE_CRIMINAL or dier.role == GamePlayer.ROLE_PRISONER then
        local itemId = killer:getHeldItemId()
        if itemId >= ItemID.HAND_CUFFS and itemId <= ItemID.HAND_CUFFS then
            killer:captureCriminal(dier)
        else
            killer:killCriminal(dier)
        end
    else
        if dier.role == GamePlayer.ROLE_POLICE then
            killer:killPolice(dier)
        end
    end
    dier:onDie()
    return true
end

function PlayerListener.onPlayerBuyGoods(rakssid, type, itemId, msg, isAddItem)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    return player ~= nil
end

function PlayerListener.onPlayerBuyCommodity(rakssid, type, goodsInfo, attr, msg)
    local goodsId = goodsInfo.goodsId
    local goodsNum = goodsInfo.goodsNum
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if goodsId == 10000 then
        attr.isAddGoods = false
        player:addCurrency(goodsNum)
    end
    return true
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerSelectRole(rakssid, role)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if role > GamePlayer.ROLE_CRIMINAL or role < GamePlayer.ROLE_POLICE then
        role = GamePlayer.ROLE_CRIMINAL
    end
    player:selectRoleRequire(role)
end

function PlayerListener.onPlayerBuyVehicleSuccess(rakssid, vehickeId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    player:saveVehicle(vehickeId)
end

function PlayerListener.onPlayerOpenDoor(entityId, pos)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil then
        return
    end
    if player.role == GamePlayer.ROLE_POLICE then
        return
    end
    local door = DoorConfig:getDoor(pos)
    if door == nil then
        return
    end
    if door.name == 0 then
        return
    end
    MsgSender.sendMsg(Messages:openDoor(door.name))
    player:addThreat(door.threat)
end

function PlayerListener.onGetDataFromDB(player, role, data)
    if GameMatch.selectPoliceRoleDataTrigger[tostring(player.userId)] == true then
        if role == GamePlayer.ROLE_POLICE then
            GameMatch:policeRoleDataCache(player, role, data)
            GameMatch.selectPoliceRoleDataTrigger[tostring(player.userId)] = nil
        end
    end

    if GameMatch.selectCriminalRoleDataTrigger[tostring(player.userId)] == true then
        if role == GamePlayer.ROLE_CRIMINAL then
            GameMatch:criminalRoleDataCache(player, role, data)
            GameMatch.selectCriminalRoleDataTrigger[tostring(player.userId)] = nil
        end
    end

    if GameMatch.selectPoliceRoleDataCache[tostring(player.userId)] == true
            and GameMatch.selectCriminalRoleDataCache[tostring(player.userId)] == true then
        GameMatch:sendSelectRoleData(player)
        GameMatch.selectPoliceRoleDataCache[tostring(player.userId)] = nil
        GameMatch.selectCriminalRoleDataCache[tostring(player.userId)] = nil
    end

    if GameMatch.selectRoleTrigger[tostring(player.userId)] == true then
        player:selectRoleFromDBData(data, role)
        GameMatch:sendAllPlayerTeamInfo()
        GameMatch.selectRoleTrigger[tostring(player.userId)] = nil
        HostApi.sendPlaySound(player.rakssid, 10015)
    end
end

return PlayerListener
--endregion
