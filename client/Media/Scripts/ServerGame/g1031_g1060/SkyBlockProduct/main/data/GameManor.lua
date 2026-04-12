GameManor = class()


function GameManor:ctor(manorIndex, userId)
    self.userId = userId
    self.manorIndex = manorIndex
    self.time = 0
    self.visitor = {}
end

function GameManor:addTime(time)
    self.time = self.time + time
end

function GameManor:refreshTime()
    self.time = 0
end

function GameManor:getTime()
    return self.time
end

function GameManor:masterLogin()
    HostApi.log("masterLogin " .. tostring(self.userId))
end

function GameManor:masterLogout()
    HostApi.log("masterLogout " .. tostring(self.userId))
end

function GameManor:getMasterState()
    local player = PlayerManager:getPlayerByUserId(self.userId)
    return player ~= nil
end

function GameManor:visitorInHouse(userId)
    self.visitor[userId] = userId
end

function GameManor:visitorLeaveHouse(userId)
    self.visitor[userId] = nil
end

function GameManor:checkVisitor()
    for _, userId in pairs(self.visitor) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            self:visitorLeaveHouse(userId)
        else
            -- if ManorConfig:isInManor(self.manorIndex, player:getPosition()) == false then
            --     self:visitorLeaveHouse(userId)
            -- end
        end
    end
end

function GameManor:isEmptyVisitor()
    return next(self.visitor) == nil
end


