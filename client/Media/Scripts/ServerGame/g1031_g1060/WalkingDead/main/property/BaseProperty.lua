BaseProperty = class()

function BaseProperty:ctor(config,type)
    self.config = config
    self.type = type
    self:onCreate()
end

function BaseProperty:onCreate()

end

function BaseProperty:onPlayerGet(player)

end

function BaseProperty:onPlayerEquip(player)

end

function BaseProperty:onPlayerUnload(player)

end

function BaseProperty:onPlayerUsed(player,itemId)
end

return BaseProperty