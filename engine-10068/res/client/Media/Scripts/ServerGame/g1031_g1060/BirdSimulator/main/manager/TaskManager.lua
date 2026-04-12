require "config.TaskConfig"
require "data.MainTaskNpc"
require "data.DailyTaskNpc"

TaskManager = {}

function TaskManager:Init()
    self.taskNpcs = {}
    local npcs = TaskConfig.npc
    for _, info in pairs(npcs) do
        local npc, npcId = nil
        if tonumber(info.npcType) == 1 then
            npc = DailyTaskNpc.new(info)
            npcId = npc:OnCreate()
        elseif tonumber(info.npcType) == 2 then
            npc = MainTaskNpc.new(info)
            npcId = npc:OnCreate()
        end
        if npc and npcId then
            self.taskNpcs[npcId] = npc
        end
    end
end

function TaskManager:InitPlayerTaskInfo(player)
    for _, npc in pairs(self.taskNpcs) do
        npc:InitPlayerTaskInfo(player)
    end
end

function TaskManager:getDailyTaskNpc()
    for _, npc in pairs(self.taskNpcs) do
        if npc.npcType == 1 then
            return npc
        end
    end
    return nil
end

function TaskManager:GetNpcById(id)
    local npc = self.taskNpcs[id]
    return npc
end

function TaskManager:getTaskByTaskId(taskId)
    for k, npc in pairs(self.taskNpcs) do
        local task = npc:getTaskData(taskId)
        if type(task) == "table" then
            return npc.npcId, task
        end
    end
end

function TaskManager:OnPlayerAcceptTask(player, npcId, taskId)
    local npc = self.taskNpcs[npcId]
    TaskHelper.dispatch(TaskType.NpcDialogueEvent, player.userId, npc.id, 1)
    if taskId == -1 then
        return false
    end
    local taskCon = player:getTaskControl()

    if type(npc) == "table" and taskCon then
        if taskCon:CheckTaskIsReceive(taskId, npc) == true then
            return true
        end
        if npc.npcType == 2 then
            if taskCon:acceptMainTask(player, npc, taskId) then
                return true
            end
        elseif npc.npcType == 1 then
            if taskCon:acceptDailyTask(player, npc, taskId) then
                return true
            end
        end
    end
    return false
end
