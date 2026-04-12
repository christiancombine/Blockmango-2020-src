GameTasks = class()

function GameTasks:ctor(userId)
    self.tasks = {}
    self.refreshTime = {}
    self.isStartTaskOperation = false
    self.userId = userId
    self:initRefreshTime()
end

function GameTasks:initRefreshTime()
    for i = 1, 6 do
        local item = {
            id = i,
            time = 0
        }
        table.insert(self.refreshTime, item)
    end
end

function GameTasks:startTaskOperation()
    self.isStartTaskOperation = true
end

function GameTasks:getTaskOperationStatus()
    return self.isStartTaskOperation
end

function GameTasks:initDataFromDB(data)
    self:startTaskOperation()
    self:initTasksFromDB(data.tasks or {})
    self:initTasksRefreshTimeFromDB(data.refreshTime or {})
end

function GameTasks:initTasksFromDB(tasks)
    for i, v in pairs(tasks) do
        local item = {}
        item.id = v.id
        item.uniqueId = v.uniqueId
        item.price = v.price
        item.status = v.status
        item.needTime = v.needTime
        item.startTime = v.startTime
        item.msg = v.msg
        item.guestIcon = v.guestIcon
        item.isHot = false
        if v.isHot == 1 then
            item.isHot = true
        end
        item.tasks = {}
        local _task = {}
        for j, t in pairs(v.tasks) do
            local task = {}
            task.askForHelpId = 0
            task.expandItemId = tonumber(t.expandItemId)
            task.expandNum = tonumber(t.expandNum)
            task.helperId = t.helperId
            task.helperSex = t.helperSex
            task.helperName = t.helperName
            task.isDone = false
            if t.isDone == 1 then
                task.isDone = true
            end
            task.isHelp = false
            if t.isHelp == 1 then
                task.isHelp = true
            end
            task.isHot = false
            if t.isHot == 1 then
                task.isHot = true
            end
            task.rewardItemId = tonumber(t.rewardItemId)
            task.rewardNum = tonumber(t.rewardNum)

            _task[j] = task
        end
        item.tasks = _task
        self.tasks[i] = item
    end
end

function GameTasks:initTasksRefreshTimeFromDB(refreshTime)
    if refreshTime == nil then
        return
    end

    for i, v in pairs(refreshTime) do
        local item = StringUtil.split(v, ":")
        self.refreshTime[i].time = tonumber(item[2])
    end
end

function GameTasks:initOriginalTasks(taskNum, playerLevel)
    for i = 1, taskNum do
        local task = TasksConfig:newTaskByLevel(playerLevel)
        if self.tasks[i] == nil then
            self.tasks[i] = task
        end
    end
end

function GameTasks:initRanchTask()
    local tasks = {}
    for i, _task in pairs(self.tasks) do
        local task = RanchOrder.new()
        task.id = i
        task.price = _task.price
        task.status = _task.status
        task.timeLeft = _task.needTime
        if _task.startTime ~= 0 then
            local timeLeft = _task.needTime - (os.time() - _task.startTime)
            if timeLeft <= 0 then
                timeLeft = 0
            end
            task.timeLeft = timeLeft * 1000
        end
        local refreshTimeLeft = self:getCoolDownTimeById(i) - (os.time() - self.refreshTime[i].time)
        if refreshTimeLeft <= 0 then
            refreshTimeLeft = 0
        end
        task.refreshTimeLeft = refreshTimeLeft * 1000
        task.msg = _task.msg
        task.guestIcon = _task.guestIcon
        task.isHot = _task.isHot
        local infos = {}
        local rewards = {}
        for j, v in pairs(_task.tasks) do
            local info = RanchOrderInfo.new()
            info.index = j
            info.itemId = v.expandItemId
            info.needNum = v.expandNum
            info.helperId = v.helperId
            info.helperSex = v.helperSex
            --info.helperName = v.helperName
            info.isDone = v.isDone
            info.isHelp = v.isHelp
            info.isHot = v.isHot
            infos[j] = info

            local reward = RanchCommon.new()
            reward.itemId = v.rewardItemId
            reward.num = v.rewardNum
            if v.isHelp == true then
                reward.userId = v.helperId
            end
            rewards[j] = reward
        end

        task.infos = infos
        task.rewards = rewards
        tasks[i] = task
    end

    return tasks
end

function GameTasks:prepareDataSaveToDB()
    local data = {
        tasks = self:prepareTasksToDB(),
        refreshTime = self:prepareRefreshTimeToDB()
    }
    --HostApi.log("GameTasks:prepareDataSaveToDB: data = " .. json.encode(data))
    return data
end

function GameTasks:prepareTasksToDB()
    local data = {}
    for i, v in pairs(self.tasks) do
        local item = {}
        item.id = v.id
        item.uniqueId = v.uniqueId
        item.price = v.price
        item.status = v.status
        item.needTime = v.needTime
        item.startTime = v.startTime
        item.msg = v.msg
        item.guestIcon = v.guestIcon
        item.isHot = 0
        if v.isHot == true then
            item.isHot = 1
        end

        item.tasks = {}
        local _task = {}
        for j, t in pairs(v.tasks) do
            local task = {}
            task.expandItemId = t.expandItemId
            task.expandNum = t.expandNum
            task.helperId = t.helperId
            task.helperSex = t.helperSex
            task.helperName = t.helperName
            task.isDone = 0
            if t.isDone == true then
                task.isDone = 1
            end
            task.isHelp = 0
            if t.isHelp == true then
                task.isHelp = 1
            end
            task.isHot = 0
            if t.isHot == true then
                task.isHot = 1
            end
            task.rewardItemId = t.rewardItemId
            task.rewardNum = t.rewardNum

            _task[j] = task
        end
        item.tasks = _task
        data[i] = item
    end

    return data
end

function GameTasks:prepareRefreshTimeToDB()
    local data = {}
    for i, v in pairs(self.refreshTime) do
        local item = v.id ..":" .. v.time
        data[i] = item
    end

    return data
end

function GameTasks:getTasksCanBeRefresh()
    local refreshTasksIndex = {}
    for i, v in pairs(self.tasks) do
        if v.status == 1 then
            refreshTasksIndex[i] = i
        end
    end
    return refreshTasksIndex
end

function GameTasks:refreshTaskByIndex(index, playerLevel)
    self.tasks[index] = nil
    local task = TasksConfig:newTaskByLevel(playerLevel)
    self.tasks[index] = task
end

function GameTasks:updateTasksListFromHttp(items)
    for i, v in pairs(self.tasks) do
        for j, item in pairs(items) do
            if i == item.orderId and v.uniqueId == item.uniqueId then
                if v.status == 1 then
                    v.status = 2
                end
                for k, task in pairs(v.tasks) do
                    if k == item.boxId then
                        task.askForHelpId = item.askForHelpId
                        task.isHelp = true
                        if item.helperId ~= 0 then
                            task.helperId = item.helperId
                            task.isDone = true
                            task.helperSex = item.helperSex
                            if item.helperId == item.userId then
                                task.isHelp = false
                            end
                        end
                    end
                end
            end
        end
    end

    for i, v in pairs(self.tasks) do
        local isFinish = true
        for j, task in pairs(v.tasks) do
            if task ~= nil then
                if task.isDone == false then
                    isFinish = false
                end
            else
                isFinish = false
            end
        end

        if isFinish == true then
            if v.status == 1 or v.status == 2 then
                v.status = 3
                v.startTime = os.time()
            end
        end
    end
end

function GameTasks:updateTasksStateById(id, userId)
    if self.tasks[id] == nil then
        return false
    end
    self.tasks[id].status = 4

    local player = PlayerManager:getPlayerByUserId(userId)
    if player ~= nil then
        local ranchTask = self:initRanchTask()
        player:getRanch():setOrders(ranchTask)
    end
end

function GameTasks:getTaskItemByIndex(orderId, index)
    if self.tasks[orderId] == nil then
        return nil
    end

    if self.tasks[orderId].tasks == nil then
        return nil
    end

    if self.tasks[orderId].tasks[index] == nil then
        return nil
    end

    local item = {}
    item.itemId = self.tasks[orderId].tasks[index].expandItemId
    item.num = self.tasks[orderId].tasks[index].expandNum
    return item
end

function GameTasks:updateTaskStateByOrderIdAndIndex(orderId, index)
    if self.tasks[orderId] == nil then
        return nil
    end

    if self.tasks[orderId].tasks[index] == nil then
        return nil
    end

    self.tasks[orderId].tasks[index].isDone = true
    if self.tasks[orderId].status == 1 then
        self.tasks[orderId].status = 2
    end

    for i, v in pairs(self.tasks) do
        local isDone = true
        for j, task in pairs(v.tasks) do
            if task.isDone == false then
                isDone = false
            end
        end

        if isDone == true then
            if v.status == 1 or v.status == 2 then
                v.status = 3
                v.startTime = os.time()
            end
        end
    end

end

function GameTasks:getTaskInfoByOrderIdAndIndex(orderId, index)
    if self.tasks[orderId] == nil then
        return nil
    end

    if self.tasks[orderId].tasks[index] == nil then
        return nil
    end

    return self.tasks[orderId].tasks[index]
end

function GameTasks:getRewardsByOrderId(orderId, userId)
    if self.tasks[orderId] == nil then
        return false
    end

    local rewards = {}
    local helperRewards = {}
    local selfRewards = {}

    local index = 1
    for i, v in pairs(self.tasks[orderId].tasks) do
        if tostring(v.helperId) == "0" or tostring(v.helperId) == tostring(userId) then
            local reward = RanchCommon.new()
            reward.itemId = v.rewardItemId
            reward.num = v.rewardNum
            selfRewards[index] = reward
            index = index + 1
        end
    end

    local helperIndex = 1
    for i, v in pairs(self.tasks[orderId].tasks) do
        if tostring(v.helperId) ~= "0" and tostring(v.helperId) ~= tostring(userId) then
            local helperReward = {}
            helperReward.helperId = v.helperId
            helperReward.itemId = v.rewardItemId
            helperReward.num = v.rewardNum
            helperRewards[helperIndex] = helperReward
            helperIndex = helperIndex + 1
        end
    end

    rewards.helperRewards = helperRewards
    rewards.selfRewards = selfRewards

    return rewards
end

function GameTasks:getUniqueIdByOrderId(orderId)
    if self.tasks[orderId] == nil then
        return 0
    end

    return self.tasks[orderId].uniqueId
end

function GameTasks:getFullBoxNumByOrderId(orderId)
    if self.tasks[orderId] == nil then
        return 0
    end

    local fullBoxNum = 0
    for i, v in pairs(self.tasks[orderId].tasks) do
        if v.isDone == true then
            fullBoxNum = fullBoxNum + 1
        end
    end

    return fullBoxNum
end

function GameTasks:getRefreshTimeById(id)
    for i, v in pairs(self.refreshTime) do
        if v.id == id then
            return v.time
        end
    end

    return 0
end

function GameTasks:isTaskCanRefreshById(id)
    if self.tasks[id].status == 1 then
        return true
    end

    return false
end

function GameTasks:getCoolDownTimeById(id)
    if self.tasks[id] == nil then
        return 0
    end

    return TasksConfig:getCoolDownTimeById(self.tasks[id].id)
end

function GameTasks:updateRefreshTimeById(id)
    for i, v in pairs(self.refreshTime) do
        if v.id == id then
            v.time = os.time()
        end
    end
end

