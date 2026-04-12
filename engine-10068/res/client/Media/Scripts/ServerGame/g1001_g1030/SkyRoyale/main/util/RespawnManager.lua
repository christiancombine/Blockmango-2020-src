RespawnManager = {}
RespawnManager.schedules = {}

function RespawnManager:addSchedule(userId, time)
    self.schedules[tostring(userId)] = time
end

function RespawnManager:tick()
    for userId, time in pairs(self.schedules) do
        self.schedules[tostring(userId)] = self.schedules[tostring(userId)] - 1
        if time <= 0 then
            RespawnManager:removeSchedule(userId, true)
        end
    end
end

function RespawnManager:removeSchedule(userId, isDead)
    isDead = isDead or false
    if self.schedules[tostring(userId)] ~= nil then
        self.schedules[tostring(userId)] = nil
    end
    if isDead then
        local player = PlayerManager:getPlayerByUserId(userId)
        if player ~= nil then
            player:onDie()
            GameMatch:ifGameOverByPlayer()
        end
    end
end

return RespawnManager