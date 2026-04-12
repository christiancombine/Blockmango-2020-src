
DefenseBuff = class("DefenseBuff",BaseBuff)

function DefenseBuff:onCreate()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    --player:addMaxDefense(self.value)
end

function DefenseBuff:onDestroy()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    --player:subMaxDefense(self.value)
end

return DefenseBuff