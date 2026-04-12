BaseGrid = class("BaseGrid")

function BaseGrid:ctor(progress)
    self.attackerList = {}
    self.monsterAura = {}
    self.towerAura = {}
    self.monsterList = {}
    self.tower = nil
    self.progress = progress or 0
end

function BaseGrid:setTower(tower)
    if self.tower then
        self.tower:changeAuraBuff({})
    end
    self.tower = tower
    if self.tower then
        self.tower:changeAuraBuff(self:getAuraBuff(self.towerAura))
    end
end

function BaseGrid:addAttackerToList(tower)
    table.insert(self.attackerList, tower)
    self:updateMonsterList()
    for _, monster in pairs(self.monsterList) do
        tower:addTarget(monster)
    end
end

function BaseGrid:addAuraToList(aura)
    if aura.targetType == Define.SkillTarget.Monster then
        table.insert(self.monsterAura, aura)
        self:onAuraChange()
    elseif aura.targetType == Define.SkillTarget.Tower then
        table.insert(self.towerAura, aura)
        if self.tower then
            self.tower:changeAuraBuff(self:getAuraBuff(self.towerAura))
        end
    end
end

function BaseGrid:removeAura(aura)
    --将towerid放到auraBuff表中
    if aura.targetType == Define.SkillTarget.Monster then
        for index, listAura in pairs(self.monsterAura) do
            if listAura == aura then
                table.remove(self.monsterAura, index)
            end
        end
        self:onAuraChange()
    elseif aura.targetType == Define.SkillTarget.Tower then
        for index, listAura in pairs(self.towerAura) do
            if listAura == aura then
                table.remove(self.towerAura, index)
            end
        end
        if self.tower then
            self.tower:changeAuraBuff(self:getAuraBuff(self.towerAura))
        end
    end
end

function BaseGrid:onMonsterInto(monster)
    --当怪物进入grid，先将怪物加入monsterList，然后将怪物加入塔的攻击列表，再将本格子中的光环给怪物
    self:updateMonsterList()
    table.insert(self.monsterList, monster)
    monster.curGrid = self
    monster.progress = self.progress
    for _, tower in pairs(self.attackerList) do
        if tower ~= nil then
            tower:addTarget(monster)
        end
    end
    monster:changeAuraBuff(self:getAuraBuff(self.monsterAura))
end

function BaseGrid:onMonsterLevel(levelMonster)
    --当怪物离开grid，先将怪物移除monsterList，然后将怪物从塔的攻击列表移除
    self:updateMonsterList()
    for i, monster in pairs(self.monsterList) do
        if monster.entityId == levelMonster.entityId then
            table.remove(self.monsterList, i)
            break
        end
    end

    for i = #self.attackerList, 1, -1 do
        if self.attackerList[i].entityId == 0 then
            table.remove(self.attackerList, i)
            i = i + 1
        else
            local tower = self.attackerList[i]
            if tower ~= nil then
                tower:removeTarget(levelMonster)
            end
        end
    end
end

function BaseGrid:onAuraChange()
    if #self.monsterList > 0 then
        self:updateMonsterList()
        --取光环buff列表
        local auraList = self:getAuraBuff(self.monsterAura)

        --传一个buff列表给怪物
        for _, monster in pairs(self.monsterList) do
            monster:changeAuraBuff(auraList)
        end
    end
end

function BaseGrid:getAuraBuff(List)
    local auraList = {}
    for _, aura in pairs(List) do
        local buffId = aura.config.BuffId
        local buffConfig = BuffConfig:getConfigById(buffId)
        if buffConfig.classLevel == -1 then
            auraList[tostring(buffId)] = (auraList[tostring(buffId)] or 0) + 1
        else
            auraList[tostring(buffId)] = 1
        end
    end
    local buffConfigs = {}
    for buffId, _ in pairs(auraList) do
        local config = BuffConfig:getConfigById(tonumber(buffId))
        local classConfig = buffConfigs[tostring(config.classId)]
        if classConfig then
            if classConfig.classLevel >= config.classLevel then
                auraList[tostring(config.Id)] = 0
                config = classConfig
            else
                auraList[tostring(classConfig.Id)] = 0
            end
        end
        buffConfigs[tostring(config.classId)] = config
    end
    return auraList
end

function BaseGrid:updateMonsterList()
    for i = #(self.monsterList), 1, -1 do
        if self.monsterList[i].entityId == 0 then
            table.remove(self.monsterList, i)
            i = i + 1
        end
    end
end

function BaseGrid:updateGrid()
    self:updateAuraList(self.monsterAura)
    self:updateAuraList(self.towerAura)
    if self.tower then
        self.tower:changeAuraBuff(self:getAuraBuff(self.towerAura))
    end
end

function BaseGrid:updateAuraList(List)
    for index, tower in pairs(List) do
        if tower.entityId == 0 then
            table.remove(List, index)
            break
        end
    end
end

function BaseGrid:resetGrid()
    if self.tower ~= nil and not isClient then
        self.tower:onDie()
    end
end