---
--- Created by Jimmy.
--- DateTime: 2018/6/11 0011 15:31
---
PotionEffectConfig = {}
PotionEffectConfig.potionEffects = {}

function PotionEffectConfig:init(potionEffects)
    self:initPotionEffects(potionEffects)
end

function PotionEffectConfig:initPotionEffects(potionEffects)
    for index, c_item in pairs(potionEffects) do
        local item = {}
        item.potionId = tonumber(c_item.potionId)
        item.time = tonumber(c_item.time)
        item.effectId = tonumber(c_item.effectId)
        item.effectTime = tonumber(c_item.effectTime)
        item.effectLv = tonumber(c_item.effectLv)
        local list = self.potionEffects[tostring(item.potionId)]
        if list == nil then
            list = {}
        end
        list[#list] = item
        self.potionEffects[tostring(item.potionId)] = list
    end
end

function PotionEffectConfig:getPotionEffect(potionId, time)
    local effects = self.potionEffects[tostring(potionId)]
    if effects == nil then
        return nil
    end
    local result
    for i, effect in pairs(effects) do
        if result == nil then
            result = effect
        end
        if time >= effect.time then
            result = effect
        end
    end
    return result
end

return PotionEffectConfig