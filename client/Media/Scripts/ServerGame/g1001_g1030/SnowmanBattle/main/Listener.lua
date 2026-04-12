require "Magic"

require "config.GameConfig"
require "Match"
require "RankNpc"
require "Shop"
require "Timer"
require "Player"

Listener = { }
Listener.hitHP = { }

function Listener:init()
    -- game
    BaseListener.registerCallBack(GameInitEvent, self.onGameInit)

    -- block
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)

    -- Logic
    EntityHitEvent.registerCallBack(self.onEntityHit)
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)

    -- Player
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerUseThrowableItemEvent.registerCallBack(self.onUseThrowableItem)
    PlayerPickupItemEvent.registerCallBack(self.onPlayerPickupItem)
    PlayerBuyUpgradePropEvent.registerCallBack(self.onPlayerBuyUpgradeProp)
    PlayerUpdateRealTimeRankEvent.registerCallBack(self.onPlayerUpdateRealTimeRank)
    PotionConvertGlassBottleEvent.registerCallBack(function(entityId, itemId)
        return false
    end)
    PlayerAttackCreatureEvent.registerCallBack(self.onPlayerAttackCreature)
    PlayerUseCommonItemEvent.registerCallBack(self.onPlayerUseCommonItem)
    -- Redis
    ZScoreFromRedisDBEvent.registerCallBack(self.onZScoreFromRedisDB)
    ZRangeFromRedisDBEvent.registerCallBack(self.onZRangeFromRedisDB)

    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)

    BaseMain:setGameType(Match.gameType)
    HostApi.setNeedFoodStats(false)
    HostApi.setFoodHeal(false)
end

function Listener.onGameInit(config)
    HostApi.startRedisDBService()
    GameConfig:init(config)

    Timer:init()
    Match:init()
    RankNpc:init(GameConfig.ranks)
    RankNpc:setZExpireat()

    Shop:init()

    local initPos = VectorUtil.newVector3i(GameConfig.teams[1].waitPos[1],
            GameConfig.teams[1].waitPos[2], GameConfig.teams[1].waitPos[3])
    GameServer:setInitPos(initPos)

    Match:on_open()
end
function Listener.onBlockBreak(rakssid, blockId, vec3)
    return GameConfig.enableBreak
end
function Listener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    return true
end

function Listener.onPlayerUseCommonItem(rakssId, itemId)
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if player == nil then
        return
    end


    --    if itemId == 468 then
    --        local pos = player:getPosition()
    --        Magic:buildWall(player:getYaw(), VectorUtil.toBlockVector3(pos.x, pos.y, pos.z))
    --    end

    if itemId == 469 then
        local pos = player:getPosition()
        Magic:buildFloor(player:getYaw(), VectorUtil.toBlockVector3(pos.x, pos.y - 1, pos.z))
    end
end

function Listener.onEntityHit(entityId, vec3, throwerId)
    if Match.stage == Match.MATCH_STAGE_NONE then
        return
    end

    if entityId ~= 332 then
        return
    end

    local player = PlayerManager:getPlayerByEntityId(throwerId)
    if player ~= nil then

        local ball = SnowBallConfig:getSnowballByLevel(player.snowball_lv)

        local pos = VectorUtil.newVector3i(vec3.x, vec3.y, vec3.z)
        local blockId = EngineWorld:getBlockId(pos)

        local hashKey = VectorUtil.hashVector3(vec3)
        local hp = Listener.hitHP[hashKey]
        --        print(blockId.. ":" .. vec3.x .. "-" .. vec3.y .. "-" .. vec3.z)
        if hp == nil then
            for k, v in pairs(GameConfig.obstacle) do
                if blockId == v.id then
                    Listener.hitHP[hashKey] = v.hp - ball.damageToF
                    if Listener.hitHP[hashKey] <= 0 then
                        EngineWorld:setBlockToAir(pos)
                        Listener.hitHP[hashKey] = nil
                    end
                end
            end
        else
            if blockId ~= 0 then
                Listener.hitHP[hashKey] = hp - ball.damageToF
                if Listener.hitHP[hashKey] <= 0 then
                    EngineWorld:setBlockToAir(pos)
                    Listener.hitHP[hashKey] = nil
                end
            end
        end
    end
end
function Listener.onEntityItemSpawn(itemId, itemMeta, behavior)
    if behavior == "break.block" then
        return false
    end
    return true
end
function Listener.onPlayerAttackCreature(rakssid, entityId, attackType)
    if attackType ~= 0 and Match.stage ~= Match.MATCH_STAGE_CLOSE then
        local entity;
        entity = Match:getSnowMan(entityId)
        if entity == nil then
            entity = Match:getFlag(entityId)
        end
        --        print("onPlayerAttackCreature:" .. attackType)
        if entity ~= nil then
            local fromer = PlayerManager:getPlayerByRakssid(rakssid)
            if fromer ~= nil then
                if entity.teamId ~= fromer.dynamicAttr.team then
                    local ball = SnowBallConfig:getSnowballByLevel(fromer.snowball_lv)
                    local damage = ball.damageToSF
                    entity:attackedByPlayer(rakssid, damage)
                    return true
                end
            end
        end
    end
    return false
end

-- EntityPlayerMP
function Listener.onPlayerLogin(clientPeer)
    if Match.stage ~= Match.MATCH_STAGE_WAIT then
        return GameOverCode.GameStarted
    end
    local player = Player.new(clientPeer)
    if player.dynamicAttr.team < 1 or player.dynamicAttr.team > 2 then
        HostApi.log("AllocTeamError:" .. "team:" .. player.dynamicAttr.team)
        return GameOverCode.NoThisTeam
    end
    player:init()
    Match:on_player_login(player);
    return GameOverCode.Success, player
end

function Listener.onPlayerLogout(player)
    Match:on_player_logout(player)
end

function Listener.onPlayerReady(player)
    player.is_death = false
    player:retire()
    RankNpc:getPlayerRankData(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(TextFormat:getTipType(55)))
    return 43200
end

function Listener.onPlayerMove(player, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return true
    end
    local p = PlayerManager:getPlayerByPlayerMP(player)
    if p ~= nil then
        p:on_move(x, y, z)
        return true
    end
    return false
end
function Listener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if Match.stage ~= Match.MATCH_STAGE_CLOSE then
        if damageType == "outOfWorld" then
            return true
        end
        local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
        local fromer = PlayerManager:getPlayerByPlayerMP(hurtFrom)
        if hurter == nil or fromer == nil then
            return false
        end
        if hurter.dynamicAttr.team == fromer.dynamicAttr.team then
            return false
        end
        if damageType == "thrown" then
            local ball = SnowBallConfig:getSnowballByLevel(fromer.snowball_lv)
            hurtValue.value = ball.damageToP
            fromer:addCurrency(GameConfig.attackRewradMoney)
            fromer.totalCurrency = fromer.totalCurrency + GameConfig.attackRewradMoney
        else
            hurtValue.value = 0
        end
    else
        hurtValue.value = 0
    end
    return true
end
function Listener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    if Match.stage ~= Match.MATCH_STAGE_CLOSE then
        local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)

        local killer

        if killPlayer ~= nil then
            killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
        end

        if killer ~= nil and dier ~= nil and iskillByPlayer then
            killer:addCurrency(GameConfig.killRewradMoney)
            killer.totalCurrency = killer.totalCurrency + GameConfig.killRewradMoney
            killer.kills = killer.kills + 1
            killer.appIntegral = killer.appIntegral + 1
        end

        if Match:is_team_all_death(1) then
            print("1team_all_death")
            Match:step_close_stage(1, 1)
            return true
        elseif Match:is_team_all_death(2) then
            print("2team_all_death")
            Match:step_close_stage(1, 2)
            return true
        end

        if (dier ~= nil) then
            dier.is_death = true
            local inv = dier:getInventory()
            for k, v in pairs(GameConfig.ObstacleCom) do
                if v.ItemID ~= nil then
                    local count = inv:getItemNum(v.ItemID)
                    dier.respawnObstacle[k] = { v.ItemID, count }
                end
            end

            local count469 = inv:getItemNum(469)
            if count469 > 0 then
                dier.respawnObstacle["469"] = { 469, count469 }
            end

            dier:clearInv()

            dier:on_death()

            --        if Match.stage < 2 and Match.stage > 4 then
            --            HostApi.sendGameover(diePlayer:getRaknetID(), IMessages:msgGameOverDeath(), GameOverCode.GameOver)
            --            Match:onPlayerQuit(dier)
            --        end
        end
    end
    return true
end
function Listener.onUseThrowableItem(itemId)
    if Match.stage >= 2 and Match.stage <= 4 then
        return true
    end
    return false
end

function Listener.onPlayerRespawn(player)
    player.is_death = false
    player:set_retire_pos(unpack(GameConfig.teams[player.dynamicAttr.team].initPos))
    player:retire()
    for k, v in pairs(player.respawnObstacle) do
        player:AddItem(v[1], v[2], 0)
    end
    player.respawnObstacle = { }
    return 43200
end
function Listener.onPlayerPickupItem(rakssid, itemId, itemNum, itemEntityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if (itemId == 373) then
        if (player.is_death == false) then
            player:removeItem(itemId, itemNum)
            player:addEffect(6, 2, 1)
            EngineWorld:removeEntity(itemEntityId)
        end
    end
end
function Listener.onPlayerBuyUpgradeProp(rakssid, uniqueId, msg)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if (player.is_death == true) then
        return
    end
    local id = (tonumber(uniqueId) or 0)
    if id > 100 then
        player:buyObstacle(uniqueId)
    else
        local ball = SnowBallConfig:getSnowballByLevel(uniqueId)
        if ball == nil then
            return
        end
        if player:upgradeBall(ball.id) then
            player:sentUpgradeBallData()
        end
    end
end

function Listener.onPlayerUpdateRealTimeRank(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    Match:sendRealRankData(player)
end

function Listener.onZScoreFromRedisDB(key, userId, score, rank)

    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if RankNpc:getWeekKey() == key then
        player:setWeekRank(rank, score)
        return
    end

    player:setDayRank(rank, score)
end

function Listener.onZRangeFromRedisDB(key, data)
    local ranks = StringUtil.split(data, "#")

    if RankNpc:getWeekKey() == key then
        RankNpc.RankData.week = { }
        for i, v in pairs(ranks) do
            local rank = { }
            local item = StringUtil.split(v, ":")
            rank.rank = i
            rank.userId = tonumber(item[1])
            rank.score = tonumber(item[2])
            rank.name = "Steve" .. rank.rank
            rank.vip = 0
            if rank.score ~= 0 then
                RankNpc:addWeekRank(rank)
            end
        end
        RankNpc:rebuildRank(RankNpc.TYPE_WEEK)
        return
    end

    RankNpc.RankData.day = { }
    for i, v in pairs(ranks) do
        local rank = { }
        local item = StringUtil.split(v, ":")
        rank.rank = i
        rank.userId = tonumber(item[1])
        rank.score = tonumber(item[2])
        rank.name = "Steve" .. rank.rank
        rank.vip = 0
        if rank.score ~= 0 then
            RankNpc:addDayRank(rank)
        end
    end
    RankNpc:rebuildRank(RankNpc.TYPE_DAY)
end

function Listener.onBlockTNTExplode(entityId, vec3, attr)

    local player = PlayerManager:getPlayerByEntityId(entityId)

    local function checkInRegion(region, x, y, z)
        if x < region.min[1] or x > region.max[1] then
            return false
        end

        if y < region.min[2] or y > region.max[2] then
            return false
        end

        if z < region.min[3] or z > region.max[3] then
            return false
        end

        return true
    end

    local isSameTeam = false
    if player ~= nil then
        isSameTeam = checkInRegion(GameConfig.teams[player.dynamicAttr.team].area, vec3.x, vec3.y, vec3.z);
    end

    attr.isCanHurt = false;
    if (isSameTeam) then
        attr.isBreakBlock = false
    else
        attr.isBreakBlock = true
    end

    return false
end
return Listener