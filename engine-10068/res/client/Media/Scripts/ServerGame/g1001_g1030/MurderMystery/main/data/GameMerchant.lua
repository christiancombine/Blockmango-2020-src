---
--- Created by Jimmy.
--- DateTime: 2017/12/26 0026 10:54
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
    if player.role >= GamePlayer.ROLE_MURDERER and player.role <= GamePlayer.ROLE_CIVILIAN then
        MerchantUtil:changeCommodityList(self.id, player.rakssid, self.begin + player.role)
    else
        MerchantUtil:changeCommodityList(self.id, player.rakssid, 0)
    end
end

return GameMerchant