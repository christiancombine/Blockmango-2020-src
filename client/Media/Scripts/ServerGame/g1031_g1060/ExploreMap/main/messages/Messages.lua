---
--- Created by Yaoqiang.
--- DateTime: 2018/6/22 0025 17:50
---
Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1023501)
end

function Messages:readyToStartTimeHint(seconds)
    return 1023502, tostring(seconds)
end

function Messages:msgBuildEndTimeHint(seconds)
    return 1023503, tostring(seconds)
end

function Messages:msgReadytoGrade()
    return 1023504
end

function Messages:msgStartGuess()
    return 1023505
end

function Messages:msgGuessLeft(seconds)
    return 1023506, tostring(seconds)
end

function Messages:msgStartGuessOther()
    return 1023507
end

function Messages:msgStartGuessSelf()
    return 1023508
end

function Messages:msgShowYourGrade()
    return 1023509
end

function Messages:msgShowYourFinalGrade()
    return 1023515
end

function Messages:msgShowWinnner(name)
    return 1023521, tostring(name)
end

function Messages:msgCannotGradeSelf()
    return 1023522
end


return Messages