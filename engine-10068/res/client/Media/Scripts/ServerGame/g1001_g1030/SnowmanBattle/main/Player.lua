require "config.GameConfig"
require "config.SnowBallConfig"
require "Match"
require "Timer"
require "RankNpc"

Player = class("GamePlayer", BasePlayer)

function Player:init()

    self:initStaticAttr()

    self.data_ready = false
    self.score_record = { }
    self.rank_data = { }
    self.rank_record = { }

    if self.dynamicAttr.team ~= 0 then
        self.rpos = GameConfig.teams[self.dynamicAttr.team].waitPos
    else
        self.rpos = nil
    end

    self.immune = false
    self.is_reward = false
    self.is_death = false
    self.respawnCount = GameConfig.respawn.count;
    self.pos = { }
    self.pos.x = 0
    self.pos.y = 0
    self.pos.z = 0
    self.kills = 0
    self.turedeath = false
    self.respawnObstacle = { }
    self:reset()


    self.snowball_speed = 0;

    self.snowball_lv = 1

    self.totalCurrency = 0;
    self.snowball_tick = 0;

    ReportManager:reportUserEnter(self.userId)
end

function Player:check_ready()
    return self:checkReady()
end

function Player:getDisplayName()
    if self.dynamicAttr.team ~= nil then
        if self.dynamicAttr.team == 1 then
            return TextFormat.colorWrite .. self.name .. TextFormat.colorRed .. "[" .. GameConfig.teams[self.dynamicAttr.team].name .. "]"
        else
            return TextFormat.colorWrite .. self.name .. TextFormat.colorBlue .. "[" .. GameConfig.teams[self.dynamicAttr.team].name .. "]"
        end
    else
        return TextFormat.colorWrite .. self.name
    end
end


function Player:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end
    -- pureName line
    local disName = self:getDisplayName()
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end


function Player:reset()
    -- self.rpos = nil
    self.immune = false
    self.is_reward = false
    self:setHealth(20)
    self:setFoodLevel(20)

    self.pos.x = 0
    self.pos.y = 0
    self.pos.z = 0
    self.snowball_lv = 1
    self.respawnCount = GameConfig.respawn.count;
    self.turedeath = false
    self:clearInv()
end

function Player:on_login()
    --RankNpc:updateRank()
    -- 登录的时候同步一次排行榜数据
end

function Player:on_logout()

end

function Player:checkInRegion(region, x, y, z)
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

function Player:AddItem(id, num, meta)

    self:addItem(id, num, meta)
end

function Player:on_move(x, y, z)
    if self.immune then
        return false
    end

    local Match_stage = Match.stage

    if Match.stage == Match.MATCH_STAGE_READY then
        return self:checkInRegion(GameConfig.teams[self.dynamicAttr.team].area, x, y, z)
    end
    self.pos.x = x
    self.pos.y = y
    self.pos.z = z

    return true
end

function Player:on_death()
    if self.respawnCount > 0 then
        RespawnHelper.sendRespawnCountdown(self, GameConfig.respawn.wait)
        if Match.stage > 1 and Match.stage < 5 then
            self.respawnCount = self.respawnCount - 1;
        end
    else
        self.turedeath = true
    end
end

function Player:set_retire_pos(x, y, z)
    self.rpos = { x, y, z }
end

function Player:retire()
    if not self.rpos then
        return
    end
    HostApi.resetPos(self.rakssid, unpack(self.rpos))

    self.pos.x = self.rpos[1]
    self.pos.y = self.rpos[2]
    self.pos.z = self.rpos[3]

    -- 免疫
    self.immune = true
    Timer:register(1, function()
        self.immune = false
        return 0
    end )
end

function Player:reward(rank, func, deathTeam)
    assert(Match.stage == Match.MATCH_STAGE_CLOSE)

    local isWin = false
    if deathTeam ~= nil then
        isWin = self.dynamicAttr.team ~= deathTeam
    end

    UserExpManager:addUserExp(self.userId, isWin, 2)

    self.scorePoints[ScoreID.KILL] = self.kills

    if isWin then
        HostApi.sendPlaySound(self.rakssid, 10023)
        ReportManager:reportUserWin(self.userId)
        self.appIntegral = self.appIntegral + 10
    else
        HostApi.sendPlaySound(self.rakssid, 10024)
    end

    RewardManager:getUserReward(self.userId, rank, function(data)
        self.is_reward = true
        if Match:is_all_player_reward_end() then
            func()
        end
    end)

    ReportManager:reportUserData(self.userId, self.kills, rank, 1)
end
---------------upgrade ball-------------------
function Player:upgradeBall(id)
    local ball = SnowBallConfig:getSnowballByLevel(id)
    if ball == nil then
        return false
    end
    if ball.price == 0 or self:getCurrency() < ball.price then
        return false
    end
    self:subCurrency(ball.price)
    self.snowball_lv = math.min(3, self.snowball_lv + 1)
    self.snowball_lv = math.max(1, self.snowball_lv)

    local inv = self:getInventory()
    local count = inv:getItemNum(332)
    self:removeItem(332, count)
    self:AddItem(332, count * GameConfig.snowballProportion, self.snowball_lv - 1)
    MsgSender.sendTopTipsToTarget(self.rakssid, 5, 1030008)
    return true
end

function Player:getUpgradeBallItemData(level)
    local ball = SnowBallConfig:getSnowballByLevel(level or 1)
    if ball == nil then
        return nil
    end
    local item = { }
    item.id = ball.id
    item.level = ball.damageToF
    item.name = ball.name
    item.image = ball.image
    item.desc = ball.desc
    item.price = ball.price
    item.value = ball.damageToP
    item.nextValue = ball.damageToSF
    return item
end

function Player:getObstacle(id)
    for i, obstacle in pairs(GameConfig.ObstacleCom) do
        if tonumber(obstacle.id) == id then
            return obstacle
        end
    end
end

function Player:getObstacleItemData(id)
    local obstacle = self:getObstacle(id)
    if obstacle == nil then
        return nil
    end
    local item = { }
    item.id = obstacle.id
    item.level = 1
    item.name = obstacle.name
    item.image = obstacle.image
    item.desc = obstacle.desc
    item.price = tonumber(obstacle.price)
    item.value = 1
    item.nextValue = 2
    item.status = 5
    return item
end

function Player:buyObstacle(id)
    local obstacle = self:getObstacle(tonumber(id))
    if obstacle == nil then
        return false
    end

    if tonumber(obstacle.price) == 0 or self:getCurrency() < tonumber(obstacle.price) then
        return false
    end

    local inv = self:getInventory()
    if inv:isCanAddItem(obstacle.ItemID, meta, obstacle.num) then
        self:subCurrency(tonumber(obstacle.price))
        self:AddItem(obstacle.ItemID, obstacle.num, 0)
        MsgSender.sendTopTipsToTarget(self.rakssid, 5, 1030009)
    end
    return true
end

function Player:sentUpgradeBallData()
    if self.entityPlayerMP == nil then
        return
    end
    local data = { }
    data.title = "Snowmanbattle.upgrade.ball"
    data.props = { }
    local ball = self:getUpgradeBallItemData(self.snowball_lv)
    if ball then
        data.props[#data.props + 1] = ball
    end

    for i, obstacleV in pairs(GameConfig.ObstacleCom) do
        local obstacle = self:getObstacleItemData(tonumber(obstacleV.id))
        if obstacle ~= nil then
            data.props[#data.props + 1] = obstacle
        end
    end

    self.entityPlayerMP:changeUpgradeProps(json.encode(data))

end

function Player:setWeekRank(rank, score)
    RankManager:addPlayerWeekRank(self, RankNpc.key, rank, score, function(npc)
        npc:sendRankData(self)
    end, RankNpc)
end

function Player:setDayRank(rank, score)
    RankManager:addPlayerDayRank(self, RankNpc.key, rank, score, function(npc)
        npc:sendRankData(self)
    end, RankNpc)
end

function Player:saveRankData()
    RankNpc:savePlayerRank(self)
end
return player
