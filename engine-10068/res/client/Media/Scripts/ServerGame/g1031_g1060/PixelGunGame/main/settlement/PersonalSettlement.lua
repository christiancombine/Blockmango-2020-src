require "settlement.SettlementBase"
require "config.GameConfig"

PersonalSettlement = class("PersonalSettlement", SettlementBase)
--  第一个参数 参与排序的玩家  第二个是延时时间 第三个参数表示游戏开始时间
function PersonalSettlement:SendSettlementResult(players, time)
    if #players == 0 then
        return
    end
    self:sortPlayers(players)
    for rank, player in pairs(players) do
        if rank == 1 then
            player.outCome = GameResult.WIN
        else
            player.outCome = GameResult.LOSE
        end
        player.taskController:packingProgressData(player, Define.TaskType.Fright.PersonalRank, GameMatch.gameType, rank)
    end
    LuaTimer:schedule(function()
        self:doReward(players, 0)
        self:doReport(players)
    end, time * 1000)
end

function PersonalSettlement:doGameReward(players)
    local data = {}
    data.player_list = {}
    for rank, player in pairs(players) do
        local image = "set:pixelgungameresult.json image:rank" .. (player.honorId + 1)
        table.insert(data.player_list, {
            rank = rank,
            duam_img = image,
            lv = player.level,
            name = player.name,
            kills = player.kills
        })
    end

    for rank, player in pairs(players) do
        local own_data = data
        own_data.reward_gold_num = player.gold
        own_data.reward_list = self:getPlayerRewardData(player, rank, false)
        own_data.self_rank = rank
        own_data.show_ad = (player.advert_record.GameAdTimes < GameConfig.maxGameAdTimes)
        GamePacketSender:sendPixelGunPersonResultData(player.rakssid, true, json.encode(own_data))
        player:showLvUpReward()
    end
end