require "Timer"
require "config.GameConfig"

require "TipNpc"
require "Snowman"
require "Flag"
require "RankNpc"

GameMatch = { }
Match = { }

Match.gameType = "g1030"

Match.MATCH_STAGE_NONE = 0     -- 初始状态
Match.MATCH_STAGE_WAIT = 1     -- 等待玩家
Match.MATCH_STAGE_READY = 2     -- 准备阶段
Match.MATCH_STAGE_FIGHT = 3     -- 战斗阶段
Match.MATCH_STAGE_FINAL = 4     -- 最终期间
Match.MATCH_STAGE_CLOSE = 5     -- 比赛结束

Match.stage = Match.MATCH_STAGE_NONE

function Match:init()
    EngineWorld:stopWorldTime()

    self.stage = Match.MATCH_STAGE_NONE
    self.site_idx = nil
    self.game_timer = nil

    self.limit_timer = nil
    self.money_timer = nil
    self.snowballspeed_timer = nil
    self.snowball_timer = nil
    self.tnt_timer = nil
    self.close_timer = nil

    self.teams = { }
    self.snowmans = { }
    self.flags = { }

end

local function reset(self)
    -- clear
    do
        Timer:close(self.game_timer)

            if self.limit_timer then
        Timer:close(self.limit_timer)
        self.limit_timer = nil
    end

    if self.money_timer then
        Timer:close(self.money_timer)
        self.money_timer = nil
    end

    if self.snowballspeed_timer then
        Timer:close(self.snowballspeed_timer)
        self.snowballspeed_timer = nil
    end

    if self.snowball_timer then
        Timer:close(self.snowball_timer)
        self.snowball_timer = nil
    end

    if self.tnt_timer then
        Timer:close(self.tnt_timer)
        self.tnt_timer = nil
    end

        self.stage = Match.MATCH_STAGE_NONE
    end

end

function Match:on_open()
    -- rank:request_rank()


    reset(self)
    self:step_wait_stage()
end

function Match:on_close()
    if self.stage ~= Match.MATCH_STAGE_CLOSE then
        return
    end
    self.close_timer = Timer:register(30, function()
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            HostApi.sendGameover(player.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
        end
        Timer:close(self.close_timer)
        GameServer:closeServer()
    end)
end

function Match:on_player_login(player)
    assert(self.stage == Match.MATCH_STAGE_WAIT)
    player:setCurrency(0)
    player:on_login()
    HostApi.resetPos(player.rakssid, unpack(GameConfig.teams[player.dynamicAttr.team].waitPos))
end

function Match:on_player_logout(player)
    player:on_logout()
    if PlayerManager:isPlayerEmpty() then
        self:force_reset_game()
    end
end

---------------------------------------------------------------------------
function Match:step_wait_stage()
    assert(self.stage == Match.MATCH_STAGE_NONE)
    self.stage = Match.MATCH_STAGE_WAIT

    if Listener.hitHP ~= nil then
        for k, HP in pairs(Listener.hitHP) do
            Listener.hitHP[k] = nil
        end
    end

    RankNpc:updateRank()

    if self.npcs == nil then
        self.npcs = { }
        for k, npc in pairs(GameConfig.tipNpc) do
            self.npcs[k] = TipNpc.new(npc)
        end
    end


    if (self.snowmans ~= nil) then
        for _, snowman in pairs(self.snowmans) do
            EngineWorld:removeEntity(snowman.entityId)
        end
    end
    self.snowmans = { }

    if (self.flags ~= nil) then
        for _, flag in pairs(self.flags) do
            EngineWorld:removeEntity(flag.entityId)
        end
    end
    self.flags = { }


    for teamsk, teamsv in ipairs(GameConfig.teams) do
        Snowman.new(teamsk, teamsv.guradSnowman)
        -- self.snowman[teamsk] = Snowman.new(teamsk,v)

        for _, flag in pairs(teamsv.flag) do
            Flag.new(teamsk, flag)
        end
    end

    Match:updateTeamSnowHp()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if not player:check_ready() then
            HostApi.sendGameover(player.rakssid, IMessages:msgGameHasStarting(), GameOverCode.GameStarted)
        else
            player:set_retire_pos(unpack(GameConfig.teams[player.dynamicAttr.team].waitPos))
            player:retire()
        end
    end


    local tick = 0
    local prestartPlayerCount = 0
    self.game_timer = Timer:register(1, function()
        if PlayerManager:isPlayerExists() then
            tick = tick + 1
            if tick >= GameConfig.waitingTime - 12 and prestartPlayerCount == 0 and PlayerManager:isPlayerEnough(GameConfig.minPlayers) then
                prestartPlayerCount = PlayerManager:getPlayerCount()
                HostApi.sendStartGame(prestartPlayerCount)
            end
            if tick >= GameConfig.waitingTime then
                if PlayerManager:isPlayerEnough(GameConfig.minPlayers) then
                    self:step_ready_stage()
                    return 0
                else
                    tick = 0
                    if prestartPlayerCount ~= 0 then
                        HostApi.sendResetGame(PlayerManager:getPlayerCount())
                    end
                    prestartPlayerCount = 0
                    MsgSender.sendMsg(IMessages:longWaitingHint())
                    MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.minPlayers))
                    return 1
                end
            else
                MsgSender.sendBottomTips(2, IMessages:msgWaitPlayerEnough(GameConfig.waitingTime - tick, IMessages.UNIT_SECOND, false))

                return 1
            end
        else
            tick = 0
            if prestartPlayerCount ~= 0 then
                HostApi.sendResetGame(PlayerManager:getPlayerCount())
            end
            prestartPlayerCount = 0
        end
    end )
end

function Match:step_ready_stage()
    assert(self.stage == Match.MATCH_STAGE_WAIT)
    self.stage = Match.MATCH_STAGE_READY
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    HostApi.sendGameStatus(0, 1)
    self:updateTeamSnowHp()
    RewardManager:startGame()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:sentUpgradeBallData()
    end

    MsgSender.sendCenterTips(5, 1030002)

    for _, player in pairs(players) do
        if not player:check_ready() then
            HostApi.sendGameover(player.rakssid, IMessages:msgGameHasStarting(), GameOverCode.GameStarted)
        else
            player:set_retire_pos(unpack(GameConfig.teams[player.dynamicAttr.team].initPos))
            player:retire()

            if (player.is_death == false) then
                player:AddItem(GameConfig.initObstacle.id, GameConfig.initObstacle.count, 0)
            end

            player:addCurrency(GameConfig.initMoney)
            player.totalCurrency = player:getCurrency()
        end
    end

    local tick = 0
    self.game_timer = Timer:register(1, function()
        tick = tick + 1
        if tick >= GameConfig.readyTime then
            --            HostApi.sendGameStatus(0, 1)
            self:step_fight_stage()
            return 0
        end

        MsgSender.sendBottomTips(2, 1030003,(GameConfig.readyTime - tick))
        return 1
    end )



    self.limit_timer = Timer:register(1, function()
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            if player ~= nil then
                if player.pos.y > GameConfig.heightLimit then
                    player:subHealth(1)
                end
                if player:getHealth() < 10 then
                    MsgSender.sendTopTipsToTarget(player.rakssid, 5, 1030007)
                end
            end
        end
    end )



end

function Match:updateTeamSnowHp()
    local resource = { }
    local index = 0
    --    print(#self.snowmans)
    for i, snowman in ipairs(self.snowmans) do
        index = #resource + 1
        resource[index] = { }
        resource[index].teamId = i
        resource[index].value = snowman.hp
        resource[index].maxValue = snowman.maxhp
    end
    HostApi.updateTeamResources(json.encode(resource))
end

function Match:step_fight_stage()
    assert(self.stage == Match.MATCH_STAGE_READY)
    self.stage = Match.MATCH_STAGE_FIGHT

    for x = GameConfig.airBlockArea.min[1], GameConfig.airBlockArea.max[1], 1 do
        for y = GameConfig.airBlockArea.min[2], GameConfig.airBlockArea.max[2], 1 do
            for z = GameConfig.airBlockArea.min[3], GameConfig.airBlockArea.max[3], 1 do
                local pos = VectorUtil.newVector3i(x, y, z)
                EngineWorld:setBlockToAir(pos)
            end
        end
    end

    --    print("ok!")

    local moneyTick = 0
    self.money_timer = Timer:register(GameConfig.moneyGrowth.growth[1], function()
        moneyTick = moneyTick + GameConfig.moneyGrowth.growth[1]
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            if player ~= nil then
                if (moneyTick > GameConfig.moneyGrowth.startTime) and(moneyTick <= GameConfig.moneyGrowth.stopTime) then
                    player:addCurrency(GameConfig.moneyGrowth.growth[2])
                    player.totalCurrency = player.totalCurrency + GameConfig.moneyGrowth.growth[2]
                else
                    moneyTick = 0
                end
            end
        end
    end )



    local snowballTick = 0
    self.snowballspeed_timer = Timer:register(1, function()
        snowballTick = snowballTick + 1
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            if player ~= nil then
                if (snowballTick > GameConfig.snowballSpeed[3][1]) then
                    player.snowball_speed = 3;
                elseif (snowballTick > GameConfig.snowballSpeed[2][1]) then
                    player.snowball_speed = 2;
                elseif (snowballTick > GameConfig.snowballSpeed[1][1]) then
                    player.snowball_speed = 1;
                else
                    player.snowball_speed = 0;
                end
            end
        end
    end )


    self.snowball_timer = Timer:register(1, function()
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            if player ~= nil then
                if (player.snowball_speed > 0) then
                    local sbinterval = GameConfig.snowballSpeed[player.snowball_speed][2]
                    if (player.snowball_tick >= sbinterval) then
                        local sbcount = GameConfig.snowballSpeed[player.snowball_speed][3]
                        if (player.is_death == false) then
                            player:AddItem(332, sbcount, player.snowball_lv - 1)
                        end
                        player.snowball_tick = 0;
                    else
                        player.snowball_tick = player.snowball_tick + 1;
                    end
                end
            end
        end
    end )

    local potionTick = 0
    self._potion_timer = Timer:register(GameConfig.potion.interval, function()
        potionTick = potionTick + GameConfig.potion.interval
        if potionTick >= GameConfig.potion.startTime then

            for posK, posV in pairs(GameConfig.potion.pos) do
                local producePos = VectorUtil.newVector3(unpack(posV))
                EngineWorld:addEntityItem(GameConfig.potion.id, GameConfig.potion.count, 0, GameConfig.potion.life, producePos)
            end
        end

    end )

    local tick = 0
    self.game_timer = Timer:register(1, function()
        tick = tick + 1
        if tick >= GameConfig.fightTime then
            self:step_final_stage()
            return 0
        end

        MsgSender.sendBottomTips(2, 1030004,(GameConfig.fightTime - tick))

        return 1
    end )
end

function Match:step_final_stage()
    assert(self.stage == Match.MATCH_STAGE_FIGHT)
    self.stage = Match.MATCH_STAGE_FINAL
    MsgSender.sendCenterTips(5, 1030005)

    local TNT_Tick = 0
    self.tnt_timer = Timer:register(GameConfig.TNT.produce[1], function()
        TNT_Tick = TNT_Tick + GameConfig.TNT.produce[1]
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            if player ~= nil then
                if TNT_Tick >= GameConfig.TNT.stratTime and TNT_Tick <= GameConfig.TNT.stopTime then
                    if (player.is_death == false) then
                        player:AddItem(BlockID.TNT, GameConfig.TNT.produce[2], 0)
                    end
                end
            end
        end
    end )

    local tick = 0
    self.game_timer = Timer:register(1, function()
        tick = tick + 1
        if tick >= GameConfig.finalTime then
            self:step_close_stage()
            return 0
        end

        MsgSender.sendBottomTips(2, 1030006,(GameConfig.finalTime - tick))
        return 1
    end )
end


function Match:sort_players()
    local players = PlayerManager:copyPlayers()
    table.sort(players, function(p1, p2)
        if p1.kills ~= p2.kills then
            return p1.kills > p2.kills
        end
        if p1.snowball_lv ~= p2.snowball_lv then
            return p1.snowball_lv > p2.snowball_lv
        end
        if p1._respawnCount ~= p2._respawnCount then
            return p1._respawnCount > p2._respawnCount
        end
        return p1.userId > p2.userId
    end )
    return players
end

function Match:sort_Settlement(deathTeam)
    local players = PlayerManager:copyPlayers()
    table.sort(players, function(p1, p2)

        if p1.dynamicAttr.team ~= p2.dynamicAttr.team then
            if deathTeam ~= nil then
                if p1.dynamicAttr.team ~= deathTeam then
                    return true
                else
                    return false
                end
            end
        end

        if p1.totalCurrency ~= p2.totalCurrency then
            return p1.totalCurrency > p2.totalCurrency
        end
        return p1.userId > p2.userId
    end )

    return players
end


function Match:sendRealRankData(player)
    local players = self:sort_players()
    local index = 0
    local data = { }
    data.ranks = { }
    for i, v in pairs(players) do
        index = index + 1
        local item = { }
        item.column_1 = tostring(index)
        item.column_2 = v.name
        item.column_3 = tostring(v.kills)
        item.column_4 = tostring(v.snowball_lv)
        data.ranks[index] = item
    end
    local rank = RankManager:getPlayerRank(players, player)
    local own = { }
    own.column_1 = tostring(rank)
    own.column_2 = player.name
    own.column_3 = tostring(player.kills)
    own.column_4 = tostring(player.snowball_lv)
    data.own = own
    HostApi.updateRealTimeRankInfo(player.rakssid, json.encode(data))
end

function Match:is_team_all_death(team)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.dynamicAttr.team == team then
            print("all 0")
            if player.turedeath == false then
                print("all 1")
                return false
            end
        end
    end
    return true
end

function Match:is_all_player_turedeath()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.turedeath == false then
            return false
        end
    end
    return true
end

function Match:step_close_stage(closeType, deathTeam)
    if closeType == nil then
        assert(self.stage == Match.MATCH_STAGE_FINAL)
    end
    if self.game_timer then
        Timer:close(self.game_timer)
        self.game_timer = nil
    end

    if self.limit_timer then
        Timer:close(self.limit_timer)
        self.limit_timer = nil
    end

    if self.money_timer then
        Timer:close(self.money_timer)
        self.money_timer = nil
    end

    if self.snowballspeed_timer then
        Timer:close(self.snowballspeed_timer)
        self.snowballspeed_timer = nil
    end

    if self.snowball_timer then
        Timer:close(self.snowball_timer)
        self.snowball_timer = nil
    end

    if self.tnt_timer then
        Timer:close(self.tnt_timer)
        self.tnt_timer = nil
    end
    local players = PlayerManager:getPlayers()
    for k, player in pairs(players) do
        RankNpc:savePlayerRankScore(player, deathTeam)
    end

    self.stage = Match.MATCH_STAGE_CLOSE

    local function action()
        local players = self:sort_Settlement(deathTeam)
        -- 发奖                    isWin = win and 1 or 0
        local func = function()
            -- 结算
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
                        kills = 0,
                        isWin = win,
                        adSwitch = adSwitch_result,
                    }
                end

                local players = self:sort_Settlement(deathTeam)
                local statistic = { }
                for rank, player in ipairs(players) do
                    local isWin = 2
                    if deathTeam ~= nil then
                        isWin = player.dynamicAttr.team ~= deathTeam
                        isWin = isWin and 1 or 0
                    end
                    table.insert(statistic, newSettlementItem(player, rank, isWin))
                end

                for rank, player in ipairs(players) do
                    local result = {
                        players = statistic,
                        own = statistic[rank]
                    }
                    HostApi.sendGameSettlement(player.rakssid, json.encode(result), true)
                end
            end
        end

        for rank, player in ipairs(players) do
            player:reward(rank, func, deathTeam)
        end

        HostApi.syncGameTimeShowUi(0, false, 0)

        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            player:reset()
            player:set_retire_pos(unpack(GameConfig.teams[player.dynamicAttr.team].waitPos))
            player:retire()
        end
        RankNpc:updateRank()

        self:on_close()
    end

    if closeType == 0 then
        if deathTeam == 1 then
            MsgSender.sendCenterTips(10, 1030014)
        else
            MsgSender.sendCenterTips(10, 1030015)
        end
        LuaTimer:schedule(function()
            action()
        end, 10)
    else
        action()
    end

end

function Match:force_reset_game()

end



function Match:is_all_player_reward_end()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.is_reward == false then
            return false
        end
    end
    return true
end
function Match:getEntityByEntityId(entityId)
    local entity = self:getSnowMan(entityId)
    if entity ~= nil then
        return entity
    end
    entity = self:getFlag(entityId)
    if entity ~= nil then
        return entity
    end
    return nil
end

function Match:onAddSnowMan(snowMan)
    self.snowmans[tonumber(snowMan.teamId)] = snowMan
end

function Match:getSnowMan(snowManId)
    for i, snowman in ipairs(self.snowmans) do
        if snowman.entityId == snowManId then
            return snowman
        end
    end
end

function Match:onRemoveSnowMan(snowMan)
    EngineWorld:removeEntity(snowMan.entityId)
    self.snowmans[tonumber(snowMan.teamId)] = nil
end

function Match:onAddFlag(flag)
    self.flags[tostring(flag.entityId)] = flag
end

function Match:getFlag(flagId)
    return self.flags[tostring(flagId)]
end

function Match:onRemoveFlag(flag)
    EngineWorld:removeEntity(flag.entityId)
    self.flags[tostring(flag.entityId)] = nil
end
return Match
