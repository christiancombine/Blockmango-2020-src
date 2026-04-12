---
--- Created by Jimmy.
--- DateTime: 2018/6/13 0013 12:35
---
GameSkill = class()

GameSkill.TARGET_OWN = "1"
GameSkill.TARGET_TEAMMATE = "2"
GameSkill.TARGET_ENEMY = "3"

GameSkill.EFFECT_TYPE_HEAL = 1
GameSkill.EFFECT_TYPE_DAMAGE = 2
GameSkill.EFFECT_TYPE_SPEED = 3
GameSkill.EFFECT_TYPE_DEFENSE = 4

function GameSkill:ctor(config, player, initPos)
    self.createTime = os.time()
    self.length = config.length
    self.width = config.width
    self.height = config.height
    self.isMoment = config.isMoment
    self.effectNum = config.effectNum
    self.duration = config.duration
    self.effectCd = config.effectCd
    self.effectType = config.effectType
    self.damage = config.damage
    self.effect = config.effect
    self.player = player
    self.initPos = initPos
    self.targets = {}
    self.records = {}
    self.area = {}
    self:createSkillEffect(config.effectId)
    self:initArea()
    self:initTargetPlayers(config.targets)
    self:onCreate()
end

function GameSkill:createSkillEffect(effectId)
    if effectId ~= 0 then
        EngineWorld:getWorld():addSkillEffect(effectId, self.initPos)
    end
end

function GameSkill:initArea()
    local matrix = {}
    matrix[1] = {}
    matrix[1][1] = self.initPos.x - self.length
    matrix[1][2] = self.initPos.y - self.height
    matrix[1][3] = self.initPos.z - self.width
    matrix[2] = {}
    matrix[2][1] = self.initPos.x + self.length
    matrix[2][2] = self.initPos.y + self.height
    matrix[2][3] = self.initPos.z + self.width
    self.area = Area.new(matrix)
end

function GameSkill:initTargetPlayers(targets)
    for i, target in pairs(targets) do
        local players = PlayerManager:getPlayers()
        if target == GameSkill.TARGET_OWN then
            self.targets[#self.targets + 1] = self.player.userId
        end
        if target == GameSkill.TARGET_TEAMMATE then
            for _, player in pairs(players) do
                if player ~= self.player and player:getTeamId() == self.player:getTeamId() then
                    self.targets[#self.targets + 1] = player.userId
                end
            end
        end
        if target == GameSkill.TARGET_ENEMY then
            for _, player in pairs(players) do
                if player:getTeamId() ~= self.player:getTeamId() then
                    self.targets[#self.targets + 1] = player.userId
                end
            end
        end
    end
end

function GameSkill:onCreate()
    SkillManager:addSkill(self)
    self:doAction()
    if self.isMoment then
        self:onRemove()
    end
end

function GameSkill:onTick(tick)
    local lifetime = os.time() - self.createTime
    if lifetime >= self.duration then
        self:onRemove()
    end
    if lifetime > 0 and lifetime % self.effectCd == 0 then
        self:doAction()
    end
end

function GameSkill:doAction()
    local maxNum = self.effectNum
    local players = {}
    for i, target in pairs(self.targets) do
        local player = PlayerManager:getPlayerByUserId(target)
        if player ~= nil then
            local position = player:getPosition()
            if self.area:inArea(position) then
                players[#players + 1] = player
            end
        end
    end
    maxNum = math.min(maxNum, #players)
    local deleteNum = #players - maxNum
    while deleteNum > 0 do
        table.remove(players, math.random(1, #players))
        deleteNum = deleteNum - 1
    end
    for i, player in pairs(players) do
        if self.effectType == GameSkill.EFFECT_TYPE_HEAL then
            player:addHealth(self.damage)
        end
        if self.effectType == GameSkill.EFFECT_TYPE_DAMAGE then
            local damage = (self.player.attackX * self.damage + self.player.damage - player.addDefense) * (1 - player.defense)
            damage = math.max(0.5, damage)
            if player:getHealth() <= damage and player:getHealth() > 0 then
                self.player:onKill()
            end
            player:subHealth(damage)
        end
        if self.effectType == GameSkill.EFFECT_TYPE_SPEED then
            player:setSpeedAddition(self.damage)
            self:addRecord(player)
        end
        if self.effectType == GameSkill.EFFECT_TYPE_DEFENSE then
            player:setAddDefense(self.damage)
            self:addRecord(player)
        end
        if self.effect.id ~= 0 then
            player:addEffect(self.effect.id, self.effect.time, self.effect.lv)
        end
    end
end

function GameSkill:addRecord(player)
    if self.records[tostring(player.userId)] == nil then
        self.records[tostring(player.userId)] = true
    end
end

function GameSkill:clearRecord()
    for userId, record in pairs(self.records) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player ~= nil then
            if self.effectType == GameSkill.EFFECT_TYPE_SPEED then
                player:setSpeedAddition(0)
            end
            if self.effectType == GameSkill.EFFECT_TYPE_DEFENSE then
                player:setAddDefense(0)
            end
        end
        record = nil
    end
    self.records = {}
end

function GameSkill:onRemove()
    self:clearRecord()
    SkillManager:removeSkill(self)
end
