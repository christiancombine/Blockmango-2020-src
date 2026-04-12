BaseBuff = class()

function BaseBuff:ctor(creator, target, config)
    self.creatorUid = tostring(creator)
    self.targetUid = tostring(target)
    self.id = config.id
    self.type = config.type
    self.duration = config.duration
    self.durationBk = config.duration -- Backup duration
    self.cdTime = config.cdTime
    self.value = config.value
    self.limit = config.limit
    self.overlayRule = config.overlayRule
    self.uniqueId = nil
    self.createTime = os.time()
    self.canAddTime = 0
    self.lifetime = 0
    BuffManager:addBuff(self)
    self:onCreate()
end

function BaseBuff:onTick(ticks)
    if self.duration == -1 then
        self.lifetime = self.lifetime + 1
        if self.lifetime == 0 or self.lifetime % self.cdTime == 0 then
            self:doAction()
        end
        return
    end

    self.lifetime = self.lifetime + 1
    if self.lifetime <= self.duration and self.lifetime % self.cdTime == 0 then
        self:doAction()
    end
    if self.lifetime >= self.duration then
        self:onRemove()
    end

end

function BaseBuff:onCreate()

end

function BaseBuff:getBuffInfo()
    local data = {}
    data.creatorUid = self.creatorUid
    data.targetUid = self.targetUid
    data.id = self.id
    data.type = self.type
    data.duration = self.duration
    data.durationBk = self.durationBk
    data.cdTime = self.cdTime
    data.value = self.value
    data.limit = self.limit
    data.overlayRule = self.overlayRule
    data.uniqueId = self.uniqueId
    data.createTime = self.createTime
    data.canAddTime = self.canAddTime
    data.lifetime = self.lifetime
    return data
end

function BaseBuff:onReLoad(data, userId)
    self.creatorUid = tostring(data.creatorUid)
    self.targetUid = tostring(userId)
    self.id = data.id
    self.type = data.type
    self.duration = data.duration
    self.durationBk = data.duration -- Backup duration
    self.cdTime = data.cdTime
    self.value = data.value
    self.limit = data.limit
    self.overlayRule = data.overlayRule
    self.uniqueId = data.uniqueId
    self.createTime = os.time()
    self.canAddTime = 0
    self.lifetime = data.lifetime
    BuffManager:addBuff(self)
    self:onCreate()
end

function BaseBuff:doAction()

end

function BaseBuff:onRemove()
    if self.uniqueId ~= nil then
        self:onDestroy()
        BuffManager:removeBuff(self.creatorUid, self.uniqueId, self.id)
        local player = PlayerManager:getPlayerByUserId(self.targetUid)
        if player ~= nil then
            GamePacketSender:sendWalkingDeadBuffInfo(player.rakssid, self.type, 0)
        end
    end
    self.uniqueId = nil
end

function BaseBuff:onDestroy()

end

function BaseBuff:getCreator()

end

function BaseBuff:addBuffTime(creator)
    self.creatorUid = creator
    self.duration = self.duration + self.lifetime
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    GamePacketSender:sendWalkingDeadBuffInfo(player.rakssid,self.type, self.duration - self.lifetime)
end

return BaseBuff