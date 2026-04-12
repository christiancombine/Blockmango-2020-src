---
--- Created by Jimmy.
--- DateTime: 2017/12/25 0025 11:54
---
require "Match"
require "config.GameConfig"

GameMerchant = class()

function GameMerchant:ctor(team, pos, yaw, name)
    self.team = team
    self.id = EngineWorld:addMerchantNpc(pos, yaw, name)
    self.level = 0
    self:upgrade()
end

function GameMerchant:upgrade()
    if self.level < GameConfig.maxLevel then
        self.level = self.level + 1
        self:syncPlayers()
    end
end

function GameMerchant:syncPlayers()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        MerchantUtil:changeCommodityList(self.id, v.rakssid, (self.team - 1) * 2 + self.level)
    end
end

function GameMerchant:syncPlayer(rakssid)
    MerchantUtil:changeCommodityList(self.id, rakssid, (self.team - 1) * 2 + self.level)
end
