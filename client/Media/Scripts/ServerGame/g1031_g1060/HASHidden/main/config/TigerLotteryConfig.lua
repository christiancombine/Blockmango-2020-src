
TigerLotteryConfig = {}
TigerLotteryConfig.tigerLottery = {}


function TigerLotteryConfig:init(config)
    self:initTigerLottery(config)
end

function TigerLotteryConfig:initTigerLottery(Datas)
    for id, data in pairs(Datas) do
        local item = {}
        item.id = data.id
        item.actor_name = data.actor_name
        item.name = data.name
        item.image = data.image
        item.weights = data.weights
        self.tigerLottery[id] = item
    end
    LotteryUtil:initLotteryConfig()
end

function TigerLotteryConfig:getActorById(id)
    for i, actor in pairs(self.tigerLottery) do
        if actor.id == id then
            return actor
        end
    end
    return nil
end

return TigerLotteryConfig