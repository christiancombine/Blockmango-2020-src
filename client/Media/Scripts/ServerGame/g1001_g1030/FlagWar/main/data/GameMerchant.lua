---
--- Created by Jimmy.
--- DateTime: 2017/12/25 0025 11:54
---

GameMerchant = class()

function GameMerchant:ctor(config)
    self.level = 0
    self.config = config
    self.teamId = config.teamId
    self.goods = config.goods
    self.entityId = EngineWorld:addMerchantNpc(config.initPos, config.yaw, config.name)
end

function GameMerchant:getCurGoods()
    local info = self.config.goods[self.level]
    if info == nil then
        info = self.config.goods[1]
    end
    return info
end

function GameMerchant:syncPlayers()
    local info = self:getCurGoods()
    if info == nil then
        return
    end
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player:getTeamId() == self.teamId then
            MerchantUtil:changeCommodityList(self.entityId, player.rakssid, info.groupId)
        end
    end
end

function GameMerchant:onTick(tick)
    local level = 1
    for i, v in pairs(self.config.goods) do
        if tick > v.time then
            level = v.lv
        end
    end
    if self.level ~= level then
        self.level = level
        self:syncPlayers()
    end
end

function GameMerchant:onRemove()
    EngineWorld:removeEntity(self.entityId)
end
