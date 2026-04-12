--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "data.GameJobPond"
require "Match"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)
    BaseListener.registerCallBack(PlayerDieTipEvent, self.onPlayerDiedTip)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerReviewPlayerEvent.registerCallBack(self.onPlayerReviewPlayer)
    PlayerBuyRespawnResultEvent.registerCallBack(self.onBuyRespawnResult)
    PlayerChangeItemInHandEvent.registerCallBack(self.onPlayerChangeItemInHand)
    PlayerSelectJobEvent:registerCallBack(self.onSelectJob)
    PlayerRefreshJobListInfoEvent:registerCallBack(self.RefreshJobListInfo)
    PlayerExtractJobEvent:registerCallBack(self.onPlayerExtractJob)  --玩家抽取职业的事件

    PlayerBackHallEvent:registerCallBack(self.onPlayerBackHall)
    PlayerNextGameEvent:registerCallBack(self.onPlayerNextGame)

    PlayerArmorChangeEvent.registerCallBack(self.onPlayerArmorChange)

    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerPickupItemEvent.registerCallBack(self.onPlayerPickupItem)
    PlayerPickupItemEndEvent.registerCallBack(self.onPlayerPickupItemEnd)

    CustomTipMsgEvent.registerCallBack(self.onCustomTipMsg)
    PlayerAbandonItemEvent:registerCallBack(self.onPlayerAbandonItem)
    PlayerUseCommonItemEvent.registerCallBack(self.onPlayerUseCommonItem)
    ItemSkillNeedCustomSpeedEvent.registerCallBack(self.onItemSkillNeedCustomSpeed)
    PlayerCatchFishHookEvent.registerCallBack(self.onPlayerCatchFishHook)

    CommonReceiveRewardEvent:registerCallBack(self.onCommonReceiveReward)
end

function PlayerListener.onPlayerLogin(clientPeer)
    if not GameMatch:ifPlayerCanLogin() then
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
    DbUtil:getPlayerData(player)
    GameMatch:sendAllPlayerTeamInfo()
    GameMatch:updateGameInfo()
    return 0
end

function PlayerListener.onPlayerRespawn(player)
    GameMatch:setName()
    player.team:getTeleportPos(player)
    player.damageType = ""
    return 43200
end

function PlayerListener.onBuyRespawnResult(rakssid, code)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if code == 0 or not GameMatch:isGameRunning() then
        player:onDie()
        return
    end
    if code == 1 then
        GamePacketSender:sendHideOrShowToolBar(rakssid, true)
        player.isLife = true
        player.damageType = ""
        local players = PlayerManager:getPlayers()
        for _, v in pairs(players) do
            GamePacketSender:sendGeneralDeathTip(v.rakssid, GameMatch:nameAndTeamId(player), "lucky.sky.revived", TextFormat.colorAqua)
        end
        MsgSender.sendMsg(IMessages:msgRespawn(GameMatch:nameAndTeamId(player)))
    end
end

function PlayerListener.onPlayerMove(Player, x, y, z)
    local player = PlayerManager:getPlayerByPlayerMP(Player)
    if player == nil then
        return false
    end

    local pos = VectorUtil.newVector3(x, y, z)

    if player.canChangeIce and next(player.iceArea) ~= nil then
        player.iceArea:changeIce(player, pos)
    end

    if y <= GameConfig.deadLineHeight and GameMatch:isGameRunning() and not player.useHollowBlock then
        player:canUseHollowBlock()
    end

    if y >= GameConfig.startGlideHeight and GameMatch:isGameRunning() and player.useHollowBlock then
        player:setGlide(true)
        player.useHollowBlock = false
    end
    return true
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    if hurter == nil then
        return false
    end

    if damageType == "inWall" then
        return false
    end

    if damageType == "fall" and hurter.noFallHurt then
        hurter.noFallHurt = false
        return false
    end

    hurter:onHurt(hurtValue.value)

    if damageType == "outOfWorld" then
        hurtValue.value = 9999
        hurtValue.value = hurter:subApple(hurtValue.value)
        hurter.damageType = damageType
        return not hurter.useHollowBlock
    end

    if not GameMatch:isGameRunning() then
        return false
    end

    if damageType == "fall" then
        hurtValue.value = hurter:subApple(hurtValue.value)
        return not hurter.useHollowBlock
    end

    if damageType == "explosion.player" or damageType == "explosion" then
        hurtValue.value = hurter:subApple(hurtValue.value)
        return true
    end

    local attacker = PlayerManager:getPlayerByPlayerMP(hurtFrom)
    if attacker ~= nil then
        if hurter.killByDevil == true then
            return false
        end
        --队友之间无伤害
        if hurter:getTeamId() == attacker:getTeamId() then
            return false
        end

        if damageType == "player" or damageType == "player.gun" or damageType == "player.gun.headshot" or damageType == "arrow" then
            hurter:propertyBeAttack(attacker, hurtValue)
            if attacker:getHeldItemId() ~= 0 and attacker.rakssid ~= hurter.rakssid then

                hurter:updateHurtInfo(ReportUtil.DeadType.Item, attacker.curUseItem.id,
                        attacker.curUseItem.meta, attacker.userId)

                hurtValue.value = ItemConfig:getItemDamage(attacker.curUseItem, hurtValue.value)
            else
                hurter:updateHurtInfo(ReportUtil.DeadType.Hand, 0, 0, attacker.userId)

                attacker:propertyOnHit(hurter, hurtValue.value)
            end
        end

        if damageType == "thrown" then
            local id = ItemID.SNOWBALL
            local meta = 0
            if attacker.curUseItem.id == ItemID.FISHING_ROD then
                id = attacker.curUseItem.id
                meta = attacker.curUseItem.meta
            end

            hurter:updateHurtInfo(ReportUtil.DeadType.Item, id, meta, attacker.userId)

            hurtValue.value = ItemConfig.snowBallDamage
        end

        attacker:propertyOnUse(hurter, hurtValue)

        if attacker.isHaveTnt and hurter:getItemNumById(Define.HotPotato) == 0 then
            if hurter:getInventory():isCanAddItem(Define.HotPotato, 0, 1) then
                hurter:becomeTntPlayer(attacker.hotPotato)
                attacker:becomeNormalPlayer()
            else
                MsgSender.sendBottomTipsToTarget(attacker.rakssid, 5, Messages:msgHotPotatoInvFull())
            end
        end

        attacker:onAttack(hurtValue.value)
    end

    hurtValue.value = hurter:subApple(hurtValue.value)

    if hurter:onMountHunt(hurtValue.value) then
        hurtValue.value = 0
    end
    return true
end

function PlayerListener.onPlayerDiedTip(diePlayer, killPlayer)
    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if dier == nil then
        return true
    end
    if not GameMatch:isGameRunning() then
        return true
    end
    --local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
    local attacker = PlayerManager:getPlayerByUserId(dier.hurtInfo.attacker)
    if (iskillByPlayer and attacker ~= nil and dier ~= nil) then
        if (attacker ~= nil) then
            if dier:isCountKill() then
                MsgSender.sendMsg(Messages:msgKillTip(GameMatch:nameAndTeamId(attacker), GameMatch:nameAndTeamId(dier)))
                attacker:onKill(dier)
            else
                GameMatch:sendDieMsgWithoutKill(dier.damageType == "outOfWorld", GameMatch:nameAndTeamId(dier))
            end
        end
    elseif dier.hurtInfo ~= nil and os.time() - dier.hurtInfo.time <= 10 then
        if attacker ~= nil then
            if attacker.rakssid ~= dier.rakssid then
                MsgSender.sendMsg(Messages:msgKillTip(GameMatch:nameAndTeamId(attacker), GameMatch:nameAndTeamId(dier)))
                attacker:onKill(dier)
            else
                GameMatch:sendDieMsgWithoutKill(dier.damageType == "outOfWorld", GameMatch:nameAndTeamId(dier))
            end
        else
            GameMatch:sendDieMsgWithoutKill(false, GameMatch:nameAndTeamId(dier))

        end
    else
        GameMatch:sendDieMsgWithoutKill(dier.damageType == "outOfWorld", GameMatch:nameAndTeamId(dier))

    end
    if dier.hurtInfo ~= nil and os.time() - dier.hurtInfo.time <= 10 then
        ReportUtil:addDeadType(ReportUtil:getKeyByDeadType(dier.hurtInfo.type, dier.hurtInfo.id, dier.hurtInfo.meta))
    end
    ReportUtil:addDeadType(ReportUtil:getKeyByDeadType(ReportUtil.DeadType.Other))

    dier:takeOffMount()
    dier.damageType = ""
    GameMatch:onPlayerDie(dier)
    return true
end

function PlayerListener.onPlayerReviewPlayer(rakssid1, rakssid2)
    local player1 = PlayerManager:getPlayerByRakssid(rakssid1)
    local player2 = PlayerManager:getPlayerByRakssid(rakssid2)
    if player1 == nil or player2 == nil then
        return
    end
    if player1:getTeamId() ~= player2:getTeamId() then
        player1.entityPlayerMP:changeNamePerspective(player2.rakssid, false)
        player1:setShowName(GameMatch:buildShowName(player1, false), player2.rakssid)
    else
        player1.entityPlayerMP:changeNamePerspective(player2.rakssid, false)
        player1:setShowName(GameMatch:buildShowName(player1, true), player2.rakssid)
    end
end

function PlayerListener.onPlayerChangeItemInHand(rakssid, itemId, itemMeta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end
    if player.isHaveTnt and itemId ~= Define.HotPotato then
        player:holdHotPotato(player.hotPotato.id, player.hotPotato.meta)
    end
    player:onEquipItemChange(itemId, itemMeta)
end

--当玩家选择职业的
function PlayerListener.onSelectJob(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local jobId = DataBuilder.new():fromData(data):getNumberParam("JobId")
    player:setCurJobId(jobId)
    GameMatch:setName()
end
--当玩家刷新职业
function PlayerListener.RefreshJobListInfo(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local price = DataBuilder.new():fromData(data):getNumberParam("price")
    PayHelper.payDiamonds(userId, 100, price, function(player)
        player.Jobs_Pond = {}
        GameJobPond:randomAllJobs(player)
    end)
end

--玩家抽取职业的事件
function PlayerListener.onPlayerExtractJob(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    --价格
    local price = DataBuilder.new():fromData(data):getNumberParam("price")
    PayHelper.payDiamonds(userId, 100, price, function(player)
        player.choiceJob = player.choiceJob + 1
        GameJobPond:onPlayerExtractJob(player)
    end)
end

function PlayerListener.onPlayerPickupItem(rakssid, itemId, itemNum)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if itemId == Define.HotPotato and player:getItemNumById(itemId) > 0 then
        return false
    end
    return true
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta, itemCount)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if itemId == Define.HotPotato then
        return false
    end
    return true
end

function PlayerListener.onPlayerPickupItemEnd(rakssid, itemId, itemNum, meta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if GunSetting.isGunItem(itemId) then
        return true
    end
    player:removeItem(itemId, itemNum, meta)
    player:addGameItem(itemId, itemNum, meta)
    return true
end

function PlayerListener.onPlayerArmorChange(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local ArmorInv = InventoryUtil:getPlayerArmorInventoryInfo(player)
    local armors = {}
    for _, item in pairs(ArmorInv) do
        local type = tonumber(item.stackIndex) - 100
        armors[type] = {
            id = item.id,
            meta = item.meta
        }
        if player.curArmor[type] == nil
                or player.curArmor[type] == 0
                or player.curArmor[type] ~= item.id then
            player:onEquipArmorChange(item.id, item.meta, type)
        end
    end

    if player.curArmor == nil then
        return
    end

    for type, item in pairs(player.curArmor) do
        if armors[type] == nil and item.id ~= 0 then
            player:onEquipArmorChange(0, 0, type)
        end
    end
end

function PlayerListener.onCustomTipMsg(rakssid, extra, isRight)
    if isRight == false then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if extra == "UseDevilPact" then
        player.signDevilPack = true
        SkillManager:createSkill(player.devilPactInfo.config, player, player.devilPactInfo.position)
        return
    end
end

function PlayerListener.onPlayerAbandonItem(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local builder = DataBuilder.new():fromData(data)
    local selectIndex = builder:getNumberParam("selectIndex")
    local num = builder:getNumberParam("num")
    local pos = player:getPosition()
    local inv = player:getInventory()
    local info = inv:getItemStackInfo(selectIndex)

    if num >= 1 and num <= 64 then
        if inv then
            if info and info.num >= num then
                player:decrStackSize(selectIndex, num)
                HostApi.refreshSellItemSuc(player.rakssid)
            end
        end
    end
    EngineWorld:addEntityItem(info.id, num, info.meta, 10000, VectorUtil.newVector3(pos.x - 2, pos.y, pos.z - 1))
end

function PlayerListener.onPlayerUseCommonItem(rakssId, itemId)

    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return
    end

    if itemId == PotionItemID.POTION_NIGHT_VISION then
        player:useInvulnerabilityPotion()
    end
end

function PlayerListener.onItemSkillNeedCustomSpeed(rakssid, itemId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return true
    end

    if ItemID.FLARE_BOMB == itemId then
        return false
    end

    return true
end

function PlayerListener.onPlayerCatchFishHook(anglerId, targetId)
    local angler = PlayerManager:getPlayerByEntityId(anglerId)
    if not angler then
        return
    end
    local target = PlayerManager:getPlayerByEntityId(targetId)
    if not target then
        return
    end
    StealProperty:onPlayerUsed(angler, target)
end

function PlayerListener.onPlayerBackHall(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    BackHallManager:addPlayer(player)
end

function PlayerListener.onPlayerNextGame(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    EngineUtil.sendEnterOtherGame(player.rakssid, GameMatch.gameType, player.userId, player.cur_map_id)
end

function PlayerListener.onCommonReceiveReward(player, propsType, propsId, propsCount, expireTime)
    if propsType == "Job" then
        player.GameJob:updateJobUIData(tonumber(propsId), tonumber(propsCount), true)
    end
end

return PlayerListener
--endregion
