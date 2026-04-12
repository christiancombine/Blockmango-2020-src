
BaseDeBuff = class()

function BaseDeBuff:ctor(player, target, config)
    self.creatorUid = player.userId
    self.targetUid = target.userId
    self.id = config.id
    self.duration = config.duration
    self.durationBk = config.duration -- Backup duration
    self.cdTime = config.cdTime
    self.value = config.value
    self.uniqueId = nil
    self.createTime = os.time()
    self.canAddTime = 0
    DeBuffManager:addDeBuff(self)
    self:onCreate()
end

function BaseDeBuff:onTick(ticks)
    local lifetime = os.time() - self.createTime

    if lifetime > 0 and lifetime % self.cdTime == 0 then
        self:doAction()
    end
    if lifetime >= self.duration then
        self:onRemove()
    end
end

function BaseDeBuff:onCreate()

end

function BaseSkill:doAction()

end

function BaseDeBuff:onRemove()
    if self.uniqueId ~= nil then
        self:onDestroy()
        DeBuffManager:removeDeBuff(self)
        self.uniqueId = nil
    end
end

function BaseDeBuff:onDestroy()

end

function BaseDeBuff:getCreator()
    return PlayerManager:getPlayerByUserId(self.creatorUid)
end

function BaseDeBuff:addDeBuffTime(player)
    self.creatorUid = player.userId
    local life_time = os.time() - self.createTime
    local last_time = self.duration - life_time
    local add_time = self.durationBk - last_time
    self.duration = self.duration + add_time
end

return BaseDeBuff