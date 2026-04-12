--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "data.GamePlayer"
require "Match"
require "messages.Messages"
require "config.BridgeConfig"
require "task.GameTimeTask"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerBuyCommodityEvent.registerCallBack(self.onPlayerBuyCommodity)
    PlayerUseCommonItemEvent.registerCallBack(self.onPlayerUseCommonItem)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerBuyGoods)
    PlayerBuyRespawnResultEvent.registerCallBack(self.onBuyRespawnResult)
    PlayerAttackActorNpcEvent.registerCallBack(self.onPlayerAttackActorNpc)
    PlayerUpgradeResourceEvent.registerCallBack(self.onPlayerUpgradeResource)
    PlayerKeepItemEvent.registerCallBack(self.onPlayerKeepItem)
    OpenEnchantmentEvent.registerCallBack(self.onOpenEnchantment)
    EnchantmentEffectClickEvent.registerCallBack(self.onEnchantmentEffectClick)
    PlayerEnchantmentQuickEvent.registerCallBack(self.onPlayerEnchantmentQuick)
end

function PlayerListener.onPlayerKeepItem(rakssid, isKeepItem)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if player.keepItemSource == 0 then
        RespawnHelper.sendRespawnCountdown(player, 3)
    end
    if isKeepItem then
        local Config = KeepItemConfig:getKeepItemByTimes(player.keepItemTimes)
        player:consumeDiamonds(101, Config.price, "keepItem", true)
    end
end

function PlayerListener.onPlayerLogin(clientPeer)
    if GameMatch.hasStartGame then
        return GameOverCode.GameStarted
    end
    local player = GamePlayer.new(clientPeer)
    player:init()
    if GameMatch.startWait == false then
        GameMatch.startWait = true
        WaitingPlayerTask:start()
    end
    if PlayerManager:isPlayerFull() then
        WaitPlayerTask:stop()
    end
    return GameOverCode.Success, player
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    HostApi.sendPlaySound(player.rakssid, 10000)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    for _, merchant in pairs(GameMatch.Merchants) do
        merchant:syncPlayer(player.rakssid)
    end
    CenterMerchantConfig:addEffect()
    CenterMerchantConfig:prepareStore(player.rakssid)
    return 43200
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if (GameMatch.allowPvp == false) then
        return false
    end

    if (hurtFrom == nil) then
        return true
    end

    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    local fromer = PlayerManager:getPlayerByPlayerMP(hurtFrom)

    if hurter ~= nil and fromer ~= nil then
        if hurter.team.id == fromer.team.id then
            return false
        end
    end

    if hurtValue.value < 1 then
        hurtValue.value = 1
    end

    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)

    if GameMatch:isGameStart() == false then
        return true
    end

    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)

    if (iskillByPlayer and killPlayer ~= nil and dier ~= nil) then
        local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)

        if (killer ~= nil) then
            MsgSender.sendMsg(IMessages:msgPlayerKillPlayer(dier.name, killer.name))

            if (GameMatch.firstKill == false) then
                GameMatch.firstKill = true
                MsgSender.sendMsg(IMessages:msgFirstKill(killer.name))
            end

            dier:onDieByPlayer()
            killer:onKill()
        end
    end

    if (dier ~= nil) then
        dier:setInventory()
        GameMatch:onPlayerDie(dier)
    end

    return true
end

function PlayerListener.onPlayerRespawn(player)
    if player.isKeepItem then
        player:fillInvetory(player.inventory)
    end
    player:teleInitPos()
    return 43200
end

function PlayerListener.onPlayerBuyCommodity(rakssid, type, goodsInfo, attr, msg)
    local goodsId = goodsInfo.goodsId
    local goodsNum = goodsInfo.goodsNum
    if GameMatch:isGameStart() == false then
        msg.value = IMessages:msgDontBuyGoodsByNoStart()
        return false
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if type ~= 6 then
        return true
    end

    return true
end

function PlayerListener.onPlayerUseCommonItem(rakssId, itemId)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return
    end
    if itemId >= 462 and itemId <= 466 then -- tp
        local pos = TeleportConfig:getRandomPosition(player.team.id, itemId)
        if pos ~= nil then
            player:teleportPos(pos)
        end
    end

    if itemId >= 467 and itemId <= 471 then
        local pos = player:getPosition()
        BridgeConfig:buildBridge(itemId, player:getYaw(), VectorUtil.toBlockVector3(pos.x, pos.y - 1, pos.z))
    end
end

function PlayerListener.onPlayerBuyGoods(rakssid, type, itemId, msg)
    if GameMatch:isGameStart() == false then
        msg.value = IMessages:msgDontBuyGoodsByNoStart()
        return false
    end
    return true
end


function PlayerListener.onBuyRespawnResult(rakssid, code)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        GameMatch:ifGameOver()
        return
    end
    if code == 0 then
        player:onDie()
        GameMatch:ifGameOver()
    end
    if code == 1 then
        player.isKeepItem = false
        player.keepItemSource = 1
        local Config = KeepItemConfig:getKeepItemByTimes(player.keepItemTimes)
        player.KeepItemSource = 1
        HostApi.sendKeepItemTip(player.rakssid, Config.coinId, Config.price, Config.tipTime)
        MsgSender.sendMsg(IMessages:msgRespawn(player:getDisplayName()))
    end
end

function PlayerListener.onPlayerAttackActorNpc(rakssid, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    -- mineral update npc
    local resource = GameMatch:getMoneyConfigByEntityId(entityId)
    if resource ~= nil then
        local level = GameMatch.money[resource.id].mineral.level
        if level >= GameConfig.maxLevel then
            HostApi.showUpgradeResourceUI(rakssid, -1, 1, resource.id, Messages:highestLevel(level, resource.mineral.itemName, resource.mineral.num, resource.mineral.time))
            return
        end
        HostApi.showUpgradeResourceUI(rakssid, resource.mineral.costItem, resource.mineral.costNum, resource.id, Messages:updateInfo(level, resource.mineral.itemName, resource.mineral.num, resource.mineral.time))
    end

    -- egg
    local egg = GameMatch:getEggConfigByEntityId(entityId)
    if egg ~= nil then

        if GameMatch:isGameStart() == false then
            return
        end

        if player.team.id == egg.id then
            return
        end

        for i, team in pairs(GameMatch.Teams) do
            if team.id == egg.id and team.isHaveBed then
                player:onDestroyBed()
                team:destroyBed()
                MsgSender.sendMsg(10007, Messages:breakEgg(team:getDisplayName(), player:getDisplayName()))
                EngineWorld:removeEntity(egg.entityId)
            end
        end
    end

end

function PlayerListener.onPlayerUpgradeResource(rakssid, resourceId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local mineral = GameMatch.money[resourceId].mineral
    local inv = player:getInventory()
    if inv:getItemNum(mineral.costItem) >= mineral.costNum then
        inv:removeItem(mineral.costItem, mineral.costNum)
        GameMatch:upgradeResource(resourceId)
        HostApi.sendCommonTip(rakssid, GameConfig.updateSuccess)
        HostApi.sendPlaySound(rakssid, GameConfig.updateSound)
        return
    end
    HostApi.sendCommonTip(rakssid, GameConfig.updateFailed)
end

function PlayerListener.onOpenEnchantment(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if GameMatch.allowEnchantment == false then
        if GameTimeTask.tick then
            local left_time = GameConfig.enchantmentOpenTime - GameTimeTask.tick
            if left_time < 0 then
                left_time = 0
            end
            MsgSender.sendBottomTipsToTarget(rakssid, 3, Messages:EnchantMentLeftTime(left_time))
        end
        return
    end

    player:sendOpenChantment(0)
end

function PlayerListener.onEnchantmentEffectClick(rakssid, equipId, consumeId, index)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if equipId == 0 or consumeId == 0 then
        return
    end

    local effectId, effectLevel = GameConfig:getEnchantmentEffect(index + 1)

    if effectId > 0 and effectLevel > 0 then
        local hasEquip = false
        local hasConsume = false
        local equipIndex = -1
        local consumeIndex = -1

        local inv = InventoryUtil:getPlayerInventoryInfo(player)
        local invPlayer = player:getInventory()
        if inv and inv.item then
            for _, stack in pairs(inv.item) do

                local isEnchantment = false

                if invPlayer then
                    isEnchantment = invPlayer:isEnchantment(stack.stackIndex)
                end

                if stack.id == equipId and isEnchantment == false then
                    equipIndex = stack.stackIndex
                    hasEquip = true
                end

                if stack.id == consumeId then
                    consumeIndex = stack.stackIndex
                    hasConsume = true
                end
            end
        end

        if hasEquip and hasConsume then

            if invPlayer then
                -- player:removeItem(equipId, 1)
                -- player:removeItem(consumeId, 1)
                if equipIndex >= 0 and consumeIndex >= 0 then
                    invPlayer:decrStackSize(equipIndex, 1)
                    invPlayer:decrStackSize(consumeIndex, 1)
                    player:addEnchantmentItem(equipId, 1, 0, effectId, effectLevel)
                    player:sendOpenChantment(1)
                end
            end

        end
    end
end

function PlayerListener.onPlayerEnchantmentQuick(rakssid, equipId, index)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if equipId == 0 then
        return
    end

    local effectId, effectLevel = GameConfig:getEnchantmentEffect(index + 1)

    if effectId == 0 or effectLevel == 0 then
        return
    end

    local price = GameConfig.enchantmentQuickMoney
    player:consumeDiamonds(102, price, "enchantmentQuick" .. "#" .. equipId .. "#" .. effectId .. "#" .. effectLevel, true)
end

return PlayerListener
--endregion
