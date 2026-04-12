OnEquipAddBuffProperty = class("OnEquipAddBuffProperty", BaseProperty)

function OnEquipAddBuffProperty:onPlayerEquip(player, itemId)
    local buffId = tonumber(self.config.value[1])
    BuffManager:createBuff(itemId, player.userId, buffId)
end

function OnEquipAddBuffProperty:onPlayerUnload(player, itemId)
    local uniqueId = tostring(player.userId) .. "#" .. self.config.value
    local buffId = tonumber(self.config.value[1])
    BuffManager:removeBuff(itemId, uniqueId, buffId)
end

return OnEquipAddBuffProperty