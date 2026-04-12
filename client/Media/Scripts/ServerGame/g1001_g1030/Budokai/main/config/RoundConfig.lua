---
--- Created by Jimmy.
--- DateTime: 2018/7/17 0017 14:21
---

RoundConfig = {}

RoundConfig.Prepare = 1
RoundConfig.Started = 2

RoundConfig.rounds = {}

function RoundConfig:init(rounds)
    self:initRounds(rounds)
end

function RoundConfig:initRounds(rounds)
    for _, round in pairs(rounds) do
        local item = {}
        item.id = tonumber(round.id)
        item.prepare = tonumber(round.prepare)
        item.duration = tonumber(round.duration)
        local c_range = StringUtil.split(round.playerRange, "#")
        item.playerRange = {}
        item.playerRange[1] = tonumber(c_range[1])
        item.playerRange[2] = tonumber(c_range[2])
        item.siteType = tonumber(round.siteType)
        local c_effect = StringUtil.split(round.effect, "#")
        item.effect = { id = tonumber(c_effect[1]), lv = tonumber(c_effect[2]), time = tonumber(c_effect[3]) }
        local coins = {}
        local c_coins = StringUtil.split(round.coins, "#")
        for i, coin in pairs(c_coins) do
            local c_coin = StringUtil.split(coin, "=")
            local a_item = {}
            a_item.id = tonumber(c_coin[1]) or 0
            a_item.num = tonumber(c_coin[2]) or 0
            coins[#coins + 1] = a_item
        end
        item.coins = coins
        item.watchPos = VectorUtil.newVector3(round.watchX, round.watchY, round.watchZ)
        item.status = RoundConfig.Prepare
        item.roundTip = tonumber(round.roundTip)
        self.rounds[item.id] = item
    end
end

function RoundConfig:getRoundByMinPlayer(players)
    local c_round
    for _, round in pairs(self.rounds) do
        if c_round == nil then
            c_round = round
        end
        if round.playerRange[1] <= players and round.playerRange[2] >= players then
            c_round = round
            break
        end
    end
    return c_round
end

return RoundConfig