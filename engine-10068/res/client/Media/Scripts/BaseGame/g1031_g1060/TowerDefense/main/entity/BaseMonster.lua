local function distanceTo(pos1, pos2)
    local dis = math.sqrt(math.pow(pos1.x - pos2.x, 2) + math.pow(pos1.z - pos2.z, 2))
    --print(dis)
    return dis
end

BaseMonster = class("BaseMonster", BaseEntity)

function BaseMonster:onCreate(config, path, processId)
    self:init()
    self.config = config
    self.Hp = self.config.Hp
    self.futureHp = config.Hp
    self.oldSpeed = self.config.Speed * self.properties["speedPercent"]
    self.skillList = {}
    self.progress = 0
    self.path = path
    self.processId = processId
    self.curGrid = nil
    self.status = Define.MonsterStatus.Normal
    self:createEntity()
    self.path:addMonster(self)
    self:initSkillList()
end

function BaseMonster:createEntity()
    local pos = VectorUtil.toVector3(self.path.startPos)
    pos.x = pos.x + 0.5
    pos.z = pos.z + 0.5
    local actorName = self.config.Actor
    if isClient and not GUIMain.openMonster then
        actorName = ""
    end
    self.entityId = EngineWorld:addRailCar(pos, self.path.startYaw, actorName, true, function(entity)
        entity:setEnableTracker(false)
        entity:setNeedPlayedGuide(false)
        entity:setYOffsetForBloodStrip(0.25)
        entity:setCurSpeed(self.config.Speed)
        entity:setBoolProperty("banClicked", true)
        self.entity = entity
    end)
    self.entity:setHealth(self.Hp)
    self.entity:setMaxHealth(self.Hp)
end

function BaseMonster:onTick()
    if self:checkProgress() == false then
        return
    end

    if self.properties["isControl"] > 0 then
        self.status = Define.MonsterStatus.Controlled
    elseif self.properties["isControl"] <= 0 and self.status == Define.MonsterStatus.Controlled then
        self.status = Define.MonsterStatus.Normal
    end

    ---cd计算
    self:calculateSkillCd()

    ---根据自己的状态，执行不同的逻辑
    if self.status == Define.MonsterStatus.Normal then
        ---技能释放判定
        self:tryUseSkill()

    elseif self.status == Define.MonsterStatus.Guide then
        ---时间计算
        ---引导效果判定
        ---结束判定
        self:onGuideSkill()
    elseif self.status == Define.MonsterStatus.Controlled then
        ---打断判定
        ---结束判定
        self:onSkillBeBreak()
    end
end

function BaseMonster:initSkillList()
    for _, skillId in pairs(self.config.skills) do
        local skillConfig = ActiveSkillConfig:getConfigById(tonumber(skillId))
        if skillConfig then
            local skill = {
                skillId = skillId,
                config = skillConfig,
                isEffective = false,
                status = Define.SkillStatus.CD,
                cd = skillConfig.InitCd * 1000,
                guideTime = skillConfig.GuideTime * 1000,
                skillNum = skillConfig.Num,
            }
            table.insert(self.skillList, skill)
        end
    end
end

function BaseMonster:calculateSkillCd()
    for _, skill in pairs(self.skillList) do
        if skill.cd > 0 then
            skill.cd = math.max(skill.cd - Define.PathManagerTickPeriod, 0)
        end

        if skill.cd == 0 and skill.status == Define.SkillStatus.CD then
            skill.status = Define.SkillStatus.Ready
        end

        if skill.status == Define.SkillStatus.Guide then
            skill.guideTime = math.max(skill.guideTime - Define.PathManagerTickPeriod, 0)
        end
    end
end

function BaseMonster:tryUseSkill()
    for _, skill in pairs(self.skillList) do
        if skill.status == Define.SkillStatus.Ready and skill.skillNum > 0 then
            self:onUseSkill(skill)
            break
        end
    end
end

function BaseMonster:onUseSkill(skill)
    self.status = Define.MonsterStatus.Guide

    skill.status = Define.SkillStatus.Guide
    skill.cd = skill.config.Cd * 1000
    skill.skillNum = skill.skillNum - 1

    ---站原地引导
    if self.properties["speedPercent"] >= 0 then
        self.properties["speedPercent"] = self.properties["speedPercent"] - 999
        self:checkPropertyChange()
    end
    if isClient then
        self.entity:setSkillName("attack2")
    end

    ---即时生效的技能
    if not skill.isEffective and skill.guideTime == 0 then
        ---实现技能效果
        local aura = BaseAura.new(self.entity:getPosition(), tonumber(skill.skillId), skill.config)
        BaseGridListManager:addAura(self.processId, aura)

        skill.isEffective = true
        self:onSkillEnding(skill)
    end
end

function BaseMonster:onGuideSkill()
    for _, skill in pairs(self.skillList) do
        if skill.status == Define.SkillStatus.Guide then
            if not skill.isEffective and skill.config.GuideTime * 1000 - skill.guideTime >= skill.config.DelayTime * 1000 then
                ---实现技能效果
                self.aura = BaseAura.new(self.entity:getPosition(), tonumber(skill.skillId), skill.config)
                BaseGridListManager:addAura(self.processId, self.aura)
                skill.isEffective = true
            end

            if skill.guideTime == 0 then
                self:onSkillEnding(skill)
            end
            break
        end
    end
end

function BaseMonster:onSkillEnding(skill)
    self.properties["speedPercent"] = self.properties["speedPercent"] + 999
    self:checkPropertyChange()
    self.status = Define.MonsterStatus.Normal
    self.aura = nil

    if isClient then
        self.entity:setSkillName("")
    end

    skill.status = Define.SkillStatus.CD
    skill.guideTime = skill.config.GuideTime * 1000
    skill.isEffective = false
end

function BaseMonster:onSkillBeBreak()
    for _, skill in pairs(self.skillList) do
        if skill.status == Define.SkillStatus.Guide then
            self:onSkillEnding(skill)
        end
    end

    if self.aura then
        self.aura:onDestroy()
    end
end

function BaseMonster:checkPropertyChange()
    local curSpeed = self.config.Speed * self.properties["speedPercent"]
    if curSpeed < 0 then
        curSpeed = 0
    end
    if self.entityId ~= 0 and self.oldSpeed ~= curSpeed then
        self.oldSpeed = curSpeed
        self.entity:setCurSpeed(curSpeed)
    end
end

function BaseMonster:checkProgress()
    if self.entityId == 0 then
        return false
    end
    local position = self:getPosition()
    local newPos = math.floor(position.x) .. "#" .. math.floor(position.z)
    local oldPos = self.pos
    if newPos ~= oldPos then
        BaseGridListManager:intoGrid(self, newPos, oldPos, self.processId)
        self.pos = newPos
    end
    self.progress = self.curGrid.progress + distanceTo(self.curGrid.enterPos, self:getPosition())
    self:checkAttackHome()
    return self.progress
end

function BaseMonster:checkAttackHome()
    if BaseGridListManager:GetPathMaxProgress(self.path.Id, self.processId) then
        if self.progress >= BaseGridListManager:GetPathMaxProgress(self.path.Id, self.processId) or self.progress == 0 then
            self:attackHome()
            self:onDie()
        end
    end
end

function BaseMonster:attackHome()

end

function BaseMonster:hurt(damage, buffIds, attackerId, dieCallback)
    for _, buffId in pairs(buffIds) do
        local buffConfig = BuffConfig:getConfigById(buffId)
        if buffConfig then
            GameBuff.new(self, buffId, attackerId)
        end
    end
    if damage <= 0 then
        return
    end
    if isClient then
        if self.entityId ~= 0 then
            self.entity:setHurting(true)
            LuaTimer:schedule(function()
                if self.entityId ~= 0 then
                    self.entity:setHurting(false)
                end
            end, 250)
        end
    end
    self:subHp(damage, attackerId, dieCallback)
end

function BaseMonster:hurtFromBuff(recoveryValue, color, attackerId)
    if self.entity == nil or self.entityId == 0 or self.Hp <= 0 then
        return
    end
    self:showFloatNumber(math.abs(recoveryValue), color)
    --若回血buff
    if recoveryValue > self.config.Hp - self.Hp then
        recoveryValue = self.config.Hp - self.Hp
    end
    --当没有 attackerId时,伤害不致死
    if not attackerId and recoveryValue + self.Hp <= 0 then
        recoveryValue = (self.Hp - 1) * -1
    end
    --计算预判血量
    self.futureHp = self.futureHp + recoveryValue
    self:subHp(recoveryValue * -1, attackerId)
end

function BaseMonster:showFloatNumber(damage, color, scale)

end

function BaseMonster:subHp(damage, attackerId, dieCallback)
    self.Hp = self.Hp - damage
    if self.entityId ~= 0 then
        self.entity:setHealth(self.Hp)
    end
    if self.Hp <= 0 then
        if dieCallback then
            dieCallback()
        end
        if isClient then

        else
            self:killed(attackerId)
        end
        self:onDie()
    end
end

function BaseMonster:getPosition()
    return self.entity:getPosition()
end

function BaseMonster:getCentrePos()
    local pos = self:getPosition()
    pos.y = pos.y + self.entity:getHeight() * self.entity:getScale() / 2
    return pos
end

function BaseMonster:getDefenseValue()
    return self.config.Defense * self.properties["defensePercent"] + self.properties["defense"]
end

function BaseMonster:onDie()
    LuaTimer:cancel(self.recoveryTimer)
    EngineWorld:removeEntity(self.entityId)
    self.path:subMonster(self.entityId)
    BaseGridListManager:intoGrid(self, nil, self.pos, self.processId)
    self.entity = nil
    self.entityId = 0
end