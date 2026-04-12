require "settlement.SettlementBase"

--local settlementIndex = 0
PvpSettlement = class("PvpSettlement", SettlementBase)
-- 第一个参数 参与排序的玩家  第二个是延时时间
function PvpSettlement:SendSettlementResult(players, time, winner)
    if #players == 0 then
        return
    end
    for i, player in pairs(players) do
        if i == winner then
            player.outCome = GameResult.WIN
        else
            player.outCome = GameResult.LOSE
        end
    end

    LuaTimer:schedule(function()
        self:doReward(players, winner)
        self:doReport(players)
    end, time * 1000)
end

function PvpSettlement:doGameReward(players, winner)
    local blue = players[1] or {}
    local red = players[2] or {}
    local data = {}
    local blue_img = ""
    if blue.honorId ~= nil then
        blue_img = "set:pixelgungameresult.json image:rank" .. (blue.honorId + 1)
    end
    data.blue_duan_img = blue_img
    data.blue_lv = blue.level or -1
    data.blue_name = blue.name or ""
    data.blue_kills = blue.kills or -1

    local red_img = ""
    if red.honorId ~= nil then
        red_img = "set:pixelgungameresult.json image:rank" .. (red.honorId + 1)
    end
    data.red_duan_img = red_img
    data.red_lv = red.level or -1
    data.red_name = red.name or ""
    data.red_kills = red.kills or -1

    data.min_money = ChestConfig:getChestMoneyById(1)
    for i, player in pairs(players) do
        local own_data = data
        own_data.self_team = i
        own_data.reward_gold_num = player.gold
        own_data.reward_list = self:getPlayerRewardData(player, 1, i == winner)
        own_data.win_team = winner
        own_data.show_ad = (player.advert_record.GameAdTimes < GameConfig.maxGameAdTimes)
        local site = SiteConfig:getSitesById(player.siteId)
        if site.playernum == 2 then
            own_data.revenge_btn_enable = true
        else
            own_data.revenge_btn_enable = false
        end
        GamePacketSender:sendPixelGun1v1ResultData(player.rakssid, true, json.encode(own_data))
        player:showLvUpReward()
        player.kills = 0
        player.headshots = 0
        player.dies = 0
    end
end