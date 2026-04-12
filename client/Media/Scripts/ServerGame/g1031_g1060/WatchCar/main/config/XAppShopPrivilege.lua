XAppShopPrivilege = {}
XAppShopPrivilege.confisTables = {}
function XAppShopPrivilege:OnInit(configs)
    local propsTabs = {}
    for _, config in pairs(configs) do
        local configTab = {}
        configTab.GoodsId = tonumber(config.id)
        configTab.GoodsDesc = config.PrivilegeDesc == '#' and "" or config.PrivilegeName
        configTab.GoodsMetaId = tonumber(config.GoodsMetaId)
        configTab.Limit = tonumber(config.Limit) or -1
        configTab.Numberof = 0--configTab.Limit or 1
        configTab.CoinId = tonumber(config.CoinId)
        configTab.BlockymodsPrice = tonumber(config.BlockmodsPrice) or 1
        configTab.BlockmanPrice = tonumber(config.BlockmanPrice) or 0
        configTab.GoodsImage = config.PrivilegeImage == '#' and '' or config.PrivilegeImage
        configTab.BindShopId = tonumber(config.BindShopId) or 1
        table.insert(self.confisTables, configTab)
    end
end

function XAppShopPrivilege:GetLimitCount(id)
    for _, privilege in pairs(self.confisTables) do
        if privilege and privilege.GoodsId == id then
            return privilege.Limit
        end
    end
    return 0
end

function XAppShopPrivilege:getConfigs()
    local goodsTab = {}
    for _, config in pairs(self.confisTables) do
        local goods = self:newGoodsIntance(config)
        table.insert(goodsTab, goods)
    end
    return goodsTab
end

function XAppShopPrivilege:getGoodsByShopId(shopId)
    local goodsTab = {}
    for _, config in pairs(self.confisTables) do
        if config.BindShopId == shopId then
            local goods = self:newGoodsIntance(config)
            table.insert(goodsTab, goods)
        end
    end
    return goodsTab
end

function XAppShopPrivilege:newGoodsIntance(item)
    local goods = Goods.new()
    goods.itemId = item.GoodsId
    goods.itemMeta = item.GoodsMetaId
    goods.itemNum = XPrivilegeConfig:getReawrdItemCount(goods.itemId) or 0
    goods.coinId = item.CoinId
    goods.blockymodsPrice = item.BlockymodsPrice
    goods.blockmanPrice = item.BlockmanPrice

    goods.limit = -1
    goods.desc = item.GoodsDesc
    local tab = {}
    tab.type = item.BindShopId
    tab.intance = goods
    return tab
end