local define = require "define"
local config = require "config"
local utils = require "utils"

local rank = {}

function rank:init()
    local match = require "match"
    self._rank_data = {}
    self._rank_npc = {}
    self._rank_record = {}
    self._sessionNpc = {}

    for _, site_rank in ipairs(config.ranks) do
        local site = assert(site_rank.site)
        HostApi.ZExpireat(utils.week_rank_id(site), tostring(DateUtil.getWeekLastTime()))

        -- create npc
        do
            self._rank_npc[site] = EngineWorld:addRankNpc(
                    VectorUtil.newVector3(unpack(site_rank.npc.pos)),
                    site_rank.npc.dir,
                    site_rank.npc.name
            )
        end
    end

    if config and config.session_npc then
        for i, v in pairs(config.session_npc) do
            local sessionNpc = {}
            sessionNpc.pos = VectorUtil.newVector3(v.pos[1], v.pos[2], v.pos[3])
            sessionNpc.yaw = tonumber(v.pos[4])
            sessionNpc.message = tostring(v.message)
            sessionNpc.name = tostring(v.name)
            sessionNpc.actor = tostring(v.actor)
            sessionNpc.session_npc_type = tostring(v.session_npc_type)
            self._sessionNpc[i] = sessionNpc
        end

        for i, v in pairs(self._sessionNpc) do
            local entityId = EngineWorld:addSessionNpc(v.pos, v.yaw, v.session_npc_type, v.actor, function(entity)
                entity:setNameLang(v.name or "")
                entity:setSessionContent(v.message or "")
            end)
            v.entityId = entityId
        end
    end
end

----------------------------------------------
-- rank
function rank:request_rank(site)
    local keys = {}
    for _, site_rank in ipairs(config.ranks) do
        if not site or site == site_rank.site then
            self._rank_record[site_rank.site] = 0
            table.insert(keys, utils.week_rank_id(site_rank.site))
            table.insert(keys, utils.hist_rank_id(site_rank.site))
        end
    end

    for _, key in ipairs(keys) do
        HostApi.ZRange(key, 0, 9)
    end
end

function rank:receive_rank(key, data)
    --    utils.log("rank:receive_rank", key, data)
    self._rank_data[key] = {}

    local userIds = {}
    local ranks = StringUtil.split(data, "#")
    for i, data in pairs(ranks) do
        local item = StringUtil.split(data, ":")
        local rank = {
            rank = i,
            userId = tonumber(item[1]),
            score = tonumber(item[2]),
            vip = 0,
            name = "anonymous_" .. item[1],
        }
        table.insert(self._rank_data[key], rank)
        table.insert(userIds, tonumber(item[1]))
    end

    UserInfoCache:GetCacheByUserIds(userIds, function(key)
        for _, rank in ipairs(self._rank_data[key]) do
            local info = UserInfoCache:GetCache(rank.userId)
            if info then
                rank.vip = info.vip
                rank.name = info.name
            end
        end

        local site = utils:get_site_from_key(key)
        if self._rank_record[site] == nil then
            return
        end
        self._rank_record[site] = self._rank_record[site] + 1
        if self._rank_record[site] % 2 == 0 then
            local players = PlayerManager:getPlayers()
            for _, player in pairs(players) do
                player:sync_rank(site)
            end
        end

    end, key, key)

end

function rank:fetch_rank(key)
    return self._rank_data[key]
end

function rank:fetch_rank_npc(site)
    return assert(self._rank_npc[site], string.format("site is not exist: %s", tostring(site)))
end

return rank
