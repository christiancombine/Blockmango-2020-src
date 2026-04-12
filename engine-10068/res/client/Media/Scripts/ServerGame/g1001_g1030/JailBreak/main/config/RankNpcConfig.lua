---
--- Created by Jimmy.
--- DateTime: 2018/3/13 0013 20:50
---

RankNpcConfig = {}
RankNpcConfig.rankNpc = {}

function RankNpcConfig:init(config)
    self:initRankNpc(config.rankNpc)
end

function RankNpcConfig:initRankNpc(config)
    for i, v in pairs(config) do
        local npc = {}
        npc.pos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        npc.yaw = tonumber(v.initPos[4])
        npc.name = v.name
        self.rankNpc[i] = npc
    end
end

function RankNpcConfig:prepareRankNpc()
    for i, v in pairs(self.rankNpc) do
        v.entityId = EngineWorld:addRankNpc(v.pos, v.yaw, v.name)
    end
end

function RankNpcConfig:sendRankData(rakssid, data)
    for i, v in pairs(self.rankNpc) do
        HostApi.sendRankData(rakssid, v.entityId, data)
    end
end

return RankNpcConfig
