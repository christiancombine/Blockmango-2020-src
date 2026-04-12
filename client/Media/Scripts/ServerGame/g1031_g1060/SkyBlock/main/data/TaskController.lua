require "data.GameTask"

TaskController = class()

function TaskController:ctor(player)
    self.userId = player.userId
    self.dareTasks = {}
    self.mainLineTasks = {}
    self.changeMaintask = {}
    self.sendDareInfo = false
end

function TaskController:init()

    local player = PlayerManager:getPlayerByUserId(self.userId)
    if not player then
        return
    end
    if DbUtil.CanSavePlayerData(player, DbUtil.TAG_MIAN_LINE) and DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
        self.Level = player.taskIsLandLevel
        self:initDareTasks(player)
        self:initMainLineTasks(player)
        self:unLockMainlineTask()
        self:sendMainAndDareTaskToClient()
    end
end

function TaskController:initDareTasks(player)
    local dare_tasks = player.dare_tasks
    for _, task in pairs(dare_tasks) do
        self:addDareTask(GameTask.new(player.userId, task.id, Define.TaskMain.Dare, task.progress, task.status, task.rewardIds))
    end

	if #dare_tasks == 0 and player.dareIsUnLock then
		player.taskController:randomDareTask()
	end
end

function TaskController:initMainLineTasks(player)
    local mainline_tasks = player.mainline_tasks or {}
    for _, task in pairs(mainline_tasks) do
        self:addMainLineTask(GameTask.new(player.userId, task.id, Define.TaskMain.Mainline, task.progress, task.status, task.rewardIds))
    end
	if #mainline_tasks == 0 then
		player.taskController:unlockMainLineTaskByLevel(player, player.progressesLevel)
	end
end

function TaskController:unlockMainLineTaskByLevel(player, level)

    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_MIAN_LINE) and not DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
        return
    end

    local taskIds = TaskConfig:getTaskIds(level)
    if #taskIds == 0 then
        if player.progressesLevel >= 0 and player.progressesLevel < TaskConfig.TASK_LEVEL_COUNT then

            player.progressesLevel = player.progressesLevel + 1
            self:unlockMainLineTaskByLevel(player, player.progressesLevel)
            return
        end
    end
    for _, taskId in pairs(taskIds) do
        local isNothave = true
        for k, v in pairs(self.mainLineTasks) do
            if v.taskId == taskId then
                isNothave = false
                break
            end
        end

        if isNothave then
            if TaskConfig.TaskList[taskId].TaskTypeId == Define.TaskType.look_ad then
                self:addMainLineTask(GameTask.new(player.userId, taskId, Define.TaskMain.Mainline, nil ,GameTask.TaskStatus.LOOK_AD))
            else
                self:addMainLineTask(GameTask.new(player.userId, taskId, Define.TaskMain.Mainline))
            end
        end
    end
    self:sendMainTaskToClient()
end

function TaskController:packingProgressData(player, taskTypeId, gameType, remarks, value, meta, id)
    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_MIAN_LINE) or not DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA)  then
        return
    end

    local data = {
        Type = taskTypeId,
        GameType = gameType,
        Remarks = remarks or 0,
        Meta = meta or -1,
        Value = value or 1
    }
    self:consumeTaskProcess(data, id)
    self:sendTaskChangeInfo()
end

function TaskController:sendTaskChangeInfo()
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if not player then
        return
    end

    if #self.changeMaintask > 0 then
        local cur_level = player.taskIsLandLevel
        HostApi.sendUpdateSkyBlockMainLineTaskData(player.rakssid, cur_level, player.progressesLevel, player.dareIsUnLock, json.encode(self.changeMaintask), Define.TaskMain.Mainline)
        self.changeMaintask = {}
    end


    if self.sendDareInfo then
        self:sendDareTaskToClient()
        self.sendDareInfo = false
    end
end

function TaskController:saveHaveChangeMainTask(task)
    table.insert(self.changeMaintask, task)

end

function TaskController:saveHaveChangeDareTask()
    self.sendDareInfo = true
end

function TaskController:consumeTaskProcess(progress, id)
    for _, task in pairs(self.dareTasks) do
        if id then
            if task.taskId == id then
                task:matching(progress)
            end
        else
            task:matching(progress)
        end
    end

    --HostApi.log("TaskController:consumeTaskProcess")
    for _, task in pairs(self.mainLineTasks) do
        if id then
            if task.taskId == id then
                task:matching(progress)
            end
        else
            task:matching(progress)
        end
    end
end

function TaskController:sendDareTaskToClient()
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if not player then
        return
    end

    local cur_level = player.taskIsLandLevel
    local dare_tasks = {}
    for _, task in ipairs(self.dareTasks) do
        table.insert(dare_tasks, { Id = task.taskId, CurNum = task.progress, Status = task.status, RewardId = task.rewardIds})
    end

    HostApi.sendUpdateSkyBlockMainLineTaskData(player.rakssid, cur_level, player.progressesLevel, player.dareIsUnLock, json.encode(dare_tasks), Define.TaskMain.Dare)
    player:sendDareTaskInfo()
end

function TaskController:sendMainTaskToClient()
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if not player then
        return
    end

    local cur_level = player.taskIsLandLevel
    local main_tasks = {}
    for _, task in pairs(self.mainLineTasks) do
        table.insert(main_tasks, { Id = task.taskId, CurNum = task.progress, Status = task.status, RewardId = task.rewardIds})
    end
    HostApi.sendUpdateSkyBlockMainLineTaskData(player.rakssid, cur_level, player.progressesLevel, player.dareIsUnLock, json.encode(main_tasks), Define.TaskMain.Mainline)
end

function TaskController:sendMainAndDareTaskToClient()
    self:sendDareTaskToClient()
    self:sendMainTaskToClient()
end

function TaskController:addMainLineTask(task)
    for k, v in pairs(self.mainLineTasks) do
        if (v.taskId == task.taskId) then
            return
        end
    end
    table.insert(self.mainLineTasks, task)
end

function TaskController:addDareTask(task)
    for k, v in pairs(self.dareTasks) do
        if (v.taskId == task.taskId) then
            return
        end
    end
    table.insert(self.dareTasks, task)
end

function TaskController:transformToPlayerTaskData(player)
    local dareTasksData = {}
    for _, task in pairs(self.dareTasks) do
        table.insert(dareTasksData, { id = task.taskId, progress = task.progress, status = task.status, rewardIds = task.rewardIds})
    end
    player.dare_tasks = dareTasksData

    local mainlineTasksData = {}
    for _, task in pairs(self.mainLineTasks) do
        table.insert(mainlineTasksData, { id = task.taskId, progress = task.progress, status = task.status, rewardIds = task.rewardIds})
    end

    player.mainline_tasks = mainlineTasksData
end

function TaskController:receiveTaskReward(player, taskId, status)
    local task = self:getTaskByTaskId(taskId)
    if task then
        if player:checkTaskTimes() == false and task.taskSort == Define.TaskMain.Dare then
            HostApi.sendCommonTip(player.rakssid, "gui_today_reach_max_task_times")
        else
            task:receiveTaskReward(player, taskId, status)
        end
    end
end

function TaskController:doRewards(rewardData)
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if not player then
        return
    end
    local rewardIds = StringUtil.split(rewardData, "#")
    for _, Id in pairs(rewardIds) do
        local reward = RewardConfig:getRewardById(tonumber(Id))
        if not reward then
            return
        end

        RewardConfig:onPlayerGetReward(player, reward)
    end
end

function TaskController:getTaskByTaskId(taskId)
    for _, task in pairs(self.dareTasks) do
        if task.taskId == taskId then
            return task
        end
    end

    for _, task in pairs(self.mainLineTasks) do
        if task.taskId == taskId then
            return task
        end
    end
    return nil
end

function TaskController:checkUnlockNextLevelTask(level)

    for _, task in pairs(self.mainLineTasks) do
        if task.taskId == taskId then
            return task
        end
    end
    return nil
end

function TaskController:unLockMainlineTask()
    HostApi.log("unLockMainlineTask")
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end

    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_MIAN_LINE) or not DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA)  then
        return
    end

    local data = {}
    for k, v in pairs(self.mainLineTasks) do
        if player.progressesLevel == v.level then
            table.insert(data, v)
        end
    end

    for k, v in pairs(data) do
        if v.status ~= GameTask.TaskStatus.RECEIVED then
            return
        end
    end

    local cur_level = player.taskIsLandLevel
    if cur_level >= player.progressesLevel + 1 then
        player.progressesLevel = player.progressesLevel + 1
        player.taskController:unlockMainLineTaskByLevel(player, player.progressesLevel)
        self:unLockMainlineTask()
    end
end

function TaskController:ResetDareTask()
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if not player then
        return
    end
    local data = {}
    for i, v in ipairs(self.dareTasks) do
        if (v.status == GameTask.TaskStatus.RECEIVED) then
            table.insert(data, 0)
        else
            table.insert(data, v.quality)
        end
    end
    self.dareTasks = {}
    self:randomDareTask(data)
end

function TaskController:randomDareTask(qualit_list)

    local player = PlayerManager:getPlayerByUserId(self.userId)
    if not player then
        return
    end
    local score_level = TaskConfig:getScoreLevel(player.level)

    if qualit_list == nil or #qualit_list < 3 then
        qualit_list = {0, 0, 0}
    end

    local taskId_list = {}
    local quality_data = {}

    for i, v in ipairs(qualit_list) do
        table.insert(quality_data, LotteryUtil:get(1000 + v))
    end

    for i, value in ipairs(quality_data) do
        local count = 0
        local task_id = LotteryUtil:get((score_level - 1) * 4 + value) or 1
        local task_cfg = TaskConfig:getDareTaskByQualityAndId(value, task_id, score_level)
        while (TaskConfig:ListInclude(taskId_list, {quality = value, id = task_id}) or (not player.has_ad and task_cfg and task_cfg.TaskTypeId == Define.TaskType.look_ad))
        do
            task_id = LotteryUtil:get((score_level - 1) * 4 + value) or 1
            task_cfg = TaskConfig:getDareTaskByQualityAndId(value, task_id, score_level)
            count  = count + 1
            if (count > 5000 ) then
                break
            end
        end

        local info = {quality = value, id = task_id, scoreLevel = score_level}
        table.insert(taskId_list, info)

        if #taskId_list >= 3 then
            break
        end
    end
    self.dareTasks = {}
    for i, v in ipairs(taskId_list) do
        local task_data = TaskConfig:getDareTaskByQualityAndId(v.quality, v.id, v.scoreLevel)
        if task_data then
            if task_data.TaskTypeId == Define.TaskType.look_ad then
                self:addDareTask(GameTask.new(player.userId, task_data.Id, Define.TaskMain.Dare, nil, GameTask.TaskStatus.LOOK_AD))
            else
                self:addDareTask(GameTask.new(player.userId, task_data.Id, Define.TaskMain.Dare))
            end

        end
    end
    self:sendDareTaskToClient()
end

function TaskController:randomDareTaskReward(taskId, taskType)
    local config = TaskConfig.TaskList[taskId]
    if config == nil then
        return
    end
    local weigth = {}
    local totalNum = 0
    for i, v in ipairs(config.RewardWeigth) do
        totalNum = totalNum + tonumber(v)
        table.insert(weigth, tonumber(v))
    end

    for i, v in ipairs(weigth) do
        v = v / totalNum
    end

    LotteryUtil:initRandomSeed(TaskConfig.RANDOM_REWAARD_ID, weigth)
    local Id = LotteryUtil:get(TaskConfig.RANDOM_REWAARD_ID)
    local reward = config.RewardIds[1]
    if config.RewardIds[Id] then
        reward = config.RewardIds[Id]
    end
    return reward
end

return TaskController