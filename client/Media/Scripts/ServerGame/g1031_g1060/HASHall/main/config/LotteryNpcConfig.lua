---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15
---
require "data.GameLotteryNpc"

LotteryNpcConfig = {}
LotteryNpcConfig.npcs = {}

function LotteryNpcConfig:init(npcs)
    self:initLotteryNpc(npcs)
end

function LotteryNpcConfig:initLotteryNpc(npcs)
    for _, npc in pairs(npcs) do
        self.npcs[#self.npcs + 1] = GameLotteryNpc.new(npc)
    end
end

return LotteryNpcConfig