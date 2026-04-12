BaseTower = class("BaseTower", BaseEntity)

function BaseTower:onCreate(pos, config, userId, facePos)
    self:init()
    self.pos = pos
    self.bulletPos = VectorUtil.newVector3(pos.x, pos.y + config.bulletYawY, pos.z)
    self.config = config
    self.userId = userId
    self.Hp = 0
    self.isLocked = false
    self.curAttackSpeed = config.AttackSpeed
    self.targets = {}
    self.bullets = {}
    self.auras = {}
    self.entityType = Define.EntityType.Tower

    self:initSkill()
    self:onPlace(pos, facePos)
    LuaEntityManager:addEntity(self)
    self:onTick()
    BaseGridListManager:addTower(self)

    math.randomseed(os.time())          -- 随机音效种子
end

function BaseTower:initSkill()
    self.attackBuffIds = {}
    for _, skillId in pairs(self.config.skillIds) do
        local skillConfig = SkillConfig:getConfigById(tonumber(skillId))
        if skillConfig then
            self.skillType = skillConfig.Type
            if skillConfig.Type == Define.SkillType.Aura then
                ---改为 new 一个Aura对象
                local aura = BaseAura.new(self.pos, tonumber(skillId), skillConfig)
                table.insert(self.auras, aura)
                BaseGridListManager:addAura(self.processId, aura)

            elseif skillConfig.Type == Define.SkillType.Attack then
                table.insert(self.attackBuffIds, skillConfig.BuffId)
            elseif skillConfig.Type == Define.SkillType.Crit then
                self.critProbability = skillConfig.probability
                self.critResult = skillConfig.result
                self.critEffect = skillConfig.triggerEffect
            elseif skillConfig.Type == Define.SkillType.Plunder then
                self.plunderProbability = skillConfig.probability
                self.plunderResult = skillConfig.result
                self.plunderEffect = skillConfig.triggerEffect
            end
        end
    end
end

function BaseTower:setRandom(seed)
    self.randomseed = seed
    math.randomseed(self.randomseed)
    self.randomList = {}
    self.randomIndex = 1
    for _ = 1, 1000 do
        table.insert(self.randomList, math.random(1, 100))
    end
end

function BaseTower:getSkillRandomList(count)
    local list = {}
    for _ = 1, count or 0 do
        local RandomValue = self.randomList[self.randomIndex] or 100
        self.randomIndex = math.fmod(self.randomIndex + 1, 1000)
        table.insert(list, RandomValue)
    end
    return list
end

function BaseTower:getSkillSuccess(probability)
    self.randomIndex = math.fmod(self.randomIndex + 1, 1000)
    local RandomValue = self.randomList[self.randomIndex] or 100
    if probability * 100 >= RandomValue then
        return true
    end
    return false
end

function BaseTower:onPlace(pos, facePos)
    local yaw = self:getYaw(facePos)
    self.entityId = EngineWorld:addActorNpc(pos, yaw or 0, self.config.ActorName, function(entity)
        entity:setEnableTracker(false)
        entity:setIgnoreFrustumCheck(true)
        entity:setRenderDistanceWeight(5)
        entity:setSkillName("idle")
        entity:setCanCollided(false)
        self.entity = entity
    end)
end

function BaseTower:addTarget(newMonster)
    local Index = 1
    for _, monster in pairs(self.targets) do
        if newMonster.progress > monster.progress then
            break
        end
        Index = Index + 1
    end
    table.insert(self.targets, Index, newMonster)
end

function BaseTower:removeTarget(monster)
    for i, target in pairs(self.targets) do
        if monster.entityId == target.entityId then
            table.remove(self.targets, i)
            return
        end
    end
end

function BaseTower:checkPropertyChange()
    self.curAttackSpeed = self.config.AttackSpeed / self.properties["attackSpeedPercent"]
    if self.config.MinAttackSpeed > self.curAttackSpeed then
        self.curAttackSpeed = self.config.MinAttackSpeed
    end
    LevelHelper:checkUpgrade(self)
end

function BaseTower:onTick()
    if self.entityId == 0 then
        return
    end

    if self.properties["isControl"] <= 0 then
        self:checkStartAttack()
    else
        ---播idle动作
        ---播眩晕特效
        self.entity:setSkillName("idle")
        LuaTimer:cancel(self.timerKey)
        self.timerKey = nil
    end

    LuaTimer:schedule(function()
        self:onTick()
    end, 100)
end

function BaseTower:checkStartAttack()
    if #self.targets > 0 then
        self:changeRotationYaw()
        if not self.timerKey then
            --当走弹道同步逻辑是,客户端不跑(needSyn = true)
            if not isClient or not needSyn then
                self.timerKey = LuaTimer:scheduleTimer(function(AttackSpeed)
                    self:checkAttack(AttackSpeed)
                end, self.curAttackSpeed, nil, self.curAttackSpeed)
                self:checkAttack(self.curAttackSpeed)
            end
        end
    elseif isClient and not self.lockAttackSkill then
        self.entity:setSkillName("idle")
    end
end

function BaseTower:checkAttack(nowAttackSpeed)
    if self.entityId == 0 then
        return
    end
    if #self.targets > 0 then
        self:attack(self:findTargets(), self.config.ejectionsCount)
        if self.curAttackSpeed ~= nowAttackSpeed and self.timerKey then
            LuaTimer:cancel(self.timerKey)
            self.timerKey = LuaTimer:scheduleTimer(function(AttackSpeed)
                self:checkAttack(AttackSpeed)
            end, self.curAttackSpeed, nil, self.curAttackSpeed)
        end
        return
    end
    LuaTimer:cancel(self.timerKey)
    self.timerKey = nil
end

function BaseTower:findTargets()
    local targets = {}

    for _, target in pairs(self.targets) do
        if #targets >= self.config.scatterCount then
            break
        end
        if target.config.isTaunt == 1 and self:canAttack(target) then
            table.insert(targets, target)
        end
    end

    for _, target in pairs(self.targets) do
        if #targets >= self.config.scatterCount then
            break
        end
        if target.config.isTaunt == 0 and self:canAttack(target) then
            table.insert(targets, target)
        end
    end
    return targets
end

function BaseTower:attack(targets, ejectionsCount)
    local bulletList = {}
    self.faceTarget = nil
    for _, target in pairs(targets) do
        local bullet = BaseBullet.new(self, target, ejectionsCount)
        table.insert(bulletList, bullet)
    end
    if #bulletList == 0 then
        return
    end
    self.faceTarget = bulletList[1].target

    --调前播放攻击动作
    if isClient then
        self.entity:setSkillName("attack")
        self.lockAttackSkill = true
    end
    --若上一次弹道未发射
    self:checkLastBullets()

    --初始化彈道列表
    self.bulletList = bulletList
    --延迟发射弹道,达到攻击前摇的效果
    self.shootTimerKey = LuaTimer:schedule(function(bulletList)
        if isClient and self.entityId ~= 0 then
            self.lockAttackSkill = false
            SoundUtil.playSound3D(tonumber(self.config.AttackSound[math.random(#self.config.AttackSound)]),
                    self.pos.x, self.pos.y, self.pos.z)

        end
        for _, bullet in pairs(bulletList) do
            bullet:shoot(self.bulletPos, false)
        end
    end, self.config.AttackTime, nil, bulletList)
end

function BaseTower:checkLastBullets()
    --检查上一次弹道列表是否已经发射完
    for _, bullet in pairs(self.bulletList or {}) do
        bullet:shoot(self.bulletPos, false)
    end
end

function BaseTower:ejectionsAttack(target, preTarget, ejectionsCount, hadAttackList)
    local startPos = preTarget:getCentrePos()
    local bullet = BaseBullet.new(self, target, ejectionsCount, hadAttackList)
    bullet:shoot(startPos, true)
end

function BaseTower:canAttack(target)
    if target.entityId == 0 or target.futureHp <= 0 then
        return false
    end
    return true
end

function BaseTower:addEffect(_, effect, duration, scale)
    if isClient and GUIMain.openBuff then
        local effectPos = VectorUtil.newVector3(self.pos.x, self.pos.y + 0.1, self.pos.z)
        local effect = EngineWorld:addSimpleEffect(effect, effectPos, 0, duration, scale, scale, scale)
        return effect.mEffectName
    end
end

function BaseTower:removeEffect(name)
    if isClient then
        EngineWorld:removeSimpleEffect(name)
    end
end

function BaseTower:getAttackValue()
    return self.config.Attack * self.properties["attackPercent"] + self.properties["attack"]
end

function BaseTower:getAttackSpeedValue()
    return self.config.AttackSpeed / self.properties["attackSpeedPercent"]
end

function BaseTower:getYaw(facePos)
    local vector = math.atan2(facePos.x - self.pos.x, facePos.z - self.pos.z)
    return vector / math.pi * -180
end

function BaseTower:changeRotationYaw()
    local target = self.faceTarget
    if isClient then
        if target and target.entityId ~= 0 then
            local entity = EngineWorld:getEntity(self.entityId)
            local newYaw = self:getYaw(target:getPosition())
            local oldYaw = entity.rotationYaw
            while math.abs(oldYaw - newYaw) > 180 do
                if oldYaw > newYaw then
                    newYaw = newYaw + 360
                else
                    newYaw = newYaw - 360
                end
            end
            entity.rotationYaw = newYaw
        end
    end
end

function BaseTower:onDie()
    for _, aura in pairs(self.auras) do
        aura:onDestroy()
    end
    self:checkLastBullets()
    LuaEntityManager:subEntity(self.entityId)
    EngineWorld:removeEntity(self.entityId)
    self.entity = nil
    self.entityId = 0
    BaseGridListManager:subTower(self)
end