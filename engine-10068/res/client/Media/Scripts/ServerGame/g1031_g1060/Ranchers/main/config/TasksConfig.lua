TasksConfig = {}
TasksConfig.tasks = {}

function TasksConfig:initTasks(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.minLevel = tonumber(v.minLevel)
        data.maxLevel = tonumber(v.maxLevel)
        data.taskWeight = tonumber(v.taskWeight)
        data.quantity = tonumber(v.quantity)
        data.name = v.name
        data.content = v.content
        data.icon = v.icon
        data.time = tonumber(v.time)
        data.coolDownTime = tonumber(v.coolDownTime)
        data.refreshPrice = tonumber(v.refreshPrice)
        data.labelChance = tonumber(v.labelChance)
        data.labelNumChance = StringUtil.split(v.labelNumChance, "#")
        data.finishReward = StringUtil.split(v.finishReward, "#")
        data.demand = {}
        data.demand[1] = StringUtil.split(v.demand1, "#")
        data.demand[2] = StringUtil.split(v.demand2, "#")
        data.demand[3] = StringUtil.split(v.demand3, "#")
        data.demand[4] = StringUtil.split(v.demand4, "#")
        data.demand[5] = StringUtil.split(v.demand5, "#")
        data.demand[6] = StringUtil.split(v.demand6, "#")
        data.demand[7] = StringUtil.split(v.demand7, "#")
        data.demand[8] = StringUtil.split(v.demand8, "#")
        data.demand[9] = StringUtil.split(v.demand9, "#")
        data.demand[10] = StringUtil.split(v.demand10, "#")

        data.label = {}
        data.label[1] = StringUtil.split(v.label1, "#")
        data.label[2] = StringUtil.split(v.label2, "#")
        data.label[3] = StringUtil.split(v.label3, "#")
        data.label[4] = StringUtil.split(v.label4, "#")
        data.label[5] = StringUtil.split(v.label5, "#")
        data.label[6] = StringUtil.split(v.label6, "#")
        data.label[7] = StringUtil.split(v.label7, "#")
        data.label[8] = StringUtil.split(v.label8, "#")
        data.label[9] = StringUtil.split(v.label9, "#")
        data.label[10] = StringUtil.split(v.label10, "#")

        self.tasks[data.id] = data
    end
end

function TasksConfig:getTaskInfoByLevel(level)
    local tasks = {}
    local index = 1
    local weight = 0
    for i, v in pairs(self.tasks) do
        if v.minLevel <= level and v.maxLevel >= level then
            weight = weight + v.taskWeight
            tasks[index] = v
            index = index + 1
        end
    end

    if weight < 1 then
        weight = 1
    end

    local randWeight =  math.random(1, weight)
    local num = 0
    for i, v in pairs(tasks) do
        if randWeight >= num and randWeight <= num + v.taskWeight then
            return tasks[i]
        end
        num = num + v.taskWeight
    end

    return tasks[1]
end

function TasksConfig:newTaskByLevel(level)
    local task = TasksConfig:getTaskInfoByLevel(level)
    local isLabel = false
    local labelChance = math.random(1, 100)
    if labelChance < task.labelChance then
        isLabel = true
    end

    local taskItems = {}
    for i = 1, task.quantity do
        local randNum = 0
        local taskRandNum = math.random(1, 100)
        for _, v in pairs(task.demand[i]) do
            local item = StringUtil.split(v, ":")
            if taskRandNum > randNum then
                taskItems[i] = {}
                taskItems[i].askForHelpId = 0
                taskItems[i].expandItemId = tonumber(item[1])
                taskItems[i].expandNum = tonumber(item[2])
                taskItems[i].rewardItemId = tonumber(item[3])
                taskItems[i].rewardNum = tonumber(item[4])
                taskItems[i].isHot = false
                taskItems[i].isDone = false
                taskItems[i].isHelp = false
                taskItems[i].helperId = 0
                taskItems[i].helperName = ""
                taskItems[i].helperSex = 0      -- 0. init, 1. boy, 2. girl
                --taskItems[i].chance = item[5]
            end
            randNum = randNum + item[5]
        end
    end

    local items = {}
    items.id = task.id
    local uuid = require "base.util.uuid"
    items.uniqueId = uuid()
    items.price = task.refreshPrice
    items.status = 1    -- 1. init, 2. start (not complete), 3. waiting (complete), 4.complete
    items.needTime = task.time
    items.startTime = 0
    items.msg = task.content
    items.guestIcon = task.icon
    items.isHot = false
    items.tasks = taskItems

    if isLabel == true then
        local taskLabelItems = {}
        for i = 1, task.quantity do
            local randNum = 0
            local taskRandNum = math.random(1, 100)
            for _, v in pairs(task.label[i]) do
                local item = StringUtil.split(v, ":")
                if taskRandNum > randNum then
                    taskLabelItems[i] = {}
                    taskLabelItems[i].askForHelpId = 0
                    taskLabelItems[i].expandItemId = tonumber(item[1])
                    taskLabelItems[i].expandNum = tonumber(item[2])
                    taskLabelItems[i].rewardItemId = tonumber(item[3])
                    taskLabelItems[i].rewardNum = tonumber(item[4])
                    taskLabelItems[i].isHot = true
                    taskLabelItems[i].isDone = false
                    taskLabelItems[i].isHelp = false
                    taskLabelItems[i].helperId = 0
                    taskLabelItems[i].helperName = ""
                    taskLabelItems[i].helperSex = 0     -- 0. init, 1. boy, 2. girl
                    --taskLabelItems[i].chance = item[5]
                end
                randNum = randNum + tonumber(item[5])
            end
        end

        local num = 0
        local labelNum = 1
        local labelRandNum = math.random(1, 100)
        for i, v in pairs(task.labelNumChance) do
            local item = StringUtil.split(v, ":")
            num = num + tonumber(item[2])
            if labelRandNum > num then
                labelNum = labelNum + 1
            end
        end

        for i = 1, labelNum do
            items.isHot = true
            items.tasks[i] = taskLabelItems[i]
        end
    end

    return items
end

function TasksConfig:getFinishRewardById(id)
    local items = {}
    if self.tasks[id] == nil then
        return items
    end

    for i, v in pairs(self.tasks[id].finishReward) do
        local data = StringUtil.split(v, ":")
        local item = {}
        item.itemId = data[1]
        item.num = data[2]
        table.insert(items, item)
    end

    return items
end

function TasksConfig:getCoolDownTimeById(id)
    if self.tasks[id] == nil then
        return 0
    end

    return self.tasks[id].coolDownTime
end

return TasksConfig