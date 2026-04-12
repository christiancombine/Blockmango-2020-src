---
--- Created by Jimmy.
--- DateTime: 2017/12/25 0025 11:43
---
MerchantUtil = {}

function MerchantUtil:addCommodity(index, type, goods)
    HostApi.addCommodity(index, type, goods)
end

function MerchantUtil:changeCommodityList(entityId, rakssId, index)
    HostApi.changeCommodityList(entityId, rakssId, index)
end

return MerchantUtil