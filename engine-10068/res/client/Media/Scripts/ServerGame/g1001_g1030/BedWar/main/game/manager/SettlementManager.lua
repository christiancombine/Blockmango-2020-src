SettlementManager = {}

local damageFrom = {}

local playersInfo = {}

local sendGoodRecord = {}

local function getRealLifeCount()
    local count = 0
    for _, info in pairs(playersInfo) do
        if tonumber(info:getValue("rank")) == 0 then
            count = count + 1
        end
    end
    return count
end

local function getAssistList(diedId, killerId)
    local assistIds = {}
    local playerInfo = GameInfoManager:getPlayerInfo(killerId)
    if playerInfo then
        local damageRecord = damageFrom[tostring(diedId)]
        local teamInfo = GameInfoManager:getTeamInfo(playerInfo:getValue("teamId"))
        if teamInfo and damageRecord then
            for userId, _ in pairs(damageRecord) do
                if teamInfo:isPlayerInTeam(userId) and tostring(userId) ~= tostring(killerId) then
                    table.insert(assistIds, userId)
                end
            end
        end
    end
    return assistIds
end

function SettlementManager:addPlayer(player)
    damageFrom[tostring(player.userId)] = {}
    playersInfo[tostring(player.userId)] = player.info
end

function SettlementManager:subPlayer(player)
    if GameMatch.allowPvp then
        return
    end
    damageFrom[tostring(player.userId)] = nil
    playersInfo[tostring(player.userId)] = nil
end

function SettlementManager:onPlayerDied(userId, killerId, multiKill)
    if killerId and userId ~= killerId then
        local dieInfo = GameInfoManager:getPlayerInfo(userId)
        local killInfo = GameInfoManager:getPlayerInfo(killerId)
        if killInfo and dieInfo then
            dieInfo:setValue("die", tonumber(dieInfo:getValue("die")) + 1)
            killInfo:setValue("kill", tonumber(killInfo:getValue("kill")) + 1)
            local curMaxCombo = tonumber(killInfo:getValue("max_kill_combo"))
            if curMaxCombo < multiKill then
                killInfo:setValue("max_kill_combo", multiKill)
            end
            local assistIds = getAssistList(userId, killerId)
            for _, assistId in pairs(assistIds) do
                local assistInfo = GameInfoManager:getPlayerInfo(assistId)
                if assistInfo then
                    dieInfo:setValue("assist", tonumber(dieInfo:getValue("assist")) + 1)
                end
            end
            if multiKill > 5 then
                multiKill = 5
            end
            GamePacketSender:sendBedWarKillBroadCast(killerId, userId, multiKill, table.concat(assistIds, ":"))
            local player = PlayerManager:getPlayerByUserId(killerId)
            if player then
                player.taskController:packingProgressData(Define.TaskType.Fright.Kill, nil, 1)
            end
        end
    end
    damageFrom[tostring(userId)] = {}
end

function SettlementManager:onPlayerHurt(hurtId, fromId, value)
    if fromId then
        local damageRecord = damageFrom[tostring(hurtId)]
        damageRecord[tostring(fromId)] = damageRecord[tostring(fromId)] or 0
        damageRecord[tostring(fromId)] = damageRecord[tostring(fromId)] + value
        local fromInfo = GameInfoManager:getPlayerInfo(fromId)
        if fromInfo then
            local attack_damage = tonumber(fromInfo:getValue("attack_damage")) + value
            attack_damage = math.floor(attack_damage * 10) / 10
            fromInfo:setValue("attack_damage", attack_damage)
            local player = PlayerManager:getPlayerByUserId(fromId)
            if player then
                player.taskController:packingProgressData(Define.TaskType.Fright.AttackDamage, nil, attack_damage)
            end
        end

        local hurtInfo = GameInfoManager:getPlayerInfo(hurtId)
        if hurtInfo then
            local hurt_damage = tonumber(fromInfo:getValue("hurt_damage")) + value
            hurt_damage = math.floor(hurt_damage * 10) / 10
            hurtInfo:setValue("hurt_damage", hurt_damage)
        end
    end
end

function SettlementManager:addBuildScore(userId, buildType)
    local info = playersInfo[tostring(userId)]
    if info then
        local score = tonumber(info:getValue("build_score"))
        info:setValue("build_score", score + (GameConfig.buildScore[buildType] or 1))
        local player = PlayerManager:getPlayerByUserId(userId)
        if player and tonumber(buildType) == 1 then
            player.taskController:packingProgressData(Define.TaskType.Fright.PlaceBlock, nil, 1)
        end
    end
end

function SettlementManager:updateFinalSettlement(winnerTeamId)
    for teamId, _ in pairs(TeamConfig:getTeams()) do
        local teamInfo = GameInfoManager:getTeamInfo(teamId)
        if teamInfo then
            ---失败
            teamInfo:setValue("isWin", "0")
            if teamId == tonumber(winnerTeamId) then
                ---胜利
                teamInfo:setValue("isWin", "2")
            end
            if winnerTeamId == 0 then
                ---平局
                teamInfo:setValue("isWin", "1")
            end
        end
    end
    local infoList = {}
    for _, info in pairs(playersInfo) do
        ---结算连杀奖杯
        info:forceSync({ "attack_damage" })
        info:forceSync({ "build_score" })
        info:setValue("cups", "")
        local max_kill_combo = tonumber(info:getValue("max_kill_combo"))
        if max_kill_combo >= 10 then
            info:addCup(Define.CupType.Combo_Kill_10)
        elseif max_kill_combo >= 5 then
            info:addCup(Define.CupType.Combo_Kill_5)
        elseif max_kill_combo >= 4 then
            info:addCup(Define.CupType.Combo_Kill_4)
        elseif max_kill_combo >= 3 then
            info:addCup(Define.CupType.Combo_Kill_3)
        elseif max_kill_combo >= 2 then
            info:addCup(Define.CupType.Combo_Kill_2)
        end
        table.insert(infoList, info)
    end

    if #infoList >= 2 then
        ---排序 击杀
        table.sort(infoList, function(info1, info2)
            return tonumber(info1:getValue("kill")) > tonumber(info2:getValue("kill"))
        end)
        infoList[1]:addCup(Define.CupType.Gold_Kill)
        infoList[2]:addCup(Define.CupType.Silver_Kill)
        ---排序 助攻
        table.sort(infoList, function(info1, info2)
            return tonumber(info1:getValue("assist")) > tonumber(info2:getValue("assist"))
        end)
        infoList[1]:addCup(Define.CupType.Gold_Assist)
        infoList[2]:addCup(Define.CupType.Silver_Assist)
        ---排序 受伤
        table.sort(infoList, function(info1, info2)
            return tonumber(info1:getValue("hurt_damage")) > tonumber(info2:getValue("hurt_damage"))
        end)
        infoList[1]:addCup(Define.CupType.Gold_Hurt)
        infoList[2]:addCup(Define.CupType.Silver_Hurt)

        ---排序 捡钻石
        table.sort(infoList, function(info1, info2)
            return tonumber(info1:getValue("collect_diamonds")) > tonumber(info2:getValue("collect_diamonds"))
        end)
        infoList[1]:addCup(Define.CupType.Diamonds)

        ---排序 捡绿宝石
        table.sort(infoList, function(info1, info2)
            return tonumber(info1:getValue("collect_gem")) > tonumber(info2:getValue("collect_gem"))
        end)
        infoList[1]:addCup(Define.CupType.Gem)
    end

    ---整理剩余所有玩家
    local allLifeInfo = {}
    for _, info in pairs(infoList) do
        if tonumber(info:getValue("rank")) == 0 then
            table.insert(allLifeInfo, info)
        end
    end
    table.sort(allLifeInfo, function(info1, info2)
        return tonumber(info1:getKDA()) > tonumber(info2:getKDA())
    end)

    for index, info in pairs(allLifeInfo) do
        info:setValue("rank", tostring(index))
    end

    ---按名次排序, 找出每一队的mvp
    table.sort(infoList, function(info1, info2)
        return tonumber(info1:getValue("rank")) < tonumber(info2:getValue("rank"))
    end)
    local mvpMast = {}
    for _, info in pairs(infoList) do
        local curTeam = info:getValue("teamId")
        local curMvp = mvpMast[curTeam] or { kda = 0, userId = 0 }
        local curKDA = info:getKDA()
        if curKDA > curMvp.kda then
            mvpMast[curTeam] = { kda = curKDA, info = info }
        end
    end

    for _, curMvp in pairs(mvpMast) do
        curMvp.info:setValue("isMvp", "1")
    end

    ---立即更新数据
    GameInfoManager:onTick()
    for _, player in pairs(PlayerManager:getPlayers()) do
        SettlementManager:sendSingleSettlement(player, true)
    end
    ---给玩家结算平台金币
    RewardManager:getQueueReward(function()

    end)
end

function SettlementManager:onPlayerLose(player)
    if tonumber(player.info:getValue("rank")) ~= 0 then
        return
    end
    local rank = getRealLifeCount()
    player.info:setValue("rank", tostring(rank))
    ---立即更新数据
    GameInfoManager:onTick()
    SettlementManager:sendSingleSettlement(player)
end

function SettlementManager:sendSingleSettlement(player, isEnd, watchAd)
    if AIManager:isAI(player.rakssid) then
        return
    end
    local rank = tonumber(player.info:getValue("rank"))
    local teamInfo = GameInfoManager:getTeamInfo(player.teamId)
    if not teamInfo then
        LogUtil.log("sendSingleSettlement failed", LogUtil.LogLevel.Error)
        return
    end
    if watchAd then
        SeasonManager:addUserHonor(player.userId, player.rewardHonour, player.level)
        player.rewardHonour = 0
    else
        ---give reward
        if not player.alreadyReward then
            player.alreadyReward = true
            UpdateTipsUtil:sendFeedback(player)
            self:doReward(player, teamInfo, rank)
        end
    end

    ---send packet
    local cur_max, is_max = ExpConfig:getMaxExp(player.level)
    local param = {
        level = player.level or 0,
        rewardExp = player.rewardExp or 0,
        cupExp = player.cur_exp or 0,
        maxExp = cur_max or 0,
        honorId = player.honorId or 0,
        key = player.rewardKey or 0,
        honour = player.rewardHonour or 0,
        isEnd = isEnd or false,
        watchAd = watchAd or false
    }

    local data = DataBuilder.new():fromTable(param):getData()
    PacketSender:sendLuaCommonData(player.rakssid, "SingleSettlement", data)
end

function SettlementManager:doReward(player, teamInfo, rank)
    if player.isPartyUser then
        player.appIntegral = 0
    else
        local isWin = tonumber(teamInfo:getValue("isWin")) == 2
        if not player.alreadyHonor then
            local _, is_max = ExpConfig:getMaxExp(player.level)
            if not is_max and player.isBeenHall then
                ---经验值没满，并且去过大厅才能获得经验
                player.rewardExp = HonourConfig:getRewardExp(rank, isWin, DoubleExpPrivilege:onPlayerReward(player))
                player:addExp(player.rewardExp)
            end
            player.rewardKey = HonourConfig:getYaoShiByRank(rank)
            player:addYaoShi(player.rewardKey)
            player.rewardHonour = HonourConfig:getHonourByRank(rank)
            UserExpManager:addUserExp(player.userId, isWin, 4)
            SeasonManager:addUserHonor(player.userId, player.rewardHonour, player.level)
            player.alreadyHonor = true
            player.taskController:packingProgressData(Define.TaskType.Fright.CompleteGame, nil, 1)
            player.taskController:packingProgressData(Define.TaskType.Fright.GameRank, rank, 1)
            --GameLicense:onPlayerLogout(player)
        end
        if not RewardManager:isUserRewardFinish(player.userId) then
            if isWin then
                LuaTimer:schedule(function()
                    HostApi.sendPlaySound(player.rakssid, 10023, true)
                end, 0)
            else
                LuaTimer:schedule(function()
                    HostApi.sendPlaySound(player.rakssid, 10024)
                end, 0)
            end
            RewardManager:addRewardQueue(player.userId, rank)
            RewardManager:getUserReward(player.userId, rank, function()

            end)
        end

        if isWin then
            player.appIntegral = player.appIntegral + 10
            ReportManager:reportUserWin(player.userId)
            ReportUtil.reportEndTime(player)
        end
    end
end

function SettlementManager:sendGood(fromUserId, toUserId)
    local key = tostring(fromUserId) .. "#" .. tostring(toUserId)
    if sendGoodRecord[key] then
        return
    end
    sendGoodRecord[key] = 1
    local toPlayerInfo = GameInfoManager:getPlayerInfo(toUserId)
    if toPlayerInfo then
        toPlayerInfo:setValue("be_good_times", tonumber(toPlayerInfo:getValue("be_good_times")) + 1)
    end

end

