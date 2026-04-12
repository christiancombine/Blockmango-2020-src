GuideManager = {}

GuideManager.Status = {
    UnLock = 0,
    UnTrigger = 1,
    Showed = 2,
    Done = 3
}
GuideManager.needReleaseAirWallInGuide = false

function GuideManager:recoverGuide(player, guideData)
    local running_guides = guideData.running_guides or {}
    local finish_ids = guideData.finish_ids or {}
    for _, guide in pairs(running_guides) do
        local config = GuideConfig:getGuideConfigById(guide.Id)
        if config then
            if config.ResetId > 0 and config.ResetId ~= guide.Id then
                if GuideConfig:isGuideFirstOfChain(config.ResetId) then
                    table.insert(player.running_guides, { Id = config.ResetId, Status = self.Status.UnTrigger })
                else
                    table.insert(player.running_guides, { Id = config.ResetId, Status = self.Status.Showed })
                end
            else
                table.insert(player.running_guides, { Id = guide.Id, Status = guide.Status })
            end
        end
    end
    player.finish_ids = finish_ids
end

function GuideManager:getOffset(manorIndex)
    local orientation = ManorConfig:getOrientationByIndex(manorIndex)
    local demoDrawOffset = VectorUtil.newVector3(0, 0, 0)
    if orientation == 90 then
        demoDrawOffset = GameConfig.GuideDemoDrawRPos90
    elseif orientation == 180 then
        demoDrawOffset = GameConfig.GuideDemoDrawRPos180
    elseif orientation == 270 then
        demoDrawOffset = GameConfig.GuideDemoDrawRPos270
    end
    return demoDrawOffset
end

function GuideManager:updateStatus(player, guide)
    self:removeGuide(player, guide.Id)
    if guide.Status == self.Status.Done then
        self:doEvent(player, guide.Id)
        player.finish_guideId = guide.Id
        player:guideAnalytics("g1047guide" .. guide.Id)
        if player.finish_guideId == GameConfig.inGuideLastId and player:getIsInGuide() then
            player:setIsInGuide(false)
        end
---------------------加入杂乱方块用于清除领地-------------------------
        if player.finish_guideId == GameConfig.GuidePlaceDemoDrawEventId then
            local demoDraw = DrawingConfig:getDrawingById(GameConfig.GuideDemoDrawId)
            local demoDrawOffset = GuideManager:getOffset(player.manorIndex)
            local pos = ManorConfig:getPosByOffset(player.manorIndex, demoDrawOffset, true)
            SceneManager:createOrDestroyHouse(demoDraw.fileName, true, pos, player:getYaw(), 0)
        end
-----------------------------------------------------------------
        if GuideConfig:isGuideFirstOfChain(guide.Id) then
            for _, id in pairs(player.finish_ids) do
                if id == guide.Id then
                    return
                end
            end
            table.insert(player.finish_ids, guide.Id)
        end
        return
    end
    table.insert(player.running_guides, { Id = guide.Id, Status = guide.Status })
end

function GuideManager:removeGuide(player, guideId)
    for key, guide in pairs(player.running_guides) do
        if guide.Id == guideId then
            table.remove(player.running_guides, key)
            return
        end
    end
end

function GuideManager:doEvent(player, guideId)
    local config = GuideConfig:getEventByGuideId(guideId)
    if config and config.Type == GuideConfig.EventType.FlashTo then
        local offset = VectorUtil.newVector3(config.Pos[1], config.Pos[2], config.Pos[3])
        local pos = ManorConfig:getPosByOffset(player.manorIndex, offset, true)
        player:teleportPos(pos)
    end
end

function GuideManager:sendDataToClient(player, isNeedTeleport, limit)
    local targetUserId = SceneManager:getTargetByUid(player.userId)
    if player.manor and tostring(player.userId) == tostring(targetUserId) then
        if player:getIsInGuide() then
            if isNeedTeleport then
                local pos = ManorConfig:getPosByOffset(player.manorIndex, GameConfig.GuideSuspendTeleportRPos, true)
                player:teleportPos(pos)
            end
            if player.chooseTemplateId ~= 0 then
                player:sendStoreInfo(true)
            end
        end

        local data = {}
        data.lv = limit or 1
        data.under_way = player.running_guides
        data.done = {}
        for _, id in pairs(player.finish_ids) do
            table.insert(data.done,{ Id = id })
        end
        HostApi.sendUpdateGuideData(player.rakssid, json.encode(data))
    end
end

return GuideManager