--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "data.GamePlayer"
require "Match"
require "messages.Messages"
require "config.ScoreConfig"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerPickupItemEvent.registerCallBack(self.onPlayerPickupItem)
    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerBuyGoodsEvent.registerCallBack(self.onPlayerBuyGoods)
    PlayerSkillTypeEvent.registerCallBack(self.onPlayerSkillType)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    local callback = function()
        if GameMatch.curStatus == GameMatch.Status.WaitingPlayer then
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
        end
    end
    return GameOverCode.Success, player, 0, callback
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    HostApi.sendPlaySound(player.rakssid, 10009)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    if GameMatch.isTeleScene then
        player:teleportScene()
    end
    return 43200
end

function PlayerListener.onPlayerMove()
    return true
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if GameMatch:isGameStart() == false then
        return false
    end

    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)

    if hurter == nil then
        return true
    end

    hurtValue.value = hurtValue.value * hurter.defenseCoefficient

    if hurtFrom == nil then
        if damageType ~= "magic" and damageType ~= "outOfWorld" then
            hurtValue.value = 0
        end
        local hp = hurter:getHealth()
        if hp - hurtValue.value <= 0 then
            hurter:onDie()
            GameMatch:ifGameOverByPlayer()
        end
        return true
    end
    if damageType == "explosion.player" or damageType == "explosion" then
        if hurter.role == GamePlayer.ROLE_SOLDIER then
            return false
        else
            local hp = hurter:getHealth()
            if hp - hurtValue.value <= 0 then
                hurtValue.value = hp - 1
            end
            return true
        end
    end

    local fromer = PlayerManager:getPlayerByPlayerMP(hurtFrom)

    if hurter == nil or fromer == nil or hurter.role == fromer.role then
        return false
    end

    if fromer.role == GamePlayer.ROLE_ZOMBIE then
        local zbConfig = GameMatch:getCurGameScene().zombieConfig[fromer.zombieId]

        if zbConfig then
            hurtValue.value = zbConfig.damage
            hurter:onPoison(zbConfig.poisonTime)
        end
    end

    return true
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    if diePlayer == nil then
        return
    end

    local isDeath = true

    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)

    if killPlayer == nil then
        if dier.isPoisonAttack and dier.role == GamePlayer.ROLE_SOLDIER then
            GameMatch:sendPlayerChangeTeam(dier)
            local zombie = PlayerManager:getPlayerByUserId(dier.poisonZombieUserId)
            if zombie ~= nil then
                zombie:onKill()
            end
            dier.role = GamePlayer.ROLE_ZOMBIE
            dier:onDieByPlayer()
            dier:convertToZombie()
            MsgSender.sendMsgToTarget(dier.rakssid, Messages:convertToZombie())
            dier.isPoisonAttack = false
            GameMatch:addTeamKills(GamePlayer.ROLE_ZOMBIE)
            GameMatch:ifGameOverByPlayer()
            return false
        end

        return isDeath
    end

    local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)

    if (dier ~= nil and killer ~= nil) then
        if dier.role == GamePlayer.ROLE_SOLDIER and killer.role == GamePlayer.ROLE_ZOMBIE then
            GameMatch:sendPlayerChangeTeam(dier)
            dier.role = GamePlayer.ROLE_ZOMBIE
            dier.isPoisonAttack = false
            dier:onDieByPlayer()
            dier:convertToZombie()
            MsgSender.sendMsgToTarget(dier.rakssid, Messages:convertToZombie())
            isDeath = false
        elseif dier.role == GamePlayer.ROLE_ZOMBIE then
            dier:onDie()
        end
        killer:onKill()
        GameMatch:addTeamKills(killer.role)
        GameMatch:ifGameOverByPlayer()
    end

    return isDeath
end

function PlayerListener.onPlayerRespawn(player)
    return 43200
end

function PlayerListener.onPlayerPickupItem(rakssid, itemId, itemNum, itemEntityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil and player.role == GamePlayer.ROLE_ZOMBIE then
        return false
    end
    return true
end

function PlayerListener.onPlayerBuyGoods(rakssid, type, itemId, msg, isAddItem)
    if GameMatch:isGameStart() == false then
        msg.value = IMessages:msgDontBuyGoodsByNoStart()
        return false
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)

    if player == nil then
        return false
    end

    if player.isInGame == false then
        msg.value = IMessages:msgDontBuyGoodsByNoStart()
        return false
    end

    if player.role == GamePlayer.ROLE_ZOMBIE then
        msg.value = Messages:dontBuyGoodsByZombie()
        return false
    end

    return true

end

function PlayerListener.onPlayerSkillType(rakssid, skillType, skillStatus)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil and player.role ~= GamePlayer.ROLE_ZOMBIE then
        return
    end

    local zombie = GameMatch:getCurGameScene():getZombieByZombieId(player.zombieId)
    if zombie.skillType ~= skillType then
        return
    end

    player.skillDurationTime = zombie.skillDurationTime

    if skillType == GamePlayer.SKILL_TYPE_DEFENSE then
        player:skillDefense(skillStatus)
    end

    if skillType == GamePlayer.SKILL_TYPE_SPRINT then
        player:skillSprint(skillStatus, zombie.skillSpeed, zombie.skillDurationTime, zombie.buff)
    end

    if skillType == GamePlayer.SKILL_TYPE_RELEASE_TOXIC then
        local range = tonumber(zombie.skillPoisonRange)
        local skillDurationTime = tonumber(zombie.skillDurationTime)
        local poisonAttackTime = tonumber(zombie.skillPoisonDurationTime)
        local damage = tonumber(zombie.skillPoisonDamage)
        local poisonOnTime = tonumber(zombie.skillPoisonOnTime)
        local color = VectorUtil.newVector3(zombie.skillPoisonColor.b, zombie.skillPoisonColor.g, zombie.skillPoisonColor.r)
        player:playSkillEffect("mobSpell", skillDurationTime, range, color)
        player:skillReleaseToxic(range, poisonAttackTime, damage, poisonOnTime)
    end

    HostApi.setPlayerKnockBackCoefficient(player.rakssid, zombie.knockBackCoefficient)
end

return PlayerListener
--endregion
