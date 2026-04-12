---
--- Created by Jimmy.
--- DateTime: 2017/12/25 0025 11:54
---

GameMerchant = class()

function GameMerchant:ctor(config)
    self.config = config
    self.entityId = EngineWorld:addMerchantNpc(config.initPos, config.yaw, config.name)
end

function GameMerchant:syncPlayer(player)
    MerchantUtil:changeCommodityList(self.entityId, player.rakssid, 1)
end

function GameMerchant:onRemove()
    EngineWorld:removeEntity(self.entityId)
end
