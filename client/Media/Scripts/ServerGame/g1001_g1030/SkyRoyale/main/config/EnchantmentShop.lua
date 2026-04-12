EnchantmentShop = {}
EnchantmentShop.enchantments = {}

function EnchantmentShop:initEnchantment(config)
    for i, v in pairs(config) do
        local item = {}
        item.id = v.id
        item.title = v.title
        item.itemId = tonumber(v.itemId)
        item.meta = tonumber(v.meta)
        item.num = tonumber(v.num)
        item.name = v.name
        item.image = v.image
        item.description = v.description
        item.price = tonumber(v.price)
        item.enchantId = StringUtil.split(v.enchantId, "#")
        item.enchantLevel = StringUtil.split(v.enchantLevel, "#")
        self.enchantments[tonumber(item.id)] = item
    end
end

function EnchantmentShop:findEnchantmentById(id)
    for i, v in pairs(self.enchantments) do
        if v.id == id then
            return v
        end
    end

    return nil
end

function EnchantmentShop:getEnchantmentItems()
    return self.enchantments
end

return EnchantmentShop
