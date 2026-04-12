require "data.GameMerchant"

MerchantConfig = {}
MerchantConfig.merchants = {}
MerchantConfig.upgradeMerchants = {}
MerchantConfig.tools = {}

function MerchantConfig:initMerchants(merchants)
    for _, v in pairs(merchants) do
        local merchant = {}
        merchant.id = tonumber(v.id)
        merchant.name = v.name
        merchant.initPos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        merchant.yaw = tonumber(v.initPos[4])
        merchant.goods = {}
        for _, g in pairs(v.goods) do
            local item = {}
            item.lv = tonumber(g.lv)
            item.groupId = tonumber(g.groupId)
            merchant.goods[item.lv] = item
        end
        self.merchants[merchant.id] = merchant
    end
end

function MerchantConfig:prepareMerchant()
    for _, merchant in pairs(self.merchants) do
        merchant.npc = GameMerchant.new(merchant)
    end
end

function MerchantConfig:syncPlayer(player)
    for _, merchant in pairs(self.merchants) do
        merchant.npc:syncPlayer(player)
    end
end

function MerchantConfig:initUpgradeMerchants(merchants)
    for _, v in pairs(merchants) do
        local merchant = {}
        merchant.id = tonumber(v.id)
        merchant.name = v.name
        merchant.initPos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        merchant.yaw = tonumber(v.initPos[4])
        merchant.actor = v.actor
        self.upgradeMerchants[merchant.id] = merchant
    end
end

function MerchantConfig:prepareUpgradeMerchants()
    for _, v in pairs(self.upgradeMerchants) do
        local entityId = EngineWorld:addSessionNpc(v.initPos, v.yaw, SessionNpcConfig.PERSONAL_SHOP, v.actor, function(entity)
            entity:setNameLang(v.name or "")
        end)
        v.entityId = entityId
    end
end

function MerchantConfig:initTools(tools)
    for id, tool in pairs(tools) do
        local item = {}
        item.id = id
        item.itemId = tonumber(tool.itemId)
        item.level = tonumber(tool.level)
        item.name = tool.name
        item.image = tool.image
        item.desc = tool.description
        item.price = tonumber(tool.price)
        item.title = tool.title
        self.tools[tostring(item.level)] = item
    end
end

function MerchantConfig:getToolsById(id)
    for _, tool in pairs(self.tools) do
        if id == tool.id then
            return tool
        end
    end

    return nil
end

function MerchantConfig:getToolsByLevel(level)
    return self.tools[tostring(level)]
end

return MerchantConfig