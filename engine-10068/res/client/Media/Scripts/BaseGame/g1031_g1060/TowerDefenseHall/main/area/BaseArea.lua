BaseArea = class("BaseClass")

function BaseArea:ctor(areaConfig)
    self.config = areaConfig
    self.Area = areaConfig.Area
    self.playerList = {} --{ tostring(userId) = 1}
    self.minPos = self.Area:getMinPos()
    self.maxPos = self.Area:getMaxPos()
    self.minWallPos = VectorUtil.newVector3i(self.minPos.x - 1, self.minPos.y - 1, self.minPos.z - 1)
    self.maxWallPos = VectorUtil.newVector3i(self.maxPos.x, self.maxPos.y, self.maxPos.z)
    self:init()
    self.entityId = self:createActor() or 0
end

function BaseArea:init()

end

function BaseArea:checkPlayerLevel(player)

end

function BaseArea:createActor()
    if isClient and #self.config.actor > 7 then
        return EngineWorld:addActorNpc(self.config.centrePos, self.config.yaw, self.config.actor, function(entity)
            entity:setSkillName("idle")
        end)
    end
end

function BaseArea:onClick(player)

end

function BaseArea:checkArea(player, x, y, z)
    local pos = VectorUtil.newVector3(x, y, z)
    if self.Area:inArea(pos) then
        if not self.playerList[tostring(player.userId)] then
            if self:playerIn(player) then
                self.playerList[tostring(player.userId)] = 1
            end
        end
    else
        self:isNear(player.userId, pos)
        if self.playerList[tostring(player.userId)] then
            if self:playerOut(player) then
                self.playerList[tostring(player.userId)] = nil
            end
        end
    end
end

function BaseArea:isNear(userId, pos)

end

function BaseArea:inNear(userId)

end

function BaseArea:playerIn(player)
    return true
end

function BaseArea:playerOut(player)
    return true
end

function BaseArea:openAirWall(isOpen)
    local oldBlockId = 0
    local newBlockId = 253
    if not isOpen then
        oldBlockId = 253
        newBlockId = 0
    end

    for x = self.minWallPos.x, self.maxWallPos.x do
        for y = self.minWallPos.y, self.maxWallPos.y do
            self:changeBlock(x, y, self.minWallPos.z, oldBlockId, newBlockId)
        end
    end

    for y = self.minWallPos.y, self.maxWallPos.y do
        for z = self.minWallPos.z, self.maxWallPos.z do
            self:changeBlock(self.minWallPos.x, y, z, oldBlockId, newBlockId)
        end
    end

    for x = self.minWallPos.x, self.maxWallPos.x do
        for z = self.minWallPos.z, self.maxWallPos.z do
            self:changeBlock(x, self.minWallPos.y, z, oldBlockId, newBlockId)
        end
    end

    for x = self.minWallPos.x, self.maxWallPos.x do
        for y = self.minWallPos.y, self.maxWallPos.y do
            self:changeBlock(x, y, self.maxWallPos.z, oldBlockId, newBlockId)
        end
    end

    for y = self.minWallPos.y, self.maxWallPos.y do
        for z = self.minWallPos.z, self.maxWallPos.z do
            self:changeBlock(self.maxWallPos.x, y, z, oldBlockId, newBlockId)
        end
    end

    for x = self.minWallPos.x, self.maxWallPos.x do
        for z = self.minWallPos.z, self.maxWallPos.z do
            self:changeBlock(x, self.maxWallPos.y, z, oldBlockId, newBlockId)
        end
    end
end

function BaseArea:changeBlock(x, y, z, oldBlockId, newBlockId)
    local pos = VectorUtil.newVector3i(x, y, z)
    if EngineWorld:getBlockId(pos) == oldBlockId then
        EngineWorld:setBlock(pos, newBlockId)
    end
end

function BaseArea:setEffect(open)
    if isClient then
        if open then
            if self.effectName then
                EngineWorld:removeSimpleEffect(self.effectName)
            end
            local effect = EngineWorld:addSimpleEffect(self.config.areaEffect, self.config.centrePos, 0, -1, tonumber(self.config.scale[1]), tonumber(self.config.scale[2]), tonumber(self.config.scale[3]))
            self.effectName = effect.mEffectName
        else
            EngineWorld:removeSimpleEffect(self.effectName)
        end
    end
end