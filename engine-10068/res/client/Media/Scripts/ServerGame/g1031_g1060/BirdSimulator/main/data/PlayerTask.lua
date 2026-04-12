require "data.TaskData"
require "data.MulticastEvent"
require "manager.TaskHelper"

local function getRakssid(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return 0
    end
    return player.rakssid
end

PlayerTask = class("PlayerTask")

function PlayerTask:ctor(player)
    self.canAcceptMainTasks = {} --可以接受的主线任务
    self.canAcceptDailyTasks = {} --可以接受的每日任务

    --正在做的任务
    self.taskContents = {}
    -- 已经完成的 主线任务
    self.completeMainTasks = {}
    -- 已经完成的每日任务
    self.completeDailyTasks = {}
    -- 可以领取奖励的物品
    self.canReceiveRewards = {}
    self.nextGetRewarsTime = os.time()
    self.interval = 0
    self.readDataResult = false
    self.canReceiveMainTask = {}
    self.userId = player.userId
    self.isRefresh = false
    self.refreshTaskIds = {}
    self.timerKey = "UpdateTask:" .. tostring(self.userId)
    TimerManager.AddIntervalListener(self.timerKey, nil, self.Update, self)
end

function PlayerTask:cacheRefreshTask(taskId, npcId)
    if type(taskId) == "number" then
        self.refreshTaskIds[taskId] = npcId
    end
end

function PlayerTask:onReadData(userId, data)
    if self.userId ~= userId then
        return
    end
    self.completeDailyTasks = {}
    self.completeMainTasks = {}
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    if data.__first then
        TaskManager:InitPlayerTaskInfo(player)
        return
    end
    self:OnReadMainTaskDataFromDb(player, data.mainTasks or {})
    self:OnReadDailyTaskDataFromDb(player, data.dailyTasks or {})
    for taskId, content in pairs(self.taskContents) do
        self:OnCreateTaskSuccess(taskId)
    end
    
    self:AddDailyTimer()
    TaskManager:InitPlayerTaskInfo(player)
end

function PlayerTask:OnCreateTaskSuccess(taskId)
    if type(taskId) == "number" then
        local contents = self.taskContents[taskId]
        if type(contents) == "table" then
            for _, content in pairs(contents) do
                content:OnCreateSuccess()
                content:CheckTask()
            end
        end
        if taskId == GameConfig.drawLuckyTaskId then
            HostApi.changeGuideArrowStatus(getRakssid(self.userId), GameConfig.drawLuckyPoint, true)
        end

        if taskId == GameConfig.shopTaskId then
            HostApi.changeGuideArrowStatus(getRakssid(self.userId), GameConfig.arrowPointToShopPos, true)
        end
        if taskId == GameConfig.addCarryTo3TaskId and self:CheckTaskComplete(taskId) == false then
            HostApi.showAddCarryGuide(getRakssid(self.userId))
        end
        if taskId == GameConfig.addCarryTo4TaskId and self:CheckTaskComplete(taskId) == false then
            HostApi.showAddCarryGuide(getRakssid(self.userId))
        end
        if taskId == GameConfig.feedTaskId and self:CheckTaskComplete(taskId) == false then
            HostApi.showBirdFeedGuide(getRakssid(self.userId))
        end
    end
end

function PlayerTask:getInterval()
    if self.nextGetRewarsTime == 0 then
        return 0
    end
    return self.nextGetRewarsTime - os.time()
end

function PlayerTask:OnReadMainTaskDataFromDb(player, mainTasks)
    for _, contentInfo in pairs(mainTasks.taskContents) do
        local _, task = TaskManager:getTaskByTaskId(contentInfo.taskId)
        if task ~= nil then
            task:CreateOneContent(
                self.userId,
                contentInfo.contentId,
                function(content)
                    content.curNumber = contentInfo.curNumber
                end
            )
        else
            print("[config error]  not find task  task id  = " .. contentInfo.taskId)
        end
    end
    for _, taskId in pairs(mainTasks.completeTasks) do
        self.completeMainTasks[taskId] = true
        self.canAcceptMainTasks[taskId] = nil
    end
    self:RefreshTaskInfo(nil, false)
end

function PlayerTask:OnReadDailyTaskDataFromDb(player, dailyTasks)
    if dailyTasks.time ~= DateUtil.getDate() then
        return
    end
    if type(dailyTasks.completeTasks) == "table" then
        for _, taskId in pairs(dailyTasks.completeTasks) do
            self.completeDailyTasks[taskId] = true
            self.canAcceptDailyTasks[taskId] = nil
        end
    end
    self.nextGetRewarsTime = dailyTasks.nextGetRewarsTime
    for _, contentInfo in pairs(dailyTasks.taskContents) do
        local _, task = TaskManager:getTaskByTaskId(contentInfo.taskId)
        task:CreateOneContent(
            self.userId,
            contentInfo.contentId,
            function(content)
                content.curNumber = contentInfo.curNumber
            end
        )
    end
    self:RefreshTaskInfo(nil, false)
end

function PlayerTask:PushTaskContent(key, value, taskId)
    if self.taskContents[taskId] == nil then
        self.taskContents[taskId] = {}
    end
    self.taskContents[taskId][key] = value
end

-- 接受主线任务
function PlayerTask:acceptMainTask(player, npc, taskId)
    if self.canAcceptMainTasks[taskId] == nil then
        return false
    end
    local taskData = npc:getTaskData(taskId)
    npc:OnAcceptTask(player, taskId)
    taskData:CreateTaskContent(self.userId)
    self:RefreshTaskInfo(taskId, false)
    self.canAcceptMainTasks[taskId] = nil
    HostApi.sendBirdReceiveTaskResult(player.rakssid)

    npc:OnAcceptTaskSuccess(player)
    return true
end

-- 接受每日任务
function PlayerTask:acceptDailyTask(player, npc, taskId)
    if os.time() - self.nextGetRewarsTime < 0 then
        return false
    end
    if self.canAcceptDailyTasks[taskId] == nil then
        return false
    end
    local taskData = npc:getTaskData(taskId)
    npc:OnAcceptTask(player, taskId)
    taskData:CreateTaskContent(self.userId)
    self:RefreshTaskInfo(taskId, false)
    self.canAcceptDailyTasks[taskId] = nil
    HostApi.sendBirdReceiveTaskResult(player.rakssid)
    npc:OnAcceptTaskSuccess(player)
    return true
end

TaskStatus = {
    complete = 1,
    doTask = 2,
    canReceive = 3,
    newTask = 4
}

function PlayerTask:addCanAcceptMainTask(taskId)
    local result = self.completeMainTasks[taskId]
    if result == true then
        return TaskStatus.complete
    end

    local contents = self.taskContents[taskId]
    if type(contents) == "table" then
        for _, content in pairs(contents) do
            if content:IsComplete() == false then
                return TaskStatus.doTask
            end
        end
        return TaskStatus.canReceive
    end

    self.canAcceptMainTasks[taskId] = true
    return TaskStatus.newTask
end

function PlayerTask:addCanAcceptDailyTask(taskId)
    if self.nextGetRewarsTime - os.time() > 0 then
        return nil
    end
    local result = self.completeDailyTasks[taskId]
    if result == true then
        return TaskStatus.complete
    end

    local contents = self.taskContents[taskId]
    if type(contents) == "table" then
        for _, content in pairs(contents) do
            if content:IsComplete() == false then
                return TaskStatus.doTask
            end
        end
        return TaskStatus.canReceive
    end
    self.canAcceptDailyTasks[taskId] = true
    return TaskStatus.newTask
end

function PlayerTask:CheckMainTaskIsComplete(taskId)
    return self.completeMainTasks[taskId] and true or false
end

function PlayerTask:AddDailyTimer()
    local intervalTimr = self.nextGetRewarsTime - os.time()
    if intervalTimr > 0 then
        local key = "taskRefresh:" .. tostring(self.userId)
        local npc = TaskManager:getDailyTaskNpc()
        local player = PlayerManager:getPlayerByUserId(self.userId)
        npc:OnNotTask(player)
        TimerManager.AddDelayListener(
            key,
            intervalTimr,
            function()      
                if player and npc then
                    npc:InitPlayerTaskInfo(player)
                end
            end
        )
    end
end

function PlayerTask:CheckTaskIsReceive(taskId, npc)
    local result = self.canReceiveRewards[taskId]
    if result ~= true then
        return false
    end

    if npc.npcType == 1 then --日常
        local player = PlayerManager:getPlayerByUserId(self.userId)
        local rewards = TaskConfig:getDailyTaskReward(taskId)
        if rewards == nil then
            return nil
        end
        self.interval = TaskConfig:getIntervalByTaskId(taskId)
        self.nextGetRewarsTime = os.time() + self.interval
        self:AddDailyTimer()
        self.canReceiveRewards[taskId] = nil
        local birdGains = {}
        for i, info in pairs(rewards) do
            player:addProp(info.id, info.num)
            if info.id ~= 9000001 then
                local birdGain = BirdGain.new()
                birdGain.itemId = info.id
                birdGain.num = info.num
                birdGain.icon = info.icon or ""
                birdGain.name = info.name or ""
                table.insert(birdGains, birdGain)
            end
        end
        if next(birdGains) then
            HostApi.sendBirdGain(player.rakssid, birdGains)
        end
        npc:OnNotTask(player, self.interval * 1000)
        self.completeDailyTasks[taskId] = true
        self:RefreshTaskInfo(taskId, true)
        return true
    elseif npc.npcType == 2 then
        local player = PlayerManager:getPlayerByUserId(self.userId)
        local rewards = TaskConfig:getMainTaskReward(taskId)
        if rewards == nil then
            return false
        end
        self.canReceiveRewards[taskId] = nil
        local birdGains = {}
        for i, info in pairs(rewards) do
            if info.id ~= 9000001 then
                local birdGain = BirdGain.new()
                birdGain.itemId = info.id
                birdGain.num = info.num
                birdGain.icon = info.icon or ""
                birdGain.name = info.name or ""
                table.insert(birdGains, birdGain)
            end
            player:addProp(info.id, info.num)
        end
        if next(birdGains) then
            HostApi.sendBirdGain(player.rakssid, birdGains)
        end
        self.completeMainTasks[taskId] = true
        npc:InitPlayerTaskInfo(player)
        self:RefreshTaskInfo(taskId, true)
        return true
    end
    return false
end

function PlayerTask:Update()
    for taskId, npcId in pairs(self.refreshTaskIds) do
        self:RefreshTaskInfo(taskId, false)
    end
    for taskId, npcId in pairs(self.refreshTaskIds) do
        if self:CheckTaskComplete(taskId) == true then
            self:OnCompleteTask(taskId, npcId)
        end
    end
    self.refreshTaskIds = {}
end

-- 当完成任务
function PlayerTask:OnCompleteTask(taskId, npcId)
    local contents = self.taskContents[taskId]

    if type(contents) ~= "table" then
        return
    end
    for key, value in pairs(contents) do
        TaskHelper.remove(key)
    end
    if taskId == GameConfig.drawLuckyTaskId then
        HostApi.changeGuideArrowStatus(getRakssid(self.userId), GameConfig.drawLuckyPoint, false)
    end

    local player = PlayerManager:getPlayerByUserId(self.userId)

    if taskId == GameConfig.shopTaskId then
        HostApi.changeGuideArrowStatus(getRakssid(self.userId), GameConfig.arrowPointToShopPos, false)
        if player ~= nil then
            player.isShowShopArrow = false
        end
    end

    self.canReceiveRewards[taskId] = true
    local npc = TaskManager:GetNpcById(npcId)
    if npc == nil then
        print("not find task  task id  =  " .. npcId)
        return
    end
    npc:OnCompleteTask(player, taskId)
    TaskHelper.dispatch(TaskType.CompleteNpcTaskNumEvent, self.userId, npc.id, 1)
end

function PlayerTask:RefreshTaskInfo(taskId, isRemove)
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end

    local callback = function(taskId, contentlist)
        if type(taskId) ~= "number" or type(contentlist) ~= "table" then
            return nil
        end
        local _, task = TaskManager:getTaskByTaskId(taskId)
        local birdtask = BirdTask.new()
        birdtask.taskId = task.taskId
        birdtask.entityId = task.bindNpcId
        birdtask.taskName = task.bindNpcName
        local contents = {}
        for k, info in pairs(contentlist) do
            local birdTaskItem = BirdTaskItem.new()
            birdTaskItem.id = info.contentId
            birdTaskItem.curValue = math.min(info.curNumber, info.number)
            birdTaskItem.needValue = info.number
            birdTaskItem.desc = info.desc
            birdTaskItem.name = info.name
            table.insert(contents, birdTaskItem)
        end
        birdtask.contents = contents
        return birdtask
    end
    if type(taskId) == "number" then
        local contentlist = self.taskContents[taskId]
        local birdtask = callback(taskId, contentlist)
        if birdtask ~= nil then
            HostApi.sendBirdSimulatorTaskItem(getRakssid(self.userId), isRemove, birdtask)
        end
        if isRemove == true then
            self.taskContents[taskId] = nil
        end
        return
    end
    local birdSimulator = player:getBirdSimulator()
    if birdSimulator == nil then
        return
    end
    local tasks = {}

    for taskId, contentlist in pairs(self.taskContents) do
        local birdtask = callback(taskId, contentlist)
        if birdtask ~= nil then
            table.insert(tasks, birdtask)
        end
    end
    birdSimulator:setTasks(tasks)
end

function PlayerTask:OnPlayerLogout()
    TimerManager.RemoveListenerById(self.timerKey)
    local key = "taskRefresh:" .. tostring(self.userId)
    TimerManager.RemoveListenerById(key)
    for key, contents in pairs(self.taskContents) do
        for k, v in pairs(contents) do
            TaskHelper.remove(k)
        end
    end
end

function PlayerTask:CheckTaskComplete(taskId)
    local contents = self.taskContents[taskId]
    if type(contents) == "table" then
        for _, content in pairs(contents) do
            if content:IsComplete() == false then
                return false
            end
        end
    end
    return true
end

function PlayerTask:SaveData()
    local data = {}
    data.mainTasks = {}
    data.mainTasks.taskContents = {}
    data.mainTasks.completeTasks = {}

    for k, v in pairs(self.completeMainTasks) do
        table.insert(data.mainTasks.completeTasks, k)
    end
    data.dailyTasks = {}
    data.dailyTasks.taskContents = {}
    data.dailyTasks.time = DateUtil.getDate()
    data.dailyTasks.completeTasks = {}
    data.dailyTasks.nextGetRewarsTime = self.nextGetRewarsTime
    for taskId, contents in pairs(self.taskContents) do
        for k, cintent in pairs(contents) do
            local tab = {
                taskId = cintent.bindTaskId,
                contentId = cintent.contentId,
                curNumber = cintent.curNumber
            }
            if cintent.taskType == 1 then
                table.insert(data.dailyTasks.taskContents, tab)
            elseif cintent.taskType == 2 then
                table.insert(data.mainTasks.taskContents, tab)
            end
        end
    end

    for key, val in pairs(self.completeDailyTasks) do
        table.insert(data.dailyTasks.completeTasks, key)
    end

    DBManager:savePlayerData(self.userId, DbUtil.TAG_TASK, data, true)
end
