local BaseActor = class()

local function createActor(position, yaw, actorName, name, effectName)
    if actorName == "" then
        return nil
    end
    local entityId = EngineWorld:addActorNpc(position, yaw, actorName, function(entity)
        entity:setHeadName(name or "")
        entity:setSkillName("idle")
        entity:setHaloEffectName(effectName or "")
        entity:setCanCollided(false)
    end)
    return entityId
end

local function removeActor(entityId)
    ActorManager:removeActor(entityId)
    EngineWorld:removeEntity(entityId)
    return true
end

function BaseActor:ctor(position, yaw, actorName, name, effectName, child)
    local actor = self
    if child then
        actor = child
    end
    if actor.entityId then
        actor:destroyActor()
    end
    actor.position = VectorUtil.toVector3(position)
    actor.name = name or ""
    actor.actorName = actorName or ""
    actor.effectName = effectName or ""
    actor.yaw = yaw
    actor.entityId = createActor(actor.position, actor.yaw, actor.actorName, actor.name, actor.effectName)
end

function BaseActor:onTick(ticks)

end

function BaseActor:clickedActor(player)
end

function BaseActor:destroyActor()
    return removeActor(self.entityId)
end

function BaseActor:getPosition()
    local entity = EngineWorld:getEntity(self.entityId)
    if entity == nil then
        return VectorUtil.newVector3(0, 0, 0)
    end
    return entity:getPosition()
end

return BaseActor
--endregion
