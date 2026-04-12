--region *.lua

ITask = class()
ITask.timer = 0
ITask.status = 0
ITask.created = false

function ITask:ctor(status, period)
    local taskFunc = function(ticks)
        if not self.created then
            self.created = true
            self:onCreate()
        end
        self:onTick(ticks)
    end
    if not period then
        self.timer = SecTimer.createTimer(taskFunc)
    else
        self.timer = SecTimer.createTimerWithPeriod(taskFunc, period)
    end
    self.status = status
end

function ITask:onCreate()

end

function ITask:onTick(ticks)

end

function ITask:start()
    SecTimer.startTimer(self.timer)
end

function ITask:stop()
    SecTimer.stopTimer(self.timer)
end

function ITask:pureStop()
    SecTimer:stopTimer(self.timer)
end

return ITask
--endregion
