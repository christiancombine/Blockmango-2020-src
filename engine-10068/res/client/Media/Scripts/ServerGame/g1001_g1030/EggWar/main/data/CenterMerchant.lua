---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 14:48
---

CenterMerchant = class()

function CenterMerchant:ctor(begin, pos, yaw, name, effect)
    self.begin = begin
    self.pos = pos
    self.yaw = yaw
    self.effect = effect
    self.id = EngineWorld:addMerchantNpc(pos, yaw, name)
    self:syncPlayers()
end

function CenterMerchant:syncPlayers()
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        self:syncPlayer(player.rakssid)
    end
end

function CenterMerchant:syncPlayer(rakssid)
    MerchantUtil:changeCommodityList(self.id, rakssid, self.begin)
end

return CenterMerchant