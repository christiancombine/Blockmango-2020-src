require "data.GameMainTask"
require "data.DailyTask"
GameTaskNpc = class("GameTaskNpc")

function GameTaskNpc:CreateSessionNpc()
    local entityId = EngineWorld:addSessionNpc(self.pos, self.yaw, 104104, self.actorName, function(entity)
        entity:setNameLang(self.name or "")
        entity:setActorBody(self.actorBody or "body")
        entity:setActorBodyId(self.actorBodyId or "")
        entity:setPerson(true)
    end)
    self.npcId = entityId

    entityId = EngineWorld:addSessionNpc(self.tipPos, self.yaw, -1, self.tipActorName, function(entity)
        entity:setActorBody(self.tipActorBody[1] or "body")
        entity:setActorBodyId(self.tipActorBodyId[1] or "")
        entity:setPerson(true)
    end)
    self.tipNpcId = entityId

    AreaInfoManager:push(self.npcId, AreaType.Task, self.areaMinPos, self.areaMaxPos)
    return self.npcId
end

function GameTaskNpc:OnCompleteTask(player, taskId, time)
    local task = self.taskList[taskId]
    if type(task) == "table" then
        local contentData = task:OnCompleteTask()
        contentData.taskId = taskId
        contentData.entityId = self.npcId
        local json_data = json.encode(contentData)
        self:RefreshNpc(player, json_data, true, time)
        HostApi.sendPlaySound(player.rakssid, 318)
    end
end

function GameTaskNpc:CreateDailyTask()
    if self.npcType == 1 then
        local taskConfigs = TaskConfig:getDailyTaskByNpcindex(self.id)
        for _, info in pairs(taskConfigs) do
            local task = DailyTask.new(info, self)
            self.taskList[info.taskId] = task
            table.insert(self.taskIdList, info.taskId)
        end
    end
    self:upsetMap()
end

function GameTaskNpc:upsetMap()
    if next(self.taskIdList) then
        local count = #self.taskIdList
        for k, v in pairs(self.taskIdList) do
            local randNum = HostApi.random("task", 1, count)
            self.taskIdList[k], self.taskIdList[randNum] = self.taskIdList[randNum], self.taskIdList[k]
        end
    end
end

function GameTaskNpc:getTaskData(taskId)
    return self.taskList[taskId]
end

function GameTaskNpc:OnNotTask(player, time)
    local contentData = {}
    contentData.title = self.normalTitle or ""
    contentData.tips = {}
    contentData.interactionType = 104101
    contentData.taskId = -1
    contentData.entityId = self.npcId
    for i, v in ipairs(self.normalDialog) do
        contentData.tips[i] = v
    end
    local json_data = json.encode(contentData)
    self:RefreshNpc(player, json_data, false, time)
end

function GameTaskNpc:InitPlayerTaskInfo(player)

end

function GameTaskNpc:OnNewTask(player, taskId, time)
    local task = self.taskList[taskId]
    if type(task) == "table" then
        local contentData = task:OnNewTask()
        contentData.taskId = taskId
        contentData.entityId = self.npcId
        local json_data = json.encode(contentData)
        self:RefreshNpc(player, json_data, true, time)
    end
end

function GameTaskNpc:RefreshNpc(player, data, isShowTips, time)
    local action = isShowTips and self.canReceiveAction or self.defaultAction
    local tipbody = isShowTips and self.tipActorBody[2] or self.tipActorBody[1]
    local tipbodyId = isShowTips and self.tipActorBodyId[2] or self.tipActorBodyId[1]
    EngineWorld:getWorld():updateSessionNpc(
            self.npcId,
            player.rakssid,
            self.name,
            self.actorName,
            self.actorBody,
            self.actorBodyId,
            data,
            action,
            time or 0,
            true
    )

    EngineWorld:getWorld():updateSessionNpc(
            self.tipNpcId,
            player.rakssid,
            "",
            self.tipActorName,
            tipbody,
            tipbodyId,
            "",
            "",
            0,
            false
    )
end

function GameTaskNpc:OnAcceptTask(player, taskId, time)
    local task = self.taskList[taskId]
    if type(task) == "table" then
        local contentData = task:OnAcceptTask()
        contentData.taskId = taskId
        contentData.entityId = self.npcId
        local json_data = json.encode(contentData)
        self:RefreshNpc(player, json_data, false, time)
    end
end

function GameTaskNpc:OnAcceptTaskSuccess(player)
    if self.id == GameConfig.arrowPointToNpc then
        player.isShowTaskArrow = false
        HostApi.changeGuideArrowStatus(player.rakssid, GameConfig.arrowPointToNpcPos, player.isShowTaskArrow)
    end
end
