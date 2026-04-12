---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15
---
require "data.GameModeNpc"

ModeNpcConfig = {}

function ModeNpcConfig:init(npcs)
    self:initModeNpcs(npcs)
end

function ModeNpcConfig:initModeNpcs(npcs)
    for _, npc in pairs(npcs) do
        GameModeNpc.new(npc)
    end
end

return ModeNpcConfig