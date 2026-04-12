require "config.SkillEffectConfig"

BaseSkill = class()

BaseSkill.TARGET_OWN = "1"
BaseSkill.TARGET_TEAMMATE = "2"
BaseSkill.TARGET_ENEMY = "3"

function BaseSkill:ctor(config, player, initPos)
    self.uniqueId = nil
    self.id = config.id
    self.itemId = config.itemId
    self.createTime = os.time()
    self.length = config.length
    self.width = config.width
    self.height = config.height
    self.isMoment = config.isMoment
    self.effectNum = config.effectNum
    self.duration = config.duration
    self.effectCd = config.effectCd
    self.type = config.type
    self.potionLevel = config.potionLevel
    self.creatorUid = player.userId
    self.initPos = initPos
    self.targets = {}
    self:initArea()
    self:initTargetPlayers(config.targets)
    self:createSkillEffect(config.hitEffectName, config.hitEffectTime)
    self:onCreate()
end

function BaseSkill:getCreator()
    return PlayerManager:getPlayerByUserId(self.creatorUid)
end

function BaseSkill:initArea()
    self.minX = self.initPos.x - self.length
    self.minY = self.initPos.y - self.height
    self.minZ = self.initPos.z - self.width
    self.maxX = self.initPos.x + self.length
    self.maxY = self.initPos.y + self.height
    self.maxZ = self.initPos.z + self.width
end

function BaseSkill:inArea(position)
    return position.x >= self.minX and position.x <= self.maxX
            and position.y >= self.minY and position.y <= self.maxY
            and position.z >= self.minZ and position.z <= self.maxZ
end

function BaseSkill:initTargetPlayers(targets)
    local creator = self:getCreator()
    local players = PlayerManager:getPlayers()
    for i, target in pairs(targets) do
        if target == BaseSkill.TARGET_OWN then
            self.targets[#self.targets + 1] = creator.userId
        end
        if target == BaseSkill.TARGET_TEAMMATE then
            for _, player in pairs(players) do
                if player ~= creator and creator:getTeamId() == player:getTeamId() then
                    self.targets[#self.targets + 1] = player.userId
                end
            end
        end
        if target == BaseSkill.TARGET_ENEMY then
            for _, player in pairs(players) do
                if player:getTeamId() ~= creator:getTeamId() then
                    self.targets[#self.targets + 1] = player.userId
                end
            end
        end
    end
end

function BaseSkill:createSkillEffect(name, time)
    local creator = self:getCreator()
    if #name > 6 and time > 0 then
        EngineWorld:addSimpleEffect(name, self.initPos, creator:getYaw(), time)
    end
end

function BaseSkill:onCreate()
    SkillManager:addSkill(self)
    self:doAction()
    if self.isMoment then
        self:onRemove()
    end
end

function BaseSkill:onTick(tick)
    local creator = self:getCreator()
    local lifetime = os.time() - self.createTime
    if creator == nil or creator.isLife == false or lifetime >= self.duration then
        self:onRemove()
    end
    if lifetime > 0 and lifetime % self.effectCd == 0 then
        self:doAction()
    end
end

function BaseSkill:getLastTime()
    return self.duration - (os.time() - self.createTime)
end

function BaseSkill:getEffectPlayers()
    local maxNum = self.effectNum
    local players = {}
    for _, target in pairs(self.targets) do
        local player = PlayerManager:getPlayerByUserId(target)
        if player ~= nil and player.isLife then
            local position = player:getPosition()
            if self:inArea(position) then
                table.insert(players, player)
            end
        end
    end
    maxNum = math.min(maxNum, #players)
    local deleteNum = #players - maxNum
    while deleteNum > 0 do
        table.remove(players, math.random(1, #players))
        deleteNum = deleteNum - 1
    end
    return players
end

function BaseSkill:doAction()
    local players = self:getEffectPlayers()
    for _, player in pairs(players) do
        self:onPlayerEffected(player)
    end
end

function BaseSkill:onPlayerEffected(player)

end

function BaseSkill:onRemove()
    if self.uniqueId ~= nil then
        self:onDestroy()
        SkillManager:removeSkill(self)
        self.uniqueId = nil
    end
end

function BaseSkill:onDestroy()

end

function BaseSkill:telePos(player)
    local range = 0
    local position = self.initPos
    if player == nil then
        return
    end
    local pos, isAir, tpPos
    pos = VectorUtil.toBlockVector3(position.x, position.y, position.z)
    isAir, tpPos = SkillManager:checkTopAreaIsAir(pos)
    if isAir then
        player:teleportPos(tpPos)
        return
    end
    range = 1
    while not isAir and range <= 5 do
        if not isAir then
            pos = VectorUtil.toBlockVector3(position.x - range, position.y, position.z)
            isAir, tpPos = SkillManager:checkTopAreaIsAir(pos)
            if isAir then
                player:teleportPos(tpPos)
            end
        end
        if not isAir then
            pos = VectorUtil.toBlockVector3(position.x, position.y, position.z + range)
            isAir, tpPos = SkillManager:checkTopAreaIsAir(pos)
            if isAir then
                player:teleportPos(tpPos)
            end
        end
        if not isAir then
            pos = VectorUtil.toBlockVector3(position.x + range, position.y, position.z - range)
            isAir, tpPos = SkillManager:checkTopAreaIsAir(pos)
            if isAir then
                player:teleportPos(tpPos)
            end
        end
        if not isAir then
            pos = VectorUtil.toBlockVector3(position.x - range, position.y, position.z + range)
            isAir, tpPos = SkillManager:checkTopAreaIsAir(pos)
            if isAir then
                player:teleportPos(tpPos)
            end
        end
        if not isAir then
            pos = VectorUtil.toBlockVector3(position.x + range, position.y, position.z + range)
            isAir, tpPos = SkillManager:checkTopAreaIsAir(pos)
            if isAir then
                player:teleportPos(tpPos)
            end
        end
        if not isAir then
            pos = VectorUtil.toBlockVector3(position.x - range, position.y, position.z - range)
            isAir, tpPos = SkillManager:checkTopAreaIsAir(pos)
            if isAir then
                player:teleportPos(tpPos)
            end
        end
        if not isAir then
            range = range + 1
        end
    end
end

return BaseSkill