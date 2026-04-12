---
--- Created by Jimmy.
--- DateTime: 2018/8/24 0024 15:22
---
AttackXConfig = {}
AttackXConfig.AttackX = {}

function AttackXConfig:init(AttackX)
    self:initAttackX(AttackX)
end

function AttackXConfig:initAttackX(AttackX)
    for _, attack in pairs(AttackX) do
        self.AttackX[attack.attacker .. "#" .. attack.attacked] = tonumber(attack.attackX)
    end
end

function AttackXConfig:getAttackX(attacker, attacked)
    local attackX = self.AttackX[attacker .. "#" .. attacked]
    if attackX == nil then
        return 1
    end
    return attackX
end

return AttackXConfig
