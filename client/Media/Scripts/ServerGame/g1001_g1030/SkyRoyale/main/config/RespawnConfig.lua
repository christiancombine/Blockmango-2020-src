require "config.GameConfig"

RespawnConfig = {}
RespawnConfig.respawnGoods = {}

function RespawnConfig:initRespawnGoods(config)
    for _, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.id)
        item.time = tonumber(v.time)
        item.coinId = tonumber(v.coinId)
        item.blockymodsPrice = tonumber(v.blockymodsPrice)
        item.blockmanPrice = tonumber(v.blockmanPrice)
        item.times = tonumber(v.times)
        self.respawnGoods[item.id] = item
    end
end

function RespawnConfig:prepareRespawnGoods()
    for i, goods in pairs(self.respawnGoods) do
        EngineWorld:getWorld():addRespawnGoodsToShop(self:newRespawnGoods(goods))
    end
end

function RespawnConfig:newRespawnGoods(item)
    local goods = RespawnGoods.new()
    goods.time = item.time
    goods.coinId = item.coinId
    goods.blockymodsPrice = item.blockymodsPrice
    goods.blockmanPrice = item.blockmanPrice
    goods.times = item.times
    return goods
end

function RespawnConfig:showBuyRespawn(rakssId, times)
    if times >= #self.respawnGoods then
        return false
    end
    local player = PlayerManager:getPlayerByRakssid(rakssId)
    if not player then
        return false
    end
    local goods = self:getRespawnGoodById(times + 1)
    if goods == nil then
        return false
    end
    if goods.blockymodsPrice == 0 or goods.blockmanPrice == 0 then
        RespawnHelper.sendRespawnCountdown(player, GameConfig.respawnTime)
        return true
    end
    local time = RespawnConfig:getRespawnTimeByCount(player.respawnTimes)
    RespawnManager:addSchedule(player.userId, time + 1) ---添加到队列检查，隔一段时间不买活就去死
    HostApi.showBuyRespawn(rakssId, times)
    return true
end

function RespawnConfig:getRespawnGoodById(times)
    for _, goods in pairs(self.respawnGoods) do
        if goods.id == times then
            return goods
        end
    end

    return nil
end

function RespawnConfig:getRespawnTimeByCount(count)
    local goods = self:getRespawnGoodById(count + 1)
    if goods == nil then
        return 0
    end
    return goods.time
end

return RespawnConfig