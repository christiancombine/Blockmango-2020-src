---
--- Created by Jimmy.
--- DateTime: 2018/3/6 0006 11:30
---
TaskConfig = {}
TaskConfig.task = {}
TaskConfig.task_group = {}
TaskConfig.match_task = {}
TaskConfig.Status = {}
TaskConfig.Status.NotFinish = 0
TaskConfig.Status.Finish = 1

function TaskConfig:init(config)
    self:initTask(config)
    self:initTaskGroup(config)
end

function TaskConfig:initTask(config)
    for i, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.id)
        item.desc = tonumber(v.desc)
        item.type = tonumber(v.type)
        item.num = tonumber(v.num)
        item.param = tonumber(v.param)
        item.rewardMoney = tonumber(v.rewardMoney)
        item.rewardTime = tonumber(v.rewardTime)
        item.icon = tostring(v.icon)

        self.task[item.id] = item
    end
end

function TaskConfig:initTaskGroup(config)
    for i, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.groupId)
        local taskgroup = StringUtil.split(v.taskgroup, "#")

        item.task_list = {}
        for i = 1, #taskgroup do
            table.insert(item.task_list, tonumber(taskgroup[i]))
        end

        self.task_group[item.id] = item
    end
end

function TaskConfig:prepareTask()
    math.randomseed(tostring(os.time()):reverse():sub(1, 7))
    local random = math.random(GameConfig.taskInfo[1], GameConfig.taskInfo[2])

    if self.match_task then

        self.match_task.groupId = self.task_group[random].id
        self.match_task.task_list = {}

        for k, v in pairs(self.task_group[random].task_list) do
            local item = {}
            item.task_id = v
            item.task_status = TaskConfig.Status.NotFinish
            item.task_num = self:getTaskNumById(v)
            item.task_type = self:getTaskTypeById(v)
            item.task_des = self:getTaskDesById(v)
            item.task_param = self:getTaskParamById(v)
            item.task_icon = self:getTaskIconById(v) or ""

            table.insert(self.match_task.task_list, item)
        end
    end
end

function TaskConfig:getTaskNumById(id)
    for k, v in pairs(self.task) do
        if v.id == id then
            return v.num
        end
    end 
    return 0
end

function TaskConfig:getTaskTypeById(id)
    for k, v in pairs(self.task) do
        if v.id == id then
            return v.type
        end
    end 
    return 0
end

function TaskConfig:getTaskDesById(id)
    for k, v in pairs(self.task) do
        if v.id == id then
            return v.desc
        end
    end 
    return 0
end

function TaskConfig:getTaskParamById(id)
    for k, v in pairs(self.task) do
        if v.id == id then
            return v.param
        end
    end 
    return 0
end

function TaskConfig:getTaskIconById(id)
    for k, v in pairs(self.task) do
        if v.id == id then
            return v.icon
        end
    end 
    return ""
end

function TaskConfig:getTaskRewardMoneyById(id)
    for k, v in pairs(self.task) do
        if v.id == id then
            return v.rewardMoney
        end
    end 
    return 0
end

function TaskConfig:getTaskRewardTimeById(id)
    for k, v in pairs(self.task) do
        if v.id == id then
            return v.rewardTime
        end
    end 
    return 0
end

return TaskConfig