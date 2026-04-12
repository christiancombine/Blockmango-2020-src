---
--- Created by Jimmy.
--- DateTime: 2017/12/12 0012 17:01
---

WalletUtils = {}

function WalletUtils:addCoinMappings(coinMapping)
    for _, mapping in pairs(coinMapping) do
        HostApi.addCoinMapping(mapping.coinId, mapping.itemId)
    end
end

return WalletUtils
