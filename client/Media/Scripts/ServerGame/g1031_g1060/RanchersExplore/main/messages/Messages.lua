---
--- Created by Yaoqiang.
--- DateTime: 2018/6/22 0025 17:50
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(10035001)
end

function Messages:readyToStartTimeHint(seconds)
    return 10035002, tostring(seconds)
end

function Messages:WaitPlayerToStartHint()
    return 10035003
end

function Messages:TaskHasFinished()
    return 10035004
end

function Messages:GameOver()
    return 10035005
end

function Messages:PressureTip(num)
    return 10035006, tostring(num)
end

function Messages:PlayerNotEnough()
    return 10035007
end

function Messages:GameHasOver()
    return 10035008
end

return Messages