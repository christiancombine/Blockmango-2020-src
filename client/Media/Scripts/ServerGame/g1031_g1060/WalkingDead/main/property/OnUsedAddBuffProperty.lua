OnUsedAddBuffProperty = class("OnUsedAddBuffProperty", BaseProperty)

function OnUsedAddBuffProperty:onPlayerUsed(player,itemId)
    if self.config.probability >= math.random(100) then
        local buffId = tonumber(self.config.value[1])
        BuffManager:createBuff(itemId,player.userId,buffId)
    end
end

return OnUsedAddBuffProperty
