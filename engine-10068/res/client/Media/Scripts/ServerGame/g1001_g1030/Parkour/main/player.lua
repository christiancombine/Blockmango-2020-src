
local config = require "config"
local define = require "define"
local utils = require "utils"
local match = require "match"
local timer = require "timer"
local rank = require "rank"

local player = class("GamePlayer", BasePlayer)

function player:init()

    self:initStaticAttr(0)

    self._data_ready = false
    self._score_record = {}
    self._rank_data = {}
    self._rank_record = {}

    self._score = 0
    self._score_ts = 0
    self._target = 1
    self._rpos = nil
    self._immune = false
    self._is_reward = false

    self._inv = {}

    self:reset()

end

function player:check_ready()
    return self._data_ready
end

function player:reset(pos)
    self._score = 0
    self._score_ts = 0
    self._target = 1
    self._rpos = nil
    self._immune = false
    self._is_reward = false
    self:setHealth(20)
    self:setFoodLevel(20)
    self.scorePoints[ScoreID.POINT] = 0
    if pos then
        HostApi.resetPos(self.rakssid, unpack(pos))
    end
    ReportManager:reportUserEnter(self.userId)
end

function player:on_login()
    self:request_rank()
end

function player:on_logout()

end

function player:on_spawn()
    self:setSpeedAddition(0)
    self:setSpeedAddition(config.speedLevel)

    for _, item in ipairs(self._inv or {}) do
        self:addItem(item[1], item[2], 0)
    end
    self._inv = {}
end

function player:add_score(score)
    self._score = self._score + score
    self._score_ts = os.time()

    self:modify_rank_score(match:site_name(), self._score)
    MsgSender.sendCenterTipsToTarget(self.rakssid, 3, IMessages:msgGameScore(self._score))
end

function player:get_score()
    return self._score
end

function player:get_score_time()
    return self._score_ts
end

local function check_stage(self, x, y, z)
    local match_conf = match:site_conf()
    local stage_conf = assert(match_conf.stages[self._target])

    if utils.checkInRegion(stage_conf.region, x, y, z) then
        self:add_score(stage_conf.score)
        self:set_retire_pos(unpack(stage_conf.point))
        self._target = self._target % #match_conf.stages + 1
        self.scorePoints[ScoreID.POINT] = self.scorePoints[ScoreID.POINT] + 1
    end
end

local function check_tunnel(self, x, y, z)
    local match_conf = match:site_conf()
    for _, tunnel in ipairs(match_conf.tunnels) do
        if utils.checkInRegion(tunnel.entry, x, y, z) then
            HostApi.resetPos(self.rakssid, unpack(tunnel.exit))
            return true
        end
    end
end

local function check_fatal_block(self, x, y, z)
    if self._immune then
        return
    end

    local match_conf = match:site_conf()

    local pos = VectorUtil.newVector3i(
    NumberUtil.getIntPart(x),
    NumberUtil.getIntPart(y - 1),
    NumberUtil.getIntPart(z))

    local blockId = EngineWorld:getBlockId(pos)
    if match_conf.fatalBlock[blockId] then
        self:setHealth(0)
    end
end

function player:on_move(x, y, z)
    if self._immune then
        return false
    end

    local match_stage = match:stage()
    local match_conf = match:site_conf()

    if match_stage == define.MATCH_STAGE_READY then
        return utils.checkInRegion(match_conf.initRegion, x, y, z)
    elseif match_stage == define.MATCH_STAGE_START then
        check_stage(self, x, y, z)
        check_tunnel(self, x, y, z)
        check_fatal_block(self, x, y, z)
    end

    return true
end

function player:on_death()
    if self._rpos then
        self._rpos[4] = self:getYaw()
    end

    local items = {}
    do
        local inv = self:getInventory()
        for itemId in pairs(config.need_save_items) do
            local count = inv:getItemNum(itemId)
            if count > 0 then
                table.insert(items, { itemId, count })
            end
        end
    end
    self._inv = items

    RespawnHelper.sendRespawnCountdown(self, config.reviveTime)
end

function player:set_retire_pos(x, y, z)
    self._rpos = { x, y, z, self:getYaw() }
end

function player:retire()
    if not self._rpos then
        return
    end

    HostApi.resetPosWithYaw(self.rakssid, unpack(self._rpos))

    self._immune = true
    timer:register(1, function()
        self._immune = false
        return 0
    end)
end

function player:on_user_item(itemId)
    local buff = config.buffs[itemId]
    if not buff then
        return
    end

    self:addEffect(buff.effect, buff.time, buff.level)
end

function player:reward(rank, func)
    assert(match:stage() == define.MATCH_STAGE_CLOSE)

    local isWin = rank == 1

    UserExpManager:addUserExp(self.userId, isWin)

    if isWin then
        HostApi.sendPlaySound(self.rakssid, 10023)
    else
        HostApi.sendPlaySound(self.rakssid, 10024)
    end

    local function calc_golds(score)
        assert(score >= 0)
        if score <= 40 then
            return math.ceil(10 + 0.25 * score)
        elseif score <= 70 then
            return math.ceil(20 + 0.33 * (score - 40))
        else
            return math.ceil(50 + 0.5 * (score - 70))
        end
    end
    RewardManager:getUserGoldReward(self.userId, calc_golds(self._score), function(data)
        self._is_reward = true
        if match:is_all_player_reward_end() then
            func()
        end
    end)
    ReportManager:reportUserData(self.userId, 0, rank, 1, 1)
end

------------------------------------------------------------------
-- db
function player:request_db_data()
    DBManager:getPlayerData(self.userId, define.PLAYER_DB_CHUNK_DEFAULT)
end

function player:receive_db_data(data)
    if not data.__first then
        self._score_record = data.score_record or {}
        for _, item in ipairs(data.items or {}) do
            self:addItem(item[1], item[2], 0)
        end
    end

    self._data_ready = true
end

function player:save_db_data(immediate)

    if not self:check_ready() then
        return
    end

    local items = {}
    do
        local inv = self:getInventory()
        for itemId in pairs(config.need_save_items) do
            local count = inv:getItemNum(itemId)
            if count > 0 then
                table.insert(items, { itemId, count })
            end
        end
    end

    local score_record = {}
    do
        self.appIntegral = 0
        for _, site_rank in ipairs(config.ranks) do
            local site = assert(site_rank.site)
            local hkey = utils.hist_rank_id(site)
            local wkey = utils.week_rank_id(site)
            score_record[hkey] = self._score_record[hkey]
            score_record[wkey] = self._score_record[wkey]
            self.appIntegral = self.appIntegral + (score_record[wkey] or 0)
        end
    end

    local data = {
        score_record = score_record,
        items = items
    }

    DBManager:savePlayerData(self.userId, define.PLAYER_DB_CHUNK_DEFAULT, data, immediate)
end

---------------------------------------------------------------------
-- rank
function player:modify_rank_score(site, score)
    local diff = 0
    local wkey = utils.week_rank_id(site)
    if self._score_record[wkey] then
        if score > self._score_record[wkey] then
            diff = score - self._score_record[wkey]
            self._score_record[wkey] = score
        end
    else
        self._score_record[wkey] = score
        diff = score
    end

    --    utils.log("player:modify_rank_score", wkey, tostring(self.userId), diff)
    if diff > 0 then
        HostApi.ZIncrBy(wkey, tostring(self.userId), diff)
    end

    diff = 0
    local hkey = utils.hist_rank_id(site)
    if self._score_record[hkey] then
        if score > self._score_record[hkey] then
            diff = score - self._score_record[hkey]
            self._score_record[hkey] = score
        end
    else
        self._score_record[hkey] = score
        diff = score
    end

    --    utils.log("player:modify_rank_score", hkey, tostring(self.userId), diff)
    if diff > 0 then
        HostApi.ZIncrBy(hkey, tostring(self.userId), diff)
    end
end

function player:request_rank(site)
    self._rank_data = {}
    local keys = {}
    for _, site_rank in ipairs(config.ranks) do
        if not site or site == site_rank.site then
            self._rank_record[site_rank.site] = 0
            table.insert(keys, utils.week_rank_id(site_rank.site))
            table.insert(keys, utils.hist_rank_id(site_rank.site))
        end
    end

    for _, key in ipairs(keys) do
        HostApi.ZScore(key, tostring(self.userId))
    end

end

function player:receive_rank(key, rank, score)
    --    utils.log("player:receive_rank", key, rank, score)
    self._rank_data[key] = {
        rank = rank,
        userId = tonumber(tostring(self.userId)),
        score = score,
        name = self.name,
        vip = self.vip
    }

    local rscore = self._score_record[key]
    --    utils.print_table(self._score_record)
    if rscore and rscore ~= score then
        --        utils.log("ZIncrBy", key, tostring(self.userId), rscore - score)
        HostApi.ZIncrBy(key, tostring(self.userId), rscore - score)
    end

    local site = utils:get_site_from_key(key)
    if self._rank_record[site] == nil then
        return
    end
    self._rank_record[site] = self._rank_record[site] + 1
    if self._rank_record[site] % 2 == 0 then
        self:sync_rank(site)
    end

end

function player:sync_rank(site)
    for _, site_rank in ipairs(config.ranks) do
        if not site or site == site_rank.site then
            local wkey = utils.week_rank_id(site_rank.site)
            local hkey = utils.hist_rank_id(site_rank.site)

            local ranks_week = rank:fetch_rank(wkey)
            local ranks_day = rank:fetch_rank(hkey)
            local own_week = self._rank_data[wkey]
            local own_day = self._rank_data[hkey]

            if ranks_week and ranks_day and own_week and own_day then
                local data = {
                    ranks = {
                        week = rank:fetch_rank(wkey),
                        day = rank:fetch_rank(hkey)
                    },
                    own = {
                        week = self._rank_data[wkey],
                        day = self._rank_data[hkey]
                    },
                    title = site_rank.npc.name
                }

                HostApi.sendRankData(
                self.rakssid,
                rank:fetch_rank_npc(site_rank.site),
                json.encode(data))
            end
        end
    end
end

return player
