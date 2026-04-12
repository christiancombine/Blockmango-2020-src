require "data.GameTaskNpc"
DailyTaskNpc = class("DailyTaskNpc", GameTaskNpc)

function DailyTaskNpc:ctor(config)
    self.id = config.id
    self.name = config.name
    self.actorName = config.actorName
    self.actorBody = config.actorBody
    self.actorBodyId = config.actorBodyId
    self.pos = config.pos
    self.yaw = config.yaw
    self.areaMinPos = config.areaMinPos
    self.areaMaxPos = config.areaMaxPos
    self.normalDialog = config.normalDialog
    self.normalTitle = config.normalTitle
    self.npcType = tonumber(config.npcType) -- 1 日常 2  主线
    self.tipActorName = config.tipActorName
    self.tipActorBody = config.tipActorBody
    self.tipActorBodyId = config.tipActorBodyId
    self.tipPos = config.tipPos
    self.canReceiveAction = config.canReceiveAction
    self.defaultAction = config.defaultAction
    self.taskList = {}
    self.taskIdList = {}
end

function DailyTaskNpc:OnCreate()
    local npcId = self:CreateSessionNpc()
    self:OnCreateSuccess()
    return npcId
end

function DailyTask:RefreshCountDown()
end

function DailyTaskNpc:OnCreateSuccess()
    local taskConfigs = TaskConfig:getDailyTaskByNpcindex(self.id)
    for _, info in pairs(taskConfigs) do
        local task = DailyTask.new(info, self)
        self.taskList[info.taskId] = task
        table.insert(self.taskIdList, info.taskId)
    end
    self:upsetMap()
end

function DailyTaskNpc:InitPlayerTaskInfo(player)
    for _, taskId in pairs(self.taskIdList) do
        local task = self.taskList[taskId]
        local result = task:Init(player)
        if result ~= nil then
            if result == TaskStatus.canReceive then
                self:OnCompleteTask(player, taskId)
                return
            elseif result == TaskStatus.doTask then
                self:OnAcceptTask(player, taskId)
                return
            end
        end
    end
    local taskControl = player:getTaskControl()
    if type(taskControl) == "table" then
        local countDown = taskControl:getInterval()
        if countDown > 0 then
            self:OnNotTask(player, countDown * 1000)
            return
        end
    end
    for _, taskId in pairs(self.taskIdList) do
        local task = self.taskList[taskId]
        local result = task:Init(player)
        if result == TaskStatus.newTask then
            self:OnNewTask(player, taskId)
            return
        end
    end
    self:OnNotTask(player)
end
