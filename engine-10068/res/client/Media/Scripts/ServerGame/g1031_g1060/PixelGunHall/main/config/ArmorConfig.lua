---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15
---
ArmorConfig = {}
ArmorConfig.armor = {}
ArmorConfig.baseArmor = 0
function ArmorConfig:initArmor(config)
    for k,v in pairs(config) do
        local item = {}
        item.level = tonumber(v.level)
        item.price = tonumber(v.price)
        item.priceId = tonumber(v.priceId)
        item.coin = tonumber(v.coin)
        item.coinId = tonumber(v.coinId)
        item.time = tonumber(v.time)
        item.armorUp = tonumber(v.armorUp)
        item.curLevelArmor = tonumber(v.curLevelArmor) or 0
        item.maxArmor = tonumber(v.maxArmor) or 0
        item.armorImg = v.armorImg
        item.tipHint = v.tipHint or ""
        self.armor[item.level] = item
    end

    ArmorConfig.baseArmor = self.armor[1].curLevelArmor or 10
end

function ArmorConfig:getArmorInfoByLevel(level)
    return self.armor[level] or self.armor[#self.armor]
end

function ArmorConfig:getCurArmorValueByLevel(level)
    return self:getArmorInfoByLevel(level).curLevelArmor
end

return ArmorConfig