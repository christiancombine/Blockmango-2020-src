require "messages.Messages"
require "data.GamePlayer"
require "Match"
require "config.BlockConfig"
require "config.ItemConfig"
require "config.MerchantConfig"
require "config.JewelConfig"
require "config.ScoreConfig"
require "config.RankNpcConfig"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerAttackActorNpcEvent.registerCallBack(self.onPlayerAttackActorNpc)
    PlayerAttackSessionNpcEvent.registerCallBack(self.onPlayerAttackSessionNpc)
    PlayerBuyUpgradePropEvent.registerCallBack(self.onPlayerBuyUpgradeProp)
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
    HostApi.sendPlaySound(player.rakssid, 10000)
    MerchantConfig:syncPlayer(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    player:sendToolsData()
    RankNpcConfig:getPlayerRankData(player)
    return 43200
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if damageType == "fall" then
        return false
    end

    if GameMatch.curStatus ~= GameMatch.Status.Running then
        if hurtFrom ~= nil and hurtPlayer ~= nil then
            local targetPlayer = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
            if targetPlayer ~= nil then
                local rakssId = hurtFrom:getRaknetID()
                local targetId = targetPlayer.userId
                if rakssId ~= nil and targetId ~= nil then
                    HostApi.showPlayerOperation(rakssId, targetId)
                end
            end
        end
        return false
    end

    if hurtFrom == nil then
        return true
    end

    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    local fromer = PlayerManager:getPlayerByPlayerMP(hurtFrom)

    if hurter ~= nil and fromer ~= nil then
        if hurter.team.id == fromer.team.id then
            return false
        end

        local itemId = fromer:getHeldItemId()
        for _, v in pairs(ToolConfig.tool) do
            if itemId == v.id then
                hurtValue.value = v.hurtDamage
                return true
            end
        end
    end

    if hurtValue.value < 1 then
        hurtValue.value = 1
    end

    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    if diePlayer == nil then
        return
    end

    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if dier ~= nil then
        local inv = dier:getInventory()
        local index = 1
        for i = 1, 40 do
            local info = inv:getItemStackInfo(i - 1)
            if info.id ~= 0 then
                for i, item in pairs(ItemConfig.removeItem) do
                    if item.id == info.id then
                        dier:removeItem(info.id, info.num)
                    end
                end

                for i, v in pairs(ItemConfig.noDrop) do
                    for _, jewel in pairs(JewelConfig.jewels) do
                        if v.id == info.id and info.id ~= jewel.itemId then
                            dier.tempInventory[index] = info
                            index = index + 1
                        end
                    end
                end
            end
        end
        dier:showRespawn()

        if killPlayer == nil then
            if dier.jewelId ~= 0 then
                JewelConfig:GenerateById(dier.jewelId)
                dier.jewelId = 0
                dier:changeClothes("custom_back_effect", 0)
                dier:changeClothes("custom_foot_halo", 0)
            end
            return true
        end

        local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
        if killer ~= nil then
            killer:addMoney(GameConfig.moneyFromKill)
            killer:onKill()
            if dier.jewelId ~= 0 then
                killer:addItem(905, 1, 0) -- 905是宝石
                killer.jewelId = dier.jewelId
                killer:changeClothes("custom_back_effect", 1)
                killer:changeClothes("custom_foot_halo", 1)
                killer:addScore(ScoreConfig.GET_JEWEL)
                dier.jewelId = 0
                dier:changeClothes("custom_back_effect", 0)
                dier:changeClothes("custom_foot_halo", 0)
                local players = PlayerManager:getPlayers()
                for _, v in pairs(players) do
                    if v.rakssid == killer.rakssid then
                        MsgSender.sendBottomTipsToTarget(v.rakssid, GameConfig.gameTipShowTime, Messages:placeStatue())
                        MsgSender.sendOtherTipsToTarget(v.rakssid, GameConfig.gameTime, Messages:placeStatue())
                    else
                        MsgSender.sendBottomTipsToTarget(
                                v.rakssid,
                                GameConfig.gameTipShowTime,
                                Messages:playerGetJewel(killer.team:getDisplayName(), killer.name)
                        )
                        MsgSender.sendOtherTipsToTarget(
                                v.rakssid,
                                GameConfig.gameTime,
                                Messages:playerGetJewel(killer.team:getDisplayName(), killer.name)
                        )
                    end
                end
            end
        end
    end
    return true
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return true
    end

    local player = PlayerManager:getPlayerByPlayerMP(movePlayer)

    if player == nil then
        return true
    end

    if player.team ~= nil then
        if player.team.exchangePos:inArea(VectorUtil.newVector3(x, y, z)) == true then
            if player.isExchanged == false then
                local inv = player:getInventory()
                local money = 0
                local teamResource = 0
                for i = 1, 40 do
                    local info = inv:getItemStackInfo(i - 1)
                    if
                    info.id == ItemID.INGOT_IRON or info.id == ItemID.DIAMOND or info.id == ItemID.EMERALD or
                            info.id == ItemID.INGOT_GOLD
                    then
                        money = money + BlockConfig:getExchangeByItemId(info.id) * info.num
                        teamResource = teamResource + BlockConfig:getTeamResourceByItemId(info.id) * info.num
                        player:removeItem(info.id, info.num)
                    end
                end
                player:addMoney(money)
                player.team:addResource(teamResource)
                GameMatch:updateTeamResource()
                if player.team:isFullResource() then
                    if player.team.isResourceFull == false then
                        if JewelConfig.isResourceFull == false then
                            if JewelConfig.isDiscover then
                                MsgSender.sendMsg(Messages:teamSourceFull(player.team:getDisplayName()))
                            else
                                MsgSender.sendMsg(Messages:teamSourceFullWithJewel(player.team:getDisplayName()))
                                MsgSender.sendOtherTips(GameConfig.gameTime, Messages:JewelDiscover())
                            end
                        else
                            MsgSender.sendMsg(Messages:teamSourceFull(player.team:getDisplayName()))
                        end
                    end
                    player.team.isResourceFull = true
                    JewelConfig.isResourceFull = true
                    JewelConfig:showName()
                end
                player.isExchanged = true
            end
        else
            if player.isExchanged == true then
                player.isExchanged = false
            end
        end
    end

    for i, v in pairs(BlockConfig.area) do
        if v:inArea(VectorUtil.newVector3(x, y - 1, z)) == true then
            if player.isInMineArea == false then
                player.isInMineArea = true
                MsgSender.sendBottomTipsToTarget(player.rakssid, GameConfig.gameTipShowTime, Messages:enterMine())
            end
            return true
        end
    end
    player.isInMineArea = false
    return true
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta)
    for i, v in pairs(ItemConfig.noDrop) do
        if itemId == v.id then
            return false
        end
    end

    return true
end

function PlayerListener.onPlayerRespawn(player)
    if player.tempInventory ~= nil then
        for i, v in pairs(player.tempInventory) do
            player:addItem(v.id, v.num, v.meta)
        end
        player.tempInventory = {}
    end
    player:teleportPosWithYaw(player.team.initPos, player.team.yaw)
    return 43200
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerAttackActorNpc(rakssid, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local jewel = JewelConfig:getJewelByEntityId(entityId)
    if jewel ~= nil then
        EngineWorld:removeEntity(jewel.entityId)
        player:addItem(905, 1, 0) -- 905是宝石
        JewelConfig.generation[jewel.id] = nil
        JewelConfig.isDiscover = true
        player.jewelId = jewel.id
        player:changeClothes("custom_back_effect", 1)
        player:changeClothes("custom_foot_halo", 1)
        player:addScore(ScoreConfig.GET_JEWEL)
        local players = PlayerManager:getPlayers()
        for _, v in pairs(players) do
            if v.rakssid == rakssid then
                MsgSender.sendBottomTipsToTarget(v.rakssid, GameConfig.gameTipShowTime, Messages:placeStatue())
                MsgSender.sendOtherTipsToTarget(v.rakssid, GameConfig.gameTime, Messages:placeStatue())
            else
                MsgSender.sendBottomTipsToTarget(
                        v.rakssid,
                        GameConfig.gameTipShowTime,
                        Messages:playerGetJewel(player.team:getDisplayName(), player.name)
                )
                MsgSender.sendOtherTipsToTarget(
                        v.rakssid,
                        GameConfig.gameTime,
                        Messages:playerGetJewel(player.team:getDisplayName(), player.name)
                )
            end
        end
    end
end

function PlayerListener.onPlayerAttackSessionNpc(rakssid, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local statue = TeamConfig:getStatueByEntityId(entityId)
    if statue ~= nil then
        if player.team.id ~= statue.id then
            HostApi.sendManorOperationTip(player.rakssid, "gui_jewel_not_your_team_statue")
            return
        end

        if player.jewelId == 0 then
            HostApi.sendManorOperationTip(player.rakssid, "gui_jewel_no_have_jewel")
            return
        end

        if player.team.isResourceFull == false then
            HostApi.sendManorOperationTip(player.rakssid, "gui_jewel_fill_team_source")
            return
        end

        if GameMatch.isReward then
            return
        end

        player.team.isWin = true
        player:addScore(ScoreConfig.JEWEL_KNIGHT)
        player.knightCount = 1
        player.appIntegral = player.appIntegral + 3
        player:removeItem(905, 1)
        EngineWorld:removeEntity(entityId)
        local team = TeamConfig:getTeam(player.team.id)
        if team then
            player.team.statueEntityId = EngineWorld:addSessionNpc(team.statuePos, team.statueYaw, SessionNpcConfig.STATUE_NPC, team.statueActor, function(entity)
                entity:setNameLang(team.statueName or "")
                entity:setActorBodyId("2")
            end)
        end
        GameMatch:ifGameOverByPlayer()
        MsgSender.sendBottomTips(
                GameConfig.gameTipShowTime,
                Messages:playerWin(player.team:getDisplayName(), player.name)
        )
        MsgSender.sendOtherTips(GameConfig.gameTime, Messages:playerWin(player.team:getDisplayName(), player.name))
    end
end

function PlayerListener.onPlayerBuyUpgradeProp(rakssid, id, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local tool = MerchantConfig:getToolsById(id)
    if tool == nil then
        return
    end
    if player:upgradeTools() then
        player:sendToolsData()
    end
end

return PlayerListener
--endregion
