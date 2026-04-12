PublicFacilitiesConfig = {}

local pF = PublicFacilitiesConfig
pF.publicChairs = {}
pF.elevatorActor = {}
pF.portalActor = {}
pF.portal = {}

ActorType = {
    Elevator = pF.elevatorActor,
    Portal = pF.portalActor,
}

function PublicFacilitiesConfig:initPublicChairs(config)
    for _, v in pairs(config) do
        local data = {}
        data.index = tonumber(v.index)
        data.id = tonumber(v.id)
        data.pos = VectorUtil.newVector3(v.x, v.y, v.z)
        data.yaw = tonumber(v.yaw)
        self.publicChairs[data.index] = data
    end
end

function PublicFacilitiesConfig:preparePublicChairs()
    for _, chair in pairs(self.publicChairs) do
        chair.entityId = EngineWorld:getWorld():addDecoration(chair.pos, chair.yaw, 0, chair.id)
        local decorationInfo = DecorationConfig:getDecorationInfo(chair.id)
        if decorationInfo and decorationInfo.isInteraction == 1 then
            local x = 0
            local z = 0

            if chair.yaw % 180 == 0 then
                x = decorationInfo.length / 2
                z = decorationInfo.width / 2
            else
                x = decorationInfo.width / 2
                z = decorationInfo.length / 2
            end

            local startPos = VectorUtil.newVector3(chair.pos.x - x, chair.pos.y, chair.pos.z - z)
            local endPos = VectorUtil.newVector3(chair.pos.x + x, chair.pos.y + decorationInfo.height + 1, chair.pos.z + z)
            AreaInfoManager:push(chair.entityId, AreaType.PublicChairs, decorationInfo.tipNor, decorationInfo.tipPre, decorationInfo.imageNor, decorationInfo.imagePre, false, startPos, endPos)
        end
    end
end

function PublicFacilitiesConfig:getPublicChairByEntityId(entityId)
    entityId = tonumber(entityId)
    for i, v in pairs(self.publicChairs) do
        if v.entityId == entityId then
            return v.id
        end
    end

    return 0
end

function PublicFacilitiesConfig:initElevatorActor(config)
    for _, v in pairs(config) do
        local actor = {}
        actor.pos = VectorUtil.newVector3(v.x, v.y, v.z)
        actor.yaw = tonumber(v.yaw)
        actor.name = (v.name ~= "@@@" and { v.name } or { "" })[1]
        actor.id = tonumber(v.id)
        actor.body = v.body
        actor.bodyId = v.bodyId
        actor.actor = v.actor
        self.elevatorActor[actor.id] = actor
    end
end

function PublicFacilitiesConfig:createActor(tab, isPerson)
    isPerson = isPerson or false
    for _, v in pairs(tab) do
        EngineWorld:addSessionNpc(v.pos, v.yaw, tonumber(v.type), v.actor, function(entity)
            entity:setNameLang(v.name or "")
            entity:setActorBody(v.body or "body")
            entity:setActorBodyId(v.bodyId or "")
            entity:setPerson(isPerson)
        end)
    end
end

function PublicFacilitiesConfig:initPortalActor(config)
    for _, v in pairs(config) do
        local actor = {}
        actor.id = tonumber(v.id)
        actor.uId = tonumber(v.uId)
        actor.yaw = tonumber(v.yaw)
        actor.pos = VectorUtil.newVector3(v.x, v.y, v.z)
        actor.actor = v.actor
        actor.x1, actor.x2 = ShopHouseConfig:sortDataASC(v.x1, v.x2)
        actor.y1, actor.y2 = ShopHouseConfig:sortDataASC(v.y1, v.y2)
        actor.z1, actor.z2 = ShopHouseConfig:sortDataASC(v.z1, v.z2)
        self.portalActor[actor.uId] = actor
    end
end

function PublicFacilitiesConfig:initPortal(config)
    for _, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.uId = tonumber(v.uId)
        data.isOpen = tonumber(v.isOpen)
        data.tpPos = VectorUtil.newVector3(v.x, v.y, v.z)
        data.tpYaw = tonumber(v.yaw)
        self.portal[data.uId] = self.portal[data.uId] or {}
        self.portal[data.uId][data.id] = data
    end
end

function PublicFacilitiesConfig:isInPortalActorRegion(ActorUid, pos)
    local actor = self.portalActor[tonumber(ActorUid)]
    if not actor then
        return false
    end

    return pos.x >= actor.x1 and pos.x <= actor.x2
            and pos.y >= actor.y1 and pos.y <= actor.y2
            and pos.z >= actor.z1 and pos.z <= actor.z2
end

function PublicFacilitiesConfig:getPortalInfoByUidAndId(ActorUid, id)
    return self.portal[tonumber(ActorUid)][tonumber(id)]
end

return PublicFacilitiesConfig