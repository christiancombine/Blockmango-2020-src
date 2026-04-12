GameTask = class()
GameTask.TaskStatus = {
    RECEIVED = 1,
    PROGRESSING = 2,
    FINISHED = 3,
    COMMIT = 4,
    LOOK_AD = 5,
}

function GameTask:ctor(userId, id, taskType, progress, status, reward)
    self.userId = userId
    self.taskId = id
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end

    self.taskType = taskType
    self.progress = progress or 0
    self.status = status or self.TaskStatus.PROGRESSING
    self.nextTaskId = 0
    self.taskTypeId = 0
    self.progressType = 0
    self.goalNum = 0
    self.remarks = 0
    --self.activeValue = 0
    self.gameTypes = ""
    
    self.rewardIds = ""
    
    if reward then
        self.rewardIds = reward
    else
        self.rewardIds = player.taskController:randomDareTaskReward(self.taskId, self.taskType)
    end

    self.remarksMeta = 0
    self.level = 0
    self.taskSort = 1
    self.quality = 0
    self:initDetailFromConfig()
end

--Get task's detail from config
function GameTask:initDetailFromConfig()
    local config = TaskConfig.TaskList[self.taskId]
    if config == nil then
        return
    end

    self.level = config.Level
    self.nextTaskId = config.NextTaskId
    self.taskTypeId = config.TaskTypeId
    self.progressType = config.ProgressType
    self.goalNum = config.GoalNum
    self.remarks = config.Remarks
    self.remarksMeta = config.RemarksMeta
    --self.activeValue = config.ActiveValue
    self.gameTypes = config.GameType
    self.taskSort = config.TaskSort
    self.quality = config.Quality
    self:CheckTaskStatus(config)
end

function GameTask:CheckTaskStatus(config)
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return false
    end

    if self.status ~= GameTask.TaskStatus.RECEIVED then
        if config.TaskTypeId == Define.TaskType.collect then
            self:CheckReplaceType(player)
        end

        if config.TaskTypeId == Define.TaskType.have_products then
            self:CheckHaveType(player)
        end
        
        if config.TaskTypeId == Define.TaskType.expand_islands then
            self:CheckIslandsType(player)
        end

        if config.TaskTypeId == Define.TaskType.products_level then
            self:CheckProductsLevelType(player)
        end
    end
end

function GameTask:CheckReplaceType(player)
    local itemNum =  0 
    if self.remarksMeta == -1 then
        for i = 0, 20  do
            itemNum = itemNum + player:getItemNumById(self.remarks, i)
        end
    else
        itemNum = player:getItemNumById(self.remarks, self.remarksMeta)
    end

    local data = {
        Type = self.taskTypeId,
        GameType = GameMatch.gameType,
        Remarks = self.remarks,
        Meta = self.remarksMeta,
        Value = itemNum
    }
    self:matching(data)
end

function GameTask:CheckHaveType(player)
    local itemNum = 0
    for i, v in pairs(player.products) do
        if tonumber(v.Id) == tonumber(self.remarks) then
            if v.Time == - 1 or v.EndTime >= os.time() then
                itemNum = 1
            end
        end
    end
    local data = {
        Type = self.taskTypeId,
        GameType = GameMatch.gameType,
        Remarks = self.remarks,
        Meta = self.remarksMeta,
        Value = itemNum
    }    
    self:matching(data)
end

function GameTask:CheckIslandsType(player)
    local itemNum = player.taskIsLandLevel
    local data = {
        Type = self.taskTypeId,
        GameType = GameMatch.gameType,
        Remarks = self.remarks,
        Meta = self.remarksMeta,
        Value = itemNum
    }
    self:matching(data)
end

function GameTask:CheckProductsLevelType(player)
    local itemNum = 0
	for i, v in pairs(player.products) do
		itemNum = itemNum + v.level
    end
    
    local data = {
        Type = self.taskTypeId,
        GameType = GameMatch.gameType,
        Remarks = self.remarks,
        Meta = self.remarksMeta,
        Value = itemNum
    }
    self:matching(data)
end

function GameTask:matching(progress)
    if self:isMatching(progress) then
        self.progress = self.progress + progress.Value
        if TaskConfig:include(Define.ReplaceType, progress.Type)then
            self.progress = progress.Value
        end

        if self:isFinishTask() then
            if self.taskTypeId == Define.TaskType.collect then
                self.status = self.TaskStatus.COMMIT
            else
                self.status = self.TaskStatus.FINISHED
            end
        else
            if self.taskTypeId == Define.TaskType.look_ad then
                self.status = self.TaskStatus.LOOK_AD
            else
                self.status = self.TaskStatus.PROGRESSING
            end
        end
        self:syncTaskToClient()
        return true
    end
    return false
end

function GameTask:isMatching(progress)
    --match type
    local isTaskTypeMatching = true
    local isGameTypeMatching = true
    local isRemarksMatching = true
    local isNotReciveMatching = true
    if progress.Type == self.taskTypeId then
        --match gameType
        if #self.gameTypes > 0 then
            isGameTypeMatching = false
            for _, gameType in pairs(self.gameTypes) do
                if tostring(gameType) == tostring(progress.GameType) then
                    isGameTypeMatching = true
                end
            end
        end
        --match remarks
        if self.remarks ~= 0 then
            isRemarksMatching = false
            if tonumber(self.remarks) == tonumber(progress.Remarks) then
                if (tonumber(self.RemarksMeta) == tonumber(progress.meta) or self.RemarksMeta == -1) then
                    isRemarksMatching = true 
                end
            end
        end

        if self.status == self.TaskStatus.RECEIVED then
            isNotReciveMatching = false
        end
    else
        isTaskTypeMatching = false
    end
    return isTaskTypeMatching and isGameTypeMatching and isRemarksMatching and isNotReciveMatching
end

--Finish task
function GameTask:isFinishTask()
    if self.goalNum > self.progress then
        return false
    end
    return true
end

function GameTask:receiveTaskReward(player, taskId, status)
    if self:isFinishTask() and (self.TaskStatus.FINISHED == self.status or self.TaskStatus.COMMIT == self.status) then
        local inv = player:getInventory()
        local reward = RewardConfig:getRewardById(tonumber(self.rewardIds))
        if reward and inv then
            if reward.Type == Define.TaskRewardType.item then
                if inv:isCanAddItem(reward.RewardId, reward.Meta, reward.Num) == false then
                    self:syncTaskToClient()
                    player.taskController:sendTaskChangeInfo()
                    HostApi.sendCommonTip(player.rakssid, "gui_str_app_shop_inventory_is_full")
                    return
                end
            end
        end

        if self.taskSort == Define.TaskMain.Dare then
            player.curDareTimes = player.curDareTimes - 1
            if player.enableIndie then
                GameAnalytics:design(player.userId, 0, { "g1048complete_dare_task_indie", self.taskId})
            else
                GameAnalytics:design(player.userId, 0, { "g1048complete_dare_task", self.taskId})
            end
        else
            if player.enableIndie then
                GameAnalytics:design(player.userId, 0, { "g1048complete_main_task_indie", self.taskId})
            else
                GameAnalytics:design(player.userId, 0, { "g1048complete_main_task", self.taskId})
            end
        end
        self.status = self.TaskStatus.RECEIVED
        player.taskController:doRewards(self.rewardIds)
        player.taskController:unLockMainlineTask()
        self:syncTaskToClient()
        player:sendDareTaskInfo()
        player.taskController:sendTaskChangeInfo()
    end
end

function GameTask:syncTaskToClient()
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return false
    end
    
    if self.taskSort == Define.TaskMain.Dare then
        player.taskController:saveHaveChangeDareTask()
        return
    end

    local tasks = {Id = self.taskId, CurNum = self.progress, Status = self.status, RewardId = self.rewardIds}
    player.taskController:saveHaveChangeMainTask(tasks)
end

return GameTask
--endregion
