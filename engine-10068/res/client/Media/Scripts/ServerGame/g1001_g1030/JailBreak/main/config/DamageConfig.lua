---
--- Created by Administrator.
--- DateTime: 2018/2/4 0004 18:07
---

DamageConfig = {}
DamageConfig.damage ={}


function DamageConfig:init(config)
    self:initDamage(config.damage)
end

function DamageConfig:initDamage(config)
    for i, v in pairs(config) do
        local damage = {}
        damage.id = tonumber(v.id)
        damage.damage = tonumber(v.damage)
        self.damage[tostring(damage.id)] = damage
    end
end


function DamageConfig:getDamage(id)
    return self.damage[tostring(id)]
end

return DamageConfig