VideoAdHelper = {}

function VideoAdHelper:checkShowAd(player)
    self:checkShowTiger1Ad(player)
    self:checkShowTiger2Ad(player)
    self:checkShowFlyingAd(player)
end

function VideoAdHelper:checkShowTiger1Ad(player)
    local params = {
        isShow = false
    }

    local watchAdTimeSection = VideoAdHelper:getTimeSection(player:getTiger1AdWatchTime())
    local curTimeSection = VideoAdHelper:getTimeSection(os.time())

    if curTimeSection > watchAdTimeSection then
        params.isShow = true
    end

    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(player.rakssid, "BlockCityData", "TigerAd1", data)
end

function VideoAdHelper:checkShowTiger2Ad(player)
    local params = {
        isShow = false
    }

    local watchAdTimeSection = VideoAdHelper:getTimeSection(player:getTiger2AdWatchTime())
    local curTimeSection = VideoAdHelper:getTimeSection(os.time())

    if curTimeSection > watchAdTimeSection then
        params.isShow = true
    end

    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(player.rakssid, "BlockCityData", "TigerAd2", data)
end

function VideoAdHelper:checkShowFlyingAd(player)
    local params = {
        isShow = false
    }

    local watchAdTimeSection = VideoAdHelper:getTimeSection(player:getFlyingAdWatchTime())
    local curTimeSection = VideoAdHelper:getTimeSection(os.time())

    if curTimeSection > watchAdTimeSection then
        params.isShow = true
    end

    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(player.rakssid, "BlockCityData", "FlyingAd", data)
end

function VideoAdHelper:doGetTigerAdReward(player, tigerLotteryId)
    local id = tonumber(tigerLotteryId)
    local tigerLottery = TigerLotteryManager.tigerLotterys[id]
    if tigerLottery then
        local watchAdTimeSection = 0
        if tigerLottery.tigerLotteryId == 1 then
            watchAdTimeSection = VideoAdHelper:getTimeSection(player:getTiger1AdWatchTime())
        elseif tigerLottery.tigerLotteryId == 2 then
            watchAdTimeSection = VideoAdHelper:getTimeSection(player:getTiger2AdWatchTime())
        end

        local curTimeSection = VideoAdHelper:getTimeSection(os.time())
        if curTimeSection <= watchAdTimeSection then
            return
        end

        tigerLottery:RefreshTigerLottery(player.rakssid, 0)
        player:setTigerLotteryCountdowns(id, 0)

        if tigerLottery.tigerLotteryId == 1 then
            player:setTiger1AdWatchTime(os.time())
            VideoAdHelper:checkShowTiger1Ad(player)
        elseif tigerLottery.tigerLotteryId == 2 then
            player:setTiger2AdWatchTime(os.time())
            VideoAdHelper:checkShowTiger2Ad(player)
        end

        local tigerData = {
            entityId = id
        }
        local data1 = DataBuilder.new():fromTable(tigerData):getData()
        PacketSender:sendServerCommonData(player.rakssid, "BlockCityTigerAdShowUI", "", data1)

        PacketSender:sendDataReport(player.rakssid, "reward_ad_give", DBManager:getGameType())
    end
end

function VideoAdHelper:doGetFlyingAdReward(player)
    local watchAdTimeSection = VideoAdHelper:getTimeSection(player:getFlyingAdWatchTime())
    local curTimeSection = VideoAdHelper:getTimeSection(os.time())
    if curTimeSection <= watchAdTimeSection then
        return
    end

    player:setFlyingTime(GameConfig.flyingAdReward)
    player:setFlyingAdWatchTime(os.time())
    VideoAdHelper:checkShowFlyingAd(player)

    PacketSender:sendDataReport(player.rakssid, "reward_ad_give", DBManager:getGameType())
end

function VideoAdHelper:getTimeSection(vTime)
    local time = {}
    local year = tonumber(os.date("%Y", os.time()))
    local month = tonumber(os.date("%m", os.time()))
    local day = tonumber(os.date("%d", os.time()))
    for _, v in pairs(GameConfig.ADRefreshTime) do
        local hour = tonumber(StringUtil.split(v, ":")[1])
        local min = tonumber(StringUtil.split(v, ":")[2])
        local refreshTime = os.time({ year = year, month = month, day = day, hour = hour, min = min, sec = 0 })
        table.insert(time, refreshTime)
    end

    table.sort(time, function(a, b)
        return a < b
    end)

    local timeSection = 0
    for _, v in pairs(time) do
        if vTime >= v then
            timeSection = v
        end
    end

    return timeSection
end

return VideoAdHelper