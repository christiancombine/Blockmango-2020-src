---
--- Created by Jimmy.
--- DateTime: 2018/8/3 0003 14:41
---
require "data.GameTipNpc"

TipNpcConfig = {}
TipNpcConfig.npcs = {}

function TipNpcConfig:init(npcs)
    self:initTipNpc(npcs)
end

function TipNpcConfig:initTipNpc(npcs)
    for _, npc in pairs(npcs) do
        self.npcs[#self.npcs + 1] = GameTipNpc.new(npc)
    end
end

return TipNpcConfig