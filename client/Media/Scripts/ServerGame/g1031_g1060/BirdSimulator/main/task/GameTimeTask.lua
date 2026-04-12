
local CGameTimeTask = class("GameTimeTask", ITask)

function CGameTimeTask:onTick(ticks)
    GameMatch:onTick(ticks)
end

GameTimeTask = CGameTimeTask.new(1)

return GameTimeTask
