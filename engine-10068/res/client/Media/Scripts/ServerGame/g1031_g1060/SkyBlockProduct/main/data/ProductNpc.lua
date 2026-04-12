---ProductNpc
ProductNpc = class()

function ProductNpc:ctor(producInfo, centralPos, curInterval, curLevel)
    self.id = producInfo.id
    self.realPos = VectorUtil.add3(producInfo.actorPos, centralPos)
    self.yaw = producInfo.yaw or 0
    self.type = producInfo.type or 24
    self.headInfo = tostring(producInfo.headInfo) or ""
    self.headInfoLock = tostring(producInfo.headInfoLock)
    self.headInfoForever = tostring(producInfo.headInfoForever)
    self.actor = producInfo.actor or ""
    self.actorBody = producInfo.actorBody or ""
    self.actorBodyId = producInfo.actorBodyId or ""
    self.realHead, self.old_Level, self.old_Interval = self:onSelectHeadInfo(curInterval, curLevel)
    self:onCreate()
    self.isUpdate = true
    self.is_laterPlayerReady = false
    self.use_time = 0
end

function ProductNpc:onCreate()
    local entityId = EngineWorld:addSessionNpc(self.realPos, self.yaw, self.type, self.actor, function(entity)
        entity:setNameLang(self.realHead or "")
        entity:setActorBody(self.actorBody or "body")
        entity:setActorBodyId(self.actorBodyId or "")
    end)
    self.entityId = entityId
    if self.old_Level >= 0 then
        EngineWorld:getWorld():updateSessionNpcHeadInfoMultiLang(self.entityId, self.realHead, self.old_Level, self.old_Interval)
    end
    HostApi.log("onCreate: self entityId and realPos is： " .. self.entityId .. "   " .. self.realPos.x .. "   " .. self.realPos.y .. "   " .. self.realPos.z)
end

function ProductNpc:onDestroy()
    EngineWorld:removeEntity(self.entityId)
end

function ProductNpc:onUpdate(curInterval, curLevel)
    self.use_time = self.use_time + 1
    local head_Info, cur_Level, cur_Interval = self:onSelectHeadInfo(curInterval, curLevel)
    local isChange_head = self.realHead ~= head_Info
    local isChange_level = self.old_Level ~= cur_Level
    local isChange_interval = self.old_Interval ~= cur_Interval
    local isChange = isChange_interval or isChange_level or isChange_head
    if isChange or self.is_laterPlayerReady then
        self.realHead = head_Info
        self.old_Level = cur_Level
        if isChange == false and self.old_Interval > 0 and self.old_Interval - self.use_time > 0 then
            self.old_Interval = self.old_Interval - self.use_time
        else
            self.old_Interval = cur_Interval
        end
        self.isUpdate = true
    end
    if self.isUpdate then
        EngineWorld:getWorld():updateSessionNpcHeadInfoMultiLang(self.entityId, self.realHead, self.old_Level, self.old_Interval)
        print("NPC isUpdate    id : " .. self.id .. "   cur_Level : " .. cur_Level .. "   cur_Interval : " .. cur_Interval)
        self.isUpdate = false
        self.is_laterPlayerReady = false
        self.use_time = 0
    end
    -- EngineWorld:getWorld():updateSessionNpc(self.entityId, rakssid, head_Info, self.actor, self.actorBody, self.actorBodyId, "", "run", 0, true)
end

function ProductNpc:onSelectHeadInfo(curInterval, curLevel)
    if curInterval ~= nil and type(curInterval) == "number" then
        if curInterval > 0 then
            return self.headInfo, curLevel or -2, curInterval
        elseif curInterval == -1 then
            return self.headInfoForever, curLevel or -3, -1
        end
    end
    return self.headInfoLock, 0, 0
end