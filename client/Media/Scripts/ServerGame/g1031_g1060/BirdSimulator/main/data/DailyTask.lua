require "data.TaskBase"
DailyTask = class("DailyTask",TaskBase)

function DailyTask:ctor(config, npc)
    self.taskId = config.taskId
    self.receiveNpcId = config.receiveNpcId
    self.requirement = config.requirement
    self.otherRequirement = config.otherRequirement
    self.minBox = config.minBox
    self.maxBox = config.maxBox
    self.weight = config.weight
    self.waitTime = config.waitTime
    self.rewards = config.rewards
    self.num = config.num
    self.sureContent = config.sureContent
    self.title = config.title
    self.dialog = config.dialog
    self.dialogAffter = config.dialogAffter
    self.bindNpcId = npc.npcId
    self.bindNpcName = npc.name
    self.taskType = 1
    self.dialogComplete = config.dialogComplete
    self.contenList = {}
end

function DailyTask:getTaskType()
    return self.taskType
end

function DailyTask:Init(player)
    local taskCon = player:getTaskControl()
    local result = taskCon:addCanAcceptDailyTask(self.taskId)
    if result == TaskStatus.canReceive or result == TaskStatus.doTask then
        return result
    end
    local boxNum = player.userBirdBag.maxCarryBirdNum
    if type(boxNum) == 'number' then
        if boxNum >= self.minBox and boxNum <= self.maxBox then
            return taskCon:addCanAcceptDailyTask(self.taskId)
        end
    end
    return nil
end
