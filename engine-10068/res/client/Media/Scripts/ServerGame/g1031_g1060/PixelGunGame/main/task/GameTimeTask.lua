--region *.lua

local CGameTimeTask = class("GameTimeTask", ITask)

function CGameTimeTask:onTick(ticks)
    GameMatch:onTick(ticks)
end

function CGameTimeTask:onCreate()
    GameMatch.Process.curStatus = self.status
end

GameTimeTask = CGameTimeTask.new(1)

return GameTimeTask

--endregion
