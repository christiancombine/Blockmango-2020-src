--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "Match"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent,self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent,self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent,self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerPickupItemEvent.registerCallBack(self.onPlayerPickupItem)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    RancherExBeginEvent.registerCallBack(self.onRancherExBegin)
    RancherExTaskFinishEvent.registerCallBack(self.onRancherExTaskFinish)
    PlayerBuyGoodsAddItemEvent.registerCallBack(self.onPlayerBuyGoodsAddItem)
    UseItemTeleportEvent.registerCallBack(self.onUseItemTeleport)
    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    ChangeCurrentItemInfoEvent.registerCallBack(self.onChangeCurrentItemInfo)
    PlayerBuyCommodityEvent.registerCallBack(self.onPlayerBuyCommodity)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
end

function PlayerListener.onPlayerLogin(clientPeer)


    if GameMatch.hasStartGame then
        return GameOverCode.GameStarted
    end
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    GameMatch:getDBDataRequire(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    MerchantConfig:prepareStore(player.rakssid)
    return 0
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if damageType == "generic"
        or string.find(damageType, "player") ~= nil then

        return false
    end

    return true
end

function PlayerListener.onPlayerRespawn(player)
    if player ~= nil then
        -- HostApi.log("onPlayerRespawn")
        if player.tempInventory ~= nil then
            for i, v in pairs(player.tempInventory) do
                if ToolConfig:isTool(v.id) == true then
                    if player.entityPlayerMP ~= nil then
                        player.entityPlayerMP:addItem(v.id, v.num, v.meta, false, ToolConfig:getDurableByToolId(v.id))
                    end
                else
                    if player.entityPlayerMP ~= nil then
                        player.entityPlayerMP:addItem(v.id, v.num, v.meta)
                    end
                end
            end
            player.tempInventory = {}
        end
        player:onDie()
    end
    return 0
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByRakssid(diePlayer:getRaknetID())

    if dier ~= nil then
        -- HostApi.log("onPlayerDied")
        local inv = dier:getInventory()
        for i = 1, 40 do
            local info = inv:getItemStackInfo(i - 1)
            if info and info.id ~= 0 then
                local item = {}
                item.id = info.id
                item.num = info.num
                item.meta = info.meta
                table.insert(dier.tempInventory, item)
            end
        end
        RespawnHelper.sendRespawnCountdown(dier, 3)
    end
    
    return true
end

function PlayerListener.onPlayerPickupItem(rakssid, itemId, itemNum, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    return true
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onRancherExBegin(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    HostApi.log("onRancherExBegin " .. GameMatch.curStatus)

    if GameMatch.curStatus == GameMatch.Status.WaitingPlayer then
        MsgSender.sendBottomTipsToTarget(player.rakssid, 3, Messages:PlayerNotEnough())
    end

    if GameMatch.curStatus == GameMatch.Status.WaitingPlayerToStart then
        GameMatch:readyToStart()
    end

    if GameMatch.curStatus == GameMatch.Status.GameOver then
        MsgSender.sendBottomTipsToTarget(player.rakssid, 3, Messages:GameHasOver())
    end
end

function PlayerListener.onRancherExTaskFinish(rakssid, itemId, itemNum)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    HostApi.log("onRancherExTaskFinish " .. GameMatch.curStatus .. " " .. itemId .. " " .. itemNum)

    if GameMatch.curStatus ~= GameMatch.Status.Start then
        return
    end

    GameMatch:onFinishTask(rakssid, itemId, itemNum)
end

function PlayerListener.onPlayerBuyGoodsAddItem(rakssid, itemId, itemMeta, itemNum)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    -- HostApi.log("onPlayerBuyGoodsAddItem:" .. itemId .. " " .. ItemID.TELEPORTHOME)

    if ItemID.PICKAXE_IRON == itemId or ItemID.PICKAXE_DIAMOND == itemId then
        if player.entityPlayerMP ~= nil then
            player.entityPlayerMP:addItem(itemId, 1, 0, false, ToolConfig:getDurableByToolId(itemId))
        end
    else
        if player.entityPlayerMP ~= nil then
            player.entityPlayerMP:addItem(itemId, 1, 0, false, 0)
        end
    end
    
    return false
end

function PlayerListener.onUseItemTeleport(entityId, itemId)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil then
        return
    end

    if ItemID.TELEPORTHOME == itemId then
        player:onDie()
    end
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return true
    end

    local player = PlayerManager:getPlayerByRakssid(movePlayer:getRaknetID())

    if player == nil then
        return true
    end

    if player.is_send_go_to_other_game == true then
        HostApi.log("is_send_go_to_other_game")
        return true
    end

    if GameMatch.curStatus == GameMatch.Status.GameOver then
        local beginPos = GameConfig.Gate[1]
        local endPos = GameConfig.Gate[2]

        if beginPos and endPos and 
            x >= tonumber(beginPos[1]) and x <= tonumber(endPos[1]) and
            y >= tonumber(beginPos[2]) and y <= tonumber(endPos[2]) and
            z >= tonumber(beginPos[3]) and z <= tonumber(endPos[3]) then
        
            EngineUtil.sendEnterOtherGame(player.rakssid, "g1031", player.userId)
            player.is_send_go_to_other_game = true

        end
    end

    return true
end

function PlayerListener.onChangeCurrentItemInfo(rakssid, itemIndex)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local inv = player:getInventory()
    if inv then
        local info = inv:getItemStackInfo(itemIndex)
        if info and info.id ~= 0 then
            HostApi.log("onChangeCurrentItemInfo:" .. info.id .. " " .. info.meta)
            local des = GameConfig:getCurrentItemDesById(info.id)
            local durable = ToolConfig:getDurableByToolId(info.id) - info.meta
            if durable < 0 then
                durable = 0
            end
            if ToolConfig:isTool(info.id) then
                HostApi.showRanchExCurrentItemInfo(player.rakssid, true, des, 0, durable)
            else
                HostApi.showRanchExCurrentItemInfo(player.rakssid, true, des, 1, 0)
            end
            return
        end
    end

    HostApi.showRanchExCurrentItemInfo(player.rakssid, false, "", 1, 0)
end

function PlayerListener.onPlayerBuyCommodity(rakssid, type, goodsInfo, attr, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    local goodsId = goodsInfo.goodsId
    local goodsNum = goodsInfo.goodsNum
    HostApi.log("onPlayerBuyCommodity " .. goodsId .. " " .. goodsNum)
    player:onAddRanchGoods(goodsId, goodsNum)

    attr.isAddGoods = false

    return true
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if GameConfig:CanDrop(itemId, itemMeta) then
        return true
    end

    return false
end

return PlayerListener
--endregion
