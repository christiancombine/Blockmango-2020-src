require "config.TaskConfig"
require "data.TaskData"
require "data.TaskBase"
GameMainTask = class("GameMainTask", TaskBase)

function GameMainTask:ctor(config, npc)
    self.taskId = config.taskId
    self.receiveNpcId = config.receiveNpcId
    self.requirement = config.requirement
    self.lastTaskId = config.lastTaskId
    self.otherRequirement = config.otherRequirement
    self.rewards = config.rewards
    self.num = config.num
    self.sureContent = config.sureContent
    self.title = config.title
    self.dialog = config.dialog
    self.dialogAffter = config.dialogAffter
    self.bindNpcId = npc.npcId
    self.bindNpcName = npc.name
    self.taskType = 2
    self.dialogComplete = config.dialogComplete
    self.contenList = {}
end

function GameMainTask:getTaskType()
    return self.taskType
end

function GameMainTask:Init(player)
    local taskCon = player:getTaskControl()
    local res1 = taskCon:CheckMainTaskIsComplete(self.taskId)
    local res2 = taskCon:CheckMainTaskIsComplete(self.lastTaskId)
    -- 假如当前任务没有完成  而 前置任务完成了  添加到可领取任务列表
    if res1 == false then
        if res2 == true or self.lastTaskId == 0 then
            return taskCon:addCanAcceptMainTask(self.taskId)
        end
    end
    return nil
end
