TaskConfig = {}
TaskConfig.mainTasks = {}
TaskConfig.contents = {}
TaskConfig.npc = {}
TaskConfig.requirement = {}
TaskConfig.dailyTasks = {}

-- 主线任务
function TaskConfig:initMainTasks(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.taskId = tonumber(v.taskId)
        data.receiveNpcId = tonumber(v.receiveNpcId)
        data.requirement = Tools.SplitNumber(v.requirement, "#")
        data.lastTaskId = tonumber(v.lastTaskId)
        data.otherRequirement = Tools.SplitNumber(v.otherRequirements, "#")
        data.rewards = Tools.SplitNumber(v.rewards, "#")
        data.num = Tools.SplitNumber(v.num, "#")
        data.icons = StringUtil.split(v.icons, "#")
        data.dialog = StringUtil.split(v.dialog, "#")
        data.sureContent = v.sureContent

        data.dialogAffter = StringUtil.split(v.dialogAffter, "#")
        data.dialogComplete = StringUtil.split(v.dialogComplete, "#")
        data.title = v.title
        data.names = StringUtil.split(v.names, "#")
        self.mainTasks[data.taskId] = data
    end
end

function TaskConfig:getMainTaskReward(taskId)
    local task = self.mainTasks[taskId]

    if task == nil then
        return nil
    end
    local tab = {}
    for index = 1, #task.rewards do
        local item = {}
        item.num = task.num[index]
        item.id = task.rewards[index]
        item.icon = task.icons[index]
        item.name = task.names[index]
        table.insert(tab, item)
    end
    return tab
end

function TaskConfig:getDailyTaskReward(taskId)
    local task = self.dailyTasks[taskId]

    if task == nil then
        return nil
    end
    local tab = {}
    for index = 1, #task.rewards do
        local item = {}
        item.num = task.num[index]
        item.id = task.rewards[index]
        item.icon = task.icons[index]
        item.name = task.names[index]
        table.insert(tab, item)
    end
    return tab
end

function TaskConfig:GetTaskListByNpcId(npcId)
    local taskList = {}
    for _, info in pairs(self.mainTasks) do
        if info.receiveNpcId == npcId then
            taskList[info.taskId] = info
        end
    end
    return taskList
end

function TaskConfig:initNpc(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.name = v.name
        data.actorName = v.actorName
        data.actorBody = v.actorBody
        data.actorBodyId = v.actorBodyId

        data.pos = Tools.CastToVector3(v.x, v.y, v.z)
        data.yaw = tonumber(v.yaw)
        data.areaMinPos = Tools.CastToVector3i(v.x1, v.y1, v.z1)
        data.areaMaxPos = Tools.CastToVector3i(v.x2, v.y2, v.z2)
        data.npcType = v.npcType
        data.normalTitle = v.normalTitle
        data.normalDialog = StringUtil.split(v.normalDialog, "#")
        data.tipActorName = v.tipActorName
        data.tipActorBody = StringUtil.split(v.tipActorBody, "#")
        data.tipActorBodyId = StringUtil.split(v.tipActorBodyId, "#")
        data.tipPos = Tools.CastToVector3(v.tipx, v.tipy, v.tipz)
        data.canReceiveAction = v.canReceiveAction
        data.defaultAction = v.defaultAction
        self.npc[data.id] = data
    end
end

function TaskConfig:initContents(config)
    for i, v in pairs(config) do
        local data = {}
        data.contentId = tonumber(v.contentId)
        data.type = tonumber(v.type)
        --data.id = tonumber(v.Id)
        data.id = Tools.SplitNumber(v.Id)
        data.num = tonumber(v.num)
        data.color = tonumber(v.color)
        data.desc = v.desc
        self.contents[data.contentId] = data
    end
end

function TaskConfig:getTaskContentById(id)
    local tab = self.contents[id]
    return tab
end

function TaskConfig:initTaskRequirement(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.type = tonumber(v.type)
        data.value1 = tonumber(v.value1)
        data.value2 = tonumber(v.value2)
        self.requirement[data.id] = data
    end
end

function TaskConfig:initDailyTaskConfigs(configs)
    for k, config in pairs(configs) do
        local data = {}
        data.taskId = tonumber(config.taskId)
        data.receiveNpcId = tonumber(config.receiveNpcId)
        data.requirement = Tools.SplitNumber(config.requirement)
        data.otherRequirement = Tools.SplitNumber(config.otherRequirement)
        data.minBox = tonumber(config.minBox)
        data.maxBox = tonumber(config.maxBox)
        data.weight = tonumber(config.weight)
        data.waitTime = tonumber(config.waitTime)
        data.rewards = Tools.SplitNumber(config.rewards)
        data.num = Tools.SplitNumber(config.num)
        data.icons = StringUtil.split(config.icons, "#")
        data.sureContent = config.sureContent
        data.title = config.title
        data.dialogAffter = StringUtil.split(config.dialogAffter, "#")
        data.dialog = StringUtil.split(config.dialog, "#")
        data.dialogComplete = StringUtil.split(config.dialogComplete, "#")
        data.names = StringUtil.split(config.names, "#")
        self.dailyTasks[data.taskId] = data
    end
end

function TaskConfig:getIntervalByTaskId(taskId)
    local task = self.dailyTasks[taskId]
    if task ~= nil then
        return task.waitTime
    end
    return 0
end

function TaskConfig:getDailyTaskByNpcindex(npcIndex)
    local tab = {}
    for _, data in pairs(self.dailyTasks) do
        if data.receiveNpcId == npcIndex then
            table.insert(tab, data)
        end
    end
    return tab
end

return TaskConfig
