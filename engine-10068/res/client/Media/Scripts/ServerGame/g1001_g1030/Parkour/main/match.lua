
local define = require "define"
local config = require "config"
local timer = require "timer"
local rank = require "rank"

local match = {}

function match:init()
    EngineWorld:stopWorldTime()
    
    self._stage = define.MATCH_STAGE_NONE
    self._site_idx = nil
    self._spawn_timers = {}
    self._game_timer = nil
    self._save_timer = nil
    self._close_timer = nil

end

function match:site_conf()
    return assert(config.sites[self._site_idx])
end

function match:site_name()
    local conf = self:site_conf()
    return conf.name
end

local function register_spawn_potions_timer(self)
    local conf = self:site_conf()

    for _, info in ipairs(conf.potions) do
        local potion = assert(config.potions[info.potion])
        local tid = timer:register(info.spawnTime, function()
            for _, pos in ipairs(info.list) do
                EngineWorld:addEntityItem(
                    potion.item,
                    1,
                    0,
                    potion.lifeTime,
                    VectorUtil.newVector3(unpack(pos))
                )
            end
            return info.spawnTime
        end)

        table.insert(self._spawn_timers, tid)
    end
end

local function register_save_timer(self)
    timer:close(self._save_timer)
end

local function register_close_timer(self)
    timer:close(self._close_timer)
    self._close_timer = timer:register(30, function()
        -- do nothing
        return 0
    end
    )
end

local function reset(self)    
    -- clear
    do
        timer:close(self._game_timer)
        for _, tid in ipairs(self._spawn_timers) do
            timer:close(tid)
        end
        self._spawn_timers = {}
        self._stage = define.MATCH_STAGE_NONE

    end

    -- prepare
    do
        self._site_idx = math.random(1, #config.sites)
        register_spawn_potions_timer(self)
    end
end

function match:on_open()
    rank:request_rank()
    register_save_timer(self)
    register_close_timer(self)

    reset(self)
    self:step_wait_stage()
end

function match:on_close()
    GameServer:closeServer()
end

function match:stage()
    return self._stage
end

function match:on_player_login(player)
    player:on_login()
    if self._close_timer then
        timer:close(self._close_timer)
        self._close_timer = nil
    end
end

function match:on_player_logout(player)
    player:on_logout()
    if self:player_count() == 0 then
        self:force_reset_game()
        register_close_timer(self)
    end
end

function match:player_count()
    return PlayerManager:getPlayerCount()
end

function match:player_enough()
    return self:player_count() >= config.playerMin
end

---------------------------------------------------------------------------
function match:step_wait_stage()
    assert(self._stage == define.MATCH_STAGE_NONE)
    self._stage = define.MATCH_STAGE_WAIT
    HostApi.sendResetGame(self:player_count())

    local tick = 0
    local prestartPlayerCount = 0
    self._game_timer = timer:register(1, function()
        tick = tick + 1
        if tick >= config.waitTime - 12 and prestartPlayerCount == 0 and self:player_count() >= config.playerMin then
            prestartPlayerCount = self:player_count()
            HostApi.sendStartGame(prestartPlayerCount)
        end
        if tick >= config.waitTime then
            if self:player_count() >= config.playerMin then
                self:step_ready_stage()
                return 0
            else
                tick = 0
                prestartPlayerCount = 0
                HostApi.sendResetGame(self:player_count())
                return 1
            end
        else
            MsgSender.sendBottomTips(2, IMessages:msgWaitPlayerEnough(config.waitTime - tick, IMessages.UNIT_SECOND, false))
            return 1
        end
    end)
end

function match:step_ready_stage()
    assert(self._stage == define.MATCH_STAGE_WAIT)
    self._stage = define.MATCH_STAGE_READY
    HostApi.sendStartGame(self:player_count())

    local conf = self:site_conf()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if not player:check_ready() then
            HostApi.sendGameover(player.rakssid, IMessages:msgGameHasStarting(), GameOverCode.GameStarted)
        else
            player:set_retire_pos(unpack(conf.initPoint))
            player:retire()
        end
    end

    local tick = 0
    self._game_timer = timer:register(1, function()
        tick = tick + 1
        if tick >= config.readyTime then
            self:step_start_stage()
            return 0
        end
        
        MsgSender.sendBottomTips(2, IMessages:msgGameStartTimeHint(config.readyTime - tick, IMessages.UNIT_SECOND, false))
        return 1
    end)
end

function match:step_start_stage()
    assert(self._stage == define.MATCH_STAGE_READY)
    self._stage = define.MATCH_STAGE_START
    RewardManager:startGame()
    HostApi.syncGameTimeShowUi(0, true, config.gameTime)

    self._game_timer = timer:register(config.gameTime, function()
        self:step_close_stage()
        return 0
    end)
end

function match:sort_players()
    local players = PlayerManager:copyPlayers()
    table.sort(players, function(p1, p2)
        local s1, s2 = p1:get_score(), p2:get_score()
        if s1 ~= s2 then
            return s1 > s2
        else
            local t1, t2 = p1:get_score_time(), p2:get_score_time()
            return t1 < t2
        end
    end)
    return players
end

function match:step_close_stage()
    assert(self._stage == define.MATCH_STAGE_START)
    self._stage = define.MATCH_STAGE_CLOSE

    local players = self:sort_players()

    do
        local func = function()
            do
                local function newSettlementItem(player, rank, win)
                    local adSwitch_result = player.adSwitch or 0
                    if player.gold <= 0 then
                        adSwitch_result = 0
                    end
                    
                    return {
                        name = player.name,
                        rank = rank,
                        points = player.scorePoints,
                        gold = player.gold,
                        available = player.available,
                        hasGet = player.hasGet,
                        vip = player.vip,
                        kills = player:get_score(),
                        isWin = win and 1 or 0,
                        adSwitch = adSwitch_result,
                    }
                end

                local players = self:sort_players()
                local statistic = {}
                for rank, player in ipairs(players) do
                    if rank == 1 then
                        ReportManager:reportUserWin(player.userId)
                    end
                    table.insert(statistic, newSettlementItem(player, rank, rank == 1))
                end

                for rank, player in ipairs(players) do
                    local result = {
                        players = statistic,
                        own = statistic[rank]
                    }
                    HostApi.sendGameSettlement(player.rakssid, json.encode(result), false)
                end
            end

            do
                HostApi.syncGameTimeShowUi(0, false, 0)
                reset(self)
                local players = PlayerManager:getPlayers()
                for _, player in pairs(players) do
                    player:reset(config.initPos)
                end
                self:step_wait_stage()
            end

        end

        for rank, player in ipairs(players) do
            player:reward(rank, func)
        end
    end

    self:request_rank_data(self:site_name())

end

function match:force_reset_game()
    HostApi.syncGameTimeShowUi(0, false, 0)
    reset(self)
    self:step_wait_stage()
end

function match:request_rank_data(site)
    rank:request_rank(site)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:request_rank(site)
    end
end

function match:is_all_player_reward_end()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player._is_reward == false then
            return false
        end
    end
    return true
end

return match
