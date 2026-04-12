---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:msgNotOperateTip()
    return 1045001
end

function Messages:msgGameStartTip()
    return 1045002
end

function Messages:msgWaitMatching()
    return 10042002
end

function Messages:msgCreatSafe(time)
    return 1053001, tostring(time)
end

function Messages:msgIsShringking()
    return 1053002
end

function Messages:msgWaitBackHall()
    return 1045003
end

function Messages:msgReviveTip(time)
    return 1053003, tostring((time))
end
return Messages