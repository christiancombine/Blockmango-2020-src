AddDefenseProperty = class("AddDefenseProperty", BaseProperty)

function AddDefenseProperty:onPlayerEquip(player,itemId)
    GamePlayer:updateNowDefense(player)
end

function AddDefenseProperty:onPlayerUnload(player,itemId)
    GamePlayer:updateNowDefense(player)
end

return AddDefenseProperty