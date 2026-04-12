require "settlement.SettlementBase"

TeamSettlement = class("TeamSettlement", SettlementBase)

-- 第一个参数 参与排序的玩家 第二个参数是赢的队伍的id  第三个是延时时间
function TeamSettlement:SendSettlementResult(players, winTeamId, time)
    if #players == 0 then
        return
    end
    for _, player in pairs(players) do
        if player:getTeamId() == winTeamId then
            player.outCome = GameResult.WIN
        else
            player.outCome = GameResult.LOSE
        end
    end
    self:sortPlayers(players)
    LuaTimer:schedule(function()
        self:doReward(players, winTeamId)
        self:doReport(players)
    end, time * 1000)
end

function TeamSettlement:doGameReward(players, winTeamId)
    local blue_players = {}
    local red_players = {}
    for _, player in pairs(players) do
        if player:getTeamId() == TeamManager.Blue then
            table.insert(blue_players, player)
        else
            table.insert(red_players, player)
        end
    end
    self:sortPlayers(blue_players)
    self:sortPlayers(red_players)
    local data = {}
    data.blue_team = {}
    data.red_team = {}
    data.win_team = winTeamId
    for rank, player in pairs(blue_players) do
        local image = "set:pixelgungameresult.json image:rank" .. (player.honorId + 1)
        table.insert(data.blue_team, {
            rank = rank,
            duam_img = image,
            lv = player.level,
            name = player.name,
            kills = player.kills
        })
    end
    for rank, player in pairs(red_players) do
        local image = "set:pixelgungameresult.json image:rank" .. (player.honorId + 1)
        table.insert(data.red_team, {
            rank = rank,
            duam_img = image,
            lv = player.level,
            name = player.name,
            kills = player.kills
        })
    end
    data.blue_team_score = TeamManager.Teams[TeamManager.Blue].kills
    data.red_team_score = TeamManager.Teams[TeamManager.Red].kills

    for rank, player in pairs(blue_players) do
        local own_data = data
        own_data.reward_gold_num = player.gold
        own_data.reward_list = self:getPlayerRewardData(player, rank, winTeamId == player:getTeamId())
        own_data.self_team = player:getTeamId()
        own_data.self_rank = rank
        own_data.show_ad = (player.advert_record.GameAdTimes < GameConfig.maxGameAdTimes)
        GamePacketSender:sendPixelGunTeamResultData(player.rakssid, true, json.encode(own_data))
        player:showLvUpReward()
    end

    for rank, player in pairs(red_players) do
        local own_data = data
        own_data.reward_gold_num = player.gold
        own_data.reward_list = self:getPlayerRewardData(player, rank, winTeamId == player:getTeamId())
        own_data.self_team = player:getTeamId()
        own_data.self_rank = rank
        own_data.show_ad = (player.advert_record.GameAdTimes < GameConfig.maxGameAdTimes)
        GamePacketSender:sendPixelGunTeamResultData(player.rakssid, true, json.encode(own_data))
        player:showLvUpReward()
    end
end
