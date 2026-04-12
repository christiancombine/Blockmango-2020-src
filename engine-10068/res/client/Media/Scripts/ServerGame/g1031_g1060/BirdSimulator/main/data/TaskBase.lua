TaskBase = class("TaskBase")

function TaskBase:Init(player)
end

function TaskBase:OnNewTask()
    local message = {}
    message.title = self.title
    message.tips = {}
    message.interactionType = 104101
    message.sureContent = self.sureContent
    for i, val in ipairs(self.dialog) do
        message.tips[i] = val
    end
    return message
end

function TaskBase:OnCompleteTask()
    local message = {}
    message.title = self.title
    message.tips = {}
    message.interactionType = 104101
    for i, val in ipairs(self.dialogComplete) do
        message.tips[i] = val
    end
    return message
end

function TaskBase:OnTaskBeAccept(player)
end

function TaskBase:OnAcceptTask()
    local message = {}
    message.title = self.title
    message.tips = {}
    message.interactionType = 104101
    for i, val in ipairs(self.dialogAffter) do
        message.tips[i] = val
    end
    return message
end

function TaskBase:CreateTaskContent(userId, callback)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local taskCon = player:getTaskControl()
    if type(taskCon) ~= 'table' then
        return 
    end
    for _, contentId in pairs(self.requirement) do
        local content = TaskData.new(contentId, self)
        content:Init(userId)
        if type(callback) == "function" then
            callback(content)
        end
    end
    taskCon:OnCreateTaskSuccess(self.taskId)
end

function TaskBase:CreateOneContent(userId, contentId ,callback)
    local content = TaskData.new(contentId, self)  
    if type(callback) == "function" then
        callback(content)
    end
    content:Init(userId)
end