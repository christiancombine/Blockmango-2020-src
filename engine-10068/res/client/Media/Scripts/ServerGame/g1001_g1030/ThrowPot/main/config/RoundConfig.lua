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
        item.duration = tonumber(round.duration)
        item.minPlayer = tonumber(round.minPlayer)
        local num = StringUtil.split(round.tntNum, "#")
        item.tntNum = {}
        if #num >= 2 then
            item.tntNum[1] = tonumber(num[1])
            item.tntNum[2] = tonumber(num[2])
        else
            item.tntNum[1] = tonumber(num[1])
        end
        item.status = RoundConfig.Prepare
        item.mode = tonumber(round.mode)
        item.position = VectorUtil.newVector3(round.x, round.y, round.z)
        self.rounds[#self.rounds + 1] = item
    end
    table.sort(self.rounds, function(a, b)
        return a.minPlayer > b.minPlayer
    end)
end

function RoundConfig:getRoundByMinPlayer(players)
    local c_round
    for _, round in pairs(self.rounds) do
        if c_round == nil then
            c_round = round
        end
        if round.minPlayer >= players and round.minPlayer <= c_round.minPlayer then
            c_round = round
        end
    end
    return c_round
end

return RoundConfig