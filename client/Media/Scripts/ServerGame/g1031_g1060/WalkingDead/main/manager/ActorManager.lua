local ActorCache = {}

local function isPosExist(pos)
    for _, actor in pairs(ActorCache) do
        if VectorUtil.equal(actor.position, VectorUtil.toVector3(pos)) then
            return true
        end
    end
    return false
end

local Manager = {}

function Manager:addActorToCache(actor)
    if not actor.entityId then
        return
    end
    ActorCache[tostring(actor.entityId)] = actor
end

function Manager:removeActorById(entityId)
    ActorCache[tostring(entityId)] = nil
end


function Manager:getActorById(entityId)
    return ActorCache[tostring(entityId)]
end

function Manager:createSupplyActor(pos, itemId, count, meta, actorName, effectName)
    if isPosExist(pos) then
        return false
    end
    local actor = SupplyActor.new(pos, itemId, count, meta, actorName, effectName)
    if actor.entityId then
        Manager:addActorToCache(actor)
        return actor.entityId
    end
    return false
end

function Manager:createItemActor(pos, itemId, count, meta)
    meta = meta or 0
    if isPosExist(pos) then
        return false
    end
    local actor = ItemActor.new(pos, itemId, count, meta)
    if actor.entityId then
        Manager:addActorToCache(actor)
        return actor.entityId
    end
    return false
end

function Manager:onTick(ticks)
    for _, actor in pairs(ActorCache) do
        actor:onTick(ticks)
    end
end

--MerchantActor
function Manager:createMerchantActor(id)
    local actor = MerchantActor.new(id)
    actor:dayAlternatesWithNight(not EngineWorld:isNight())
    if actor.entityId then
        Manager:addActorToCache(actor)
        return actor
    end
    return nil
end

function Manager:removeActor(entityId)
    Manager:removeActorById(entityId)
end

--TravellerActor
function Manager:createTravellerActor(id)
    local actor = TravellerActor.new(id)
    actor:dayAlternatesWithNight(not EngineWorld:isNight())
    if actor.entityId then
        Manager:addActorToCache(actor)
        return actor.entityId
    end
    return false
end

function Manager:pickAttackActor(player, entityId)
    local actor = self:getActorById(entityId)
    if not actor then
        return
    end
    actor:clickedActor(player)
end

function Manager:dayAlternatesWithNight(isDay)
    local NpcActors = {}
    for oldEntityId, actor in pairs(ActorCache) do
        if actor.__cname == "TravellerActor" then
            NpcActors[oldEntityId] = actor
        end
    end
    for oldEntityId, actor in pairs(NpcActors) do
        actor:dayAlternatesWithNight(isDay)
        if oldEntityId ~= actor.entityId then
            Manager:removeActorById(oldEntityId)
            Manager:addActorToCache(actor)
        end
    end
end

return Manager