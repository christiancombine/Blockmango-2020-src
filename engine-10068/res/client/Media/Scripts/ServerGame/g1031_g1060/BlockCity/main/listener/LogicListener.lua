LogicListener = {}

function LogicListener:init()
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)
    PlacingConsumeBlockEvent.registerCallBack(self.onPlacingConsumeBlock)
    EntityPositionChangeEvent.registerCallBack(self.onEntityPositionChange)
end

function LogicListener.onEntityItemSpawn(itemId, itemMeta, behavior)
    return false
end

function LogicListener.onPlacingConsumeBlock(itemId, itemMeta)
    return false
end

function LogicListener.onEntityPositionChange(entityId, position)
    local manorIndex = ManorConfig:getManorIndexByVec3(position)
    if manorIndex == 0 then
        return
    end

    local manorInfo = SceneManager:getUserManorInfoByIndex(manorIndex)
    if not manorInfo then
        return
    end

    local userId = manorInfo.userId
    local userDecoration = SceneManager:getUserDecorationInfo(userId)
    if not userDecoration then
        return
    end

    local offset = ManorConfig:getOffsetByPos(manorIndex, position)
    userDecoration:updateDecorationPos(entityId, offset)

    if not userDecoration.decoration[entityId] then
        return
    end

    local id = userDecoration.decoration[entityId].id
    local decorationInfo = DecorationConfig:getDecorationInfo(id)
    if decorationInfo and decorationInfo.isInteraction == 1 then
        GameMatch.isDecorationChange = true
    end
end

return LogicListener