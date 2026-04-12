AddAttackProperty = class("AddAttackProperty", BaseProperty)

function AddAttackProperty:onPlayerEquip(player)
    --player.addDamage = player.addDamage + self.config.value[1]
end

function AddAttackProperty:onPlayerUnload(player)
    --player.addDamage = player.addDamage - self.config.value[1]
end

return AddAttackProperty