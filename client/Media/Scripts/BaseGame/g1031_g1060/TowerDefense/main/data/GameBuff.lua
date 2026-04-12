GameBuff = class("GameBuff")

local index = 0

function GameBuff:ctor(target, buffId, userId)
    self.target = target
    self.userId = userId
    self.config = BuffConfig:getConfigById(buffId)
    self.effectName = nil
    if self:insert() then
        self:doBuff()
        self:updateDuration()
    end
end

--更新buff时间
function GameBuff:updateDuration()
    --更新特效时间
    self.effectName = self.target:addEffect(tostring(self.target.entityId) .. "#" .. tostring(self.config.Id), self.config.targetEffect, self.config.duration, 1)
    --启动伤害触发器
    if not self.timerRecoveryKey and self.config.recoveryInterval > 0 and self.target.__cname == "GameMonster" then
        self.timerRecoveryKey = LuaTimer:scheduleTimer(function(target, config)
            local recoveryValue = math.floor(config.recovery + config.recoveryCurPercent * target.Hp + config.recoveryMaxPercent * target.config.Hp)
            target:hurtFromBuff(recoveryValue, config.recoveryColor, self.userId)
        end, self.config.recoveryInterval, nil, self.target, self.config)
    end
    --启动自毁定时器
    LuaTimer:cancel(self.timerKey or 0)
    if self.config.duration > 0 then
        self.timerKey = LuaTimer:schedule(function()
            self:remove()
        end, self.config.duration)
    end
end

--目标增加属性
function GameBuff:doBuff()
    for key, value in pairs(self.config.properties) do
        self.target.properties[key] = self.target.properties[key] + value
    end
    self.target:checkPropertyChange()
end

--目标移除属性
function GameBuff:unDoBuff()
    for key, value in pairs(self.config.properties) do
        self.target.properties[key] = self.target.properties[key] - value
        if math.abs(self.target.properties[key]) < 0.000001 then
            --精度丢失
            self.target.properties[key] = 0
        end
    end
    self.target:checkPropertyChange()
end

function GameBuff:insert()
    --判断目标是否免疫
    for _, immunityBuffClass in pairs(self.target.config.immunityBuffClasses or {}) do
        if self.config.classId == tonumber(immunityBuffClass) then
            return
        end
    end

    index = index + 1
    self.index = index
    --同类型buff 互斥逻辑
    local list = self.target.buffList[tostring(self.config.classId)] or {}
    if self.config.classLevel ~= -1 and list[1] then
        if list[1].config.classLevel < self.config.classLevel then
            list[1]:remove()
        else
            if list[1].config.classLevel == self.config.classLevel then
                list[1]:updateDuration()
            end
            return false
        end
    end
    --插入buff
    table.insert(list, self)
    self.target.buffList[tostring(self.config.classId)] = list
    return true
end

function GameBuff:remove()
    if self.isRemove then
        return
    end
    self:unDoBuff()
    LuaTimer:cancel(self.timerRecoveryKey or 0)
    local list = self.target.buffList[tostring(self.config.classId)] or {}
    for key, buff in pairs(list) do
        if buff.index == self.index then
            table.remove(list, key)
            break
        end
    end
    local needRemoveEffect = true
    for _, buff in pairs(list) do
        if buff.config.Id == self.config.Id then
            needRemoveEffect = false
            break
        end
    end
    if needRemoveEffect and self.effectName then
        self.target:removeEffect(self.effectName, self.config.targetEffect)
    end
    self.isRemove = true
end
