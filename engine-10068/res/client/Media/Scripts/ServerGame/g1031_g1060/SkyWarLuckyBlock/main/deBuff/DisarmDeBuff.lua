require "deBuff.BaseDeBuff"

DisarmDeBuff = class("DisarmDeBuff", BaseDeBuff)

function DisarmDeBuff:onCreate()
    self:doAction()
end

function DisarmDeBuff:doAction()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    HostApi.setDisarmament(player.rakssid, true)
end

function DisarmDeBuff:onDestroy()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    HostApi.setDisarmament(player.rakssid, false)
end

return DisarmDeBuff