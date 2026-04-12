GameManor = class()

function GameManor:ctor(manorIndex, userId)
    self.userId = userId
    self.manorIndex = manorIndex
    self.level = 1
    self.name = GameConfig.defaultName
    self.time = 0
    self.isMasterInGame = false
    self.visitor = {}
end

function GameManor:initDataFromDB(data)
    self.level = tonumber(data.level or 1)
    self.name = data.name or GameConfig.defaultName
end

function GameManor:prepareDataSaveToDB()
    local data = {
        level = self.level,
        name = self.name,
    }
    return data
end

function GameManor:setManorName(name)
    self.name = name
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

function GameManor:addShowName(rakssid)
    local namePos = ManorConfig:getNamePos(self.manorIndex)
    if namePos == nil then
        return
    end
    HostApi.deleteWallText(rakssid, namePos)
    HostApi.addWallText(rakssid, 2, 0, self.name, namePos, 5, 90, 0, {1, 1, 1, 1})
end

function GameManor:deleteShowName(rakssid)
    local namePos = ManorConfig:getNamePos(self.manorIndex)
    if namePos == nil then
        return
    end
    HostApi.deleteWallText(rakssid, namePos)
end

function GameManor:masterLogin()
    self.isMasterInGame = true
end

function GameManor:masterLogout()
    self.isMasterInGame = false
end

function GameManor:getMasterState()
    return self.isMasterInGame
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
            if ManorConfig:isInManor(self.manorIndex, player:getPosition()) == false then
                self:visitorLeaveHouse(userId)
            end
        end
    end
end

function GameManor:isEmptyVisitor()
    return next(self.visitor) == nil
end

function GameManor:removeFromWorld()
    self:deleteShowName(0)
end

function GameManor:getManorLevel()
    return self.level
end

function GameManor:isAlreadyMaxLevel()
    return self.level >= ManorLevelConfig:getMaxLevel()
end

function GameManor:getNextManorLevel()
    local level = self.level + 1
    if level > ManorLevelConfig:getMaxLevel() then
        level = ManorLevelConfig:getMaxLevel()
    end

    return level
end

function GameManor:upgradeLevel()
    self.level = self.level + 1
    if self.level > ManorLevelConfig:getMaxLevel() then
        self.level = ManorLevelConfig:getMaxLevel()
    end

    local preStartPos, preEndPos = ManorConfig:getOpenManorPos(self.level - 1,self.manorIndex, true)
    if not preStartPos or not preEndPos then
        return
    end

    local curStartPos, curEndPos = ManorConfig:getOpenManorPos(self.level,self.manorIndex, true)
    if not curStartPos or not curEndPos then
        return
    end

    SceneManager:levelUpFillAreaBaseBlock(preStartPos, preEndPos, curStartPos, curEndPos, self.level)
end