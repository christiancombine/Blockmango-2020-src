BaseProperty = class()

function BaseProperty:ctor(config)
    self.config = config
    self:onCreate()
end

function BaseProperty:onCreate()

end

function BaseProperty:onPlayerGet(player)

end

function BaseProperty:onPlayerRemove(player)

end

function BaseProperty:onPlayerEquip(player)

end

function BaseProperty:onPlayerUnload(player)

end

function BaseProperty:onPlayerUsed(player, target, damage)
end

function BaseProperty:onPlayerBeAttack(player, source, damage)

end

return BaseProperty