---
--- Created by Jimmy.
--- DateTime: 2018/9/25 0025 11:52
---
require "data.GameGunMerchant"

GunMerchantConfig = {}

function GunMerchantConfig:init(merchants)
    self:initMerchants(merchants)
end

function GunMerchantConfig:initMerchants(merchants)
    for _, merchant in pairs(merchants) do
        GameGunMerchant.new(merchant)
    end
end

return GunMerchantConfig