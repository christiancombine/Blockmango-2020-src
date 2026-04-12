GamePet = class("GamePet")

---masterId: playerUserId
---petId: petConfigId
local function calculateDistance(pos1, pos2)
    return math.sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y) + (pos1.z - pos2.z) * (pos1.z - pos2.z))
end

local function getYaw(fromPos, facePos)
    local pos = fromPos
    local vector = math.atan2(facePos.x - pos.x, facePos.z - pos.z)
    return vector / math.pi * 180 * -1

end

local function getRelativePos(fromPos, yaw, xOffset, yOffset, zOffset)
    local x = -math.sin(yaw * MathUtil.DEG2RAD)
    local z = math.cos(yaw * MathUtil.DEG2RAD)
    local frontPos = VectorUtil.newVector3(fromPos.x + x * xOffset, fromPos.y + yOffset, fromPos.z + z * xOffset)
    local leftYaw = yaw + 90
    x = -math.sin(leftYaw * MathUtil.DEG2RAD)
    z = math.cos(leftYaw * MathUtil.DEG2RAD)
    return VectorUtil.newVector3(frontPos.x + x * zOffset, frontPos.y, frontPos.z + z * zOffset)
end

function GamePet:ctor(master, petId)
    self.petId = petId
    self.config = CustomShopPet.Settings[petId]
    self.master = master
    if self.master and self.config then
        self:createPet()
        self.curSpeed = 0
        self.tick = 0
        self.masterPos = self.master.Player:getPosition()
        self.masterPos.y = self.masterPos.y + 2
        self.curPos = self.entity.position
    end
end
function GamePet:createPet()
    local pos = self.master.Player:getPosition()
    pos.y = pos.y + 1
    local yaw = self.master.Player:getYaw()
    self.entityId = EngineWorld:addActorNpc(pos, yaw, self.config.ActorName, function(entity)
        --entity:setActorBody("body")
        --entity:setActorBodyId("20101")
        self.entity = entity
        entity:setFlying(true)
        entity:setCanObstruct(false)
        entity:setCanCollided(false)
        entity:setSkillName("idle")
        entity:setBoolProperty("banClicked", true)
        entity:setBoolProperty("banAABBCheck", true)
        entity:setScale(self.config.scale)
    end)
    self.actionKey = LuaTimer:scheduleTimer(function()
        self:onTaunt()
    end, 8000)
end

function GamePet:followMaster()
    --moveEntity
end

function GamePet:moveToMaster()
    --self.curSpeed = self.curSpeed + 0.001
    local masterYaw = self.master.Player.rotationYaw
    if self.tick > 10 then
        self.tick = 0
        local newPos = self.master.Player:getBottomPos()
        if calculateDistance(newPos, self.masterPos) > 0.1 then
            self.masterPos = getRelativePos(newPos, masterYaw, self.config.xOffset, self.config.yOffset, self.config.zOffset)
        end
    end
    self.tick = self.tick + 1
    local motion = VectorUtil.sub3(self.masterPos, self.curPos)
    local distance = math.sqrt(math.pow(motion.x, 2) + math.pow(motion.y, 2) + math.pow(motion.z, 2))
    self.curSpeed = math.floor(distance / 15 * 1000) / 1000
    --self.curSpeed = distance / 6
    if self.curSpeed < 0.005 then
        self.curSpeed = 0.005
        self:playSkill("idle")
    else
        self:playSkill("run")
    end
    if distance > 0.2 then
        self:changeRotationYaw(self.masterPos)
    else
        self:setYaw(masterYaw)
    end

    if distance > self.curSpeed then
        motion.x = motion.x / distance * self.curSpeed
        motion.y = motion.y / distance * self.curSpeed
        motion.z = motion.z / distance * self.curSpeed
        --else
        --    self.curSpeed = 0.02
    end
    self.curPos = VectorUtil.add3(self.curPos, motion)
    self.entity:setPosition(self.curPos)
    --self.entity:moveEntity(motion)
end

function GamePet:onUpdate()
    if not self.master.Player then
        if self.entity ~= nil then
            self:destroy()
        end
        return
    end
    if self.master ~= PlayerManager:getClientPlayer() and self.master.Player:getActivePotionEffectById(14) then
        if self.entity ~= nil then
            self:destroy()
        end
        return
    end
    local entity = EngineWorld:getEntity(self.entityId)
    if not entity then
        self.entity = nil
    end
    if self.entity == nil then
        self:createPet()
    end
    self:moveToMaster()
end

function GamePet:onTaunt()
    self:playSkill("taunt")
    self.lockAction = true
    local tauntTime = self.config.tauntTime
    local entityRender = EntityRenderManager.Instance()
    if entityRender and self.entity then
        local actor = entityRender:getEntityActor(self.entity)
        if actor then
            tauntTime = actor:GetSkillTimeLength(self.config.uiSkillName)
        end
    end
    LuaTimer:cancel(self.lockActionKey or 0)
    self.lockActionKey = LuaTimer:schedule(function()
        self.lockAction = false
    end, tauntTime)
end

function GamePet:playSkill(skillName)
    if self.entity and not self.lockAction then
        self.entity:setSkillName(skillName)
    end
end

function GamePet:changeRotationYaw(facePos)
    local newYaw = getYaw(self.curPos, facePos)
    self:setYaw(newYaw)
end

function GamePet:setYaw(newYaw)
    local oldYaw = self.entity.rotationYaw
    while math.abs(oldYaw - newYaw) > 180 do
        if oldYaw > newYaw then
            newYaw = newYaw + 360
        else
            newYaw = newYaw - 360
        end
    end

    local angle = newYaw - oldYaw
    if angle > 3 then
        angle = 3
    end
    if angle < -3 then
        angle = -3
    end
    self.entity.rotationYaw = oldYaw + angle
end

function GamePet:destroy()
    EngineWorld:removeEntity(self.entityId)
    LuaTimer:cancel(self.actionKey or 0)
    self.entityId = 0
    self.entity = nil
end

