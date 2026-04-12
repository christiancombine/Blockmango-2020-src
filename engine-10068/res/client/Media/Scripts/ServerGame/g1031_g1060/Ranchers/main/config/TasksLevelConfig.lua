TasksLevelConfig = {}
TasksLevelConfig.tasksLevel = {}

function TasksLevelConfig:initTasksLevel(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.minLevel = tonumber(v.minLevel)
        data.maxLevel = tonumber(v.maxLevel)
        data.num = tonumber(v.num)
        self.tasksLevel[data.id] = data
    end
end

function TasksLevelConfig:getNumByLevel(level)
    for i, v in pairs(self.tasksLevel) do
        if v.minLevel <= level and v.maxLevel >= level then
            return v.num
        end
    end

    return self.tasksLevel[#self.tasksLevel].num
end