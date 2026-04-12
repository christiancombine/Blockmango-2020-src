
local define = require "define"
local config = require "config"
local match = require "match"
local rank = require "rank"
local shop = require "shop"
local timer = require "timer"
local utils = require "utils"
local message = require "message"

local player_class = require "player"

local listener = {}

function listener:init()
    -- game
    BaseListener.registerCallBack(GameInitEvent, function(c_config)
        HostApi.startRedisDBService()
        config:init(c_config)
        local initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
        GameServer:setInitPos(initPos)
        timer:init()
        match:init()
        rank:init()
        shop:init()
        match:on_open()
    end)

    -- block
    BlockBreakEvent.registerCallBack(function(rakssid, blockId, vec3)
        return false
    end)


    -- player
    PlayerPlaceBlockEvent.registerCallBack(function(rakssid, itemId, meta, toPlacePos)
        return false
    end)

    BaseListener.registerCallBack(PlayerLoginEvent, function(clientPeer)
        if match:stage() ~= define.MATCH_STAGE_WAIT then
            return GameOverCode.GameStarted
        end
        local player = player_class.new(clientPeer)
        player:init()
        match:on_player_login(player)
        return GameOverCode.Success, player, 1
    end)

    BaseListener.registerCallBack(PlayerLogoutEvent, function(player)
        match:on_player_logout(player)
    end)

    BaseListener.registerCallBack(PlayerReadyEvent, function(player)
        HostApi.sendPlaySound(player.rakssid, 10029)
        MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(message:gamename()))
        player:on_spawn()
        player:request_db_data()
        return 43200
    end)

    BaseListener.registerCallBack(PlayerDieEvent, function(diePlayer, iskillByPlayer, killPlayer)
        local player = PlayerManager:getPlayerByPlayerMP(diePlayer)
        if not player then
            return true
        end

        player:on_death()
        return true
    end)

    BaseListener.registerCallBack(PlayerAttackedEvent, function(hurtPlayer, hurtFrom, damageType, hurtValue)
        if damageType == "fall" then
            return false
        end

        return not hurtFrom
    end)

    BaseListener.registerCallBack(PlayerRespawnEvent, function(player)
        player:on_spawn()
        player:retire()
        return 43200
    end)

    PlayerMoveEvent.registerCallBack(function(pPlayer, x, y, z)
        if x == 0 and y == 0 and z == 0 then
            return true -- ��ת��ͷ
        end

        local player = PlayerManager:getPlayerByPlayerMP(pPlayer)
        if not player then
            return true
        end

        if player:isWatchMode() then
            return false
        end

        return player:on_move(x, y, z)
    end)


    PlayerPickupItemEndEvent.registerCallBack(function(rakssid, itemId, itemNum)
        local player = PlayerManager:getPlayerByRakssid(rakssid)
        if not player then
            return
        end

        player:removeItem(itemId, itemNum)
        player:on_user_item(itemId)
    end
    )

    EntityItemSpawnEvent.registerCallBack(function(itemId, itemMeta, behavior)
        return false
    end)

    PotionConvertGlassBottleEvent.registerCallBack(function(entityId, itemId)
        return false
    end)

    -- rank
    ZScoreFromRedisDBEvent.registerCallBack(function(key, userId, score, rank)
        local player = PlayerManager:getPlayerByUserId(userId)
        if not player then
            return
        end

        player:receive_rank(key, rank, score)
    end)

    ZRangeFromRedisDBEvent.registerCallBack(function(key, data)
        rank:receive_rank(key, data)
    end)

    BaseListener.registerCallBack(GetDataFromDBEvent, function(player, role, data)
        player:receive_db_data(data)
    end)
    DBManager:setPlayerDBSaveFunction(function(player, immediate)
        player:save_db_data(immediate)
    end)
end

return listener
