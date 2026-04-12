---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 14:48
---
require "Match"

GameMerchant = class()

function GameMerchant:ctor(begin, pos, yaw, name)
    self.begin = begin
    self.id = EngineWorld:addMerchantNpc(pos, yaw, name)
end

function GameMerchant:syncPlayers()
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        self:syncPlayer(player)
    end
end

function GameMerchant:syncPlayer(player)
    MerchantUtil:changeCommodityList(self.id, player.rakssid, self.begin + player.yardLevel)
end

return GameMerchant