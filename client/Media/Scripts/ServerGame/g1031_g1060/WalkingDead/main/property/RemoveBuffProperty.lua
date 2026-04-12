RemoveBuffProperty  = class("RemoveBuffProperty",BaseProperty)

function RemoveBuffProperty:onPlayerUsed(player,itemId)
    for _,duffId in pairs(self.config.value) do
        local uniqueId = tostring(player.userId ).. "#" .. duffId
        BuffManager:cleanBuff(uniqueId)
    end
end

return RemoveBuffProperty