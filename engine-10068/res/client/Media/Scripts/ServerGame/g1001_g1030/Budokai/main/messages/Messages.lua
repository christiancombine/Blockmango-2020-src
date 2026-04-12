---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(10028001)
end

function Messages:msgGameData(round, name, status)
    local match = 10028007
    if status == 0 then
        match = 10028007
    elseif status == 1 then
        match = 10028008
    elseif status == 2 then
        match = 10028009
    end
    return 10028002, TextFormat:getTipType(round) .. TextFormat.argsSplit .. name .. TextFormat.argsSplit .. TextFormat:getTipType(match)
end

function Messages:msgGameBuyTip()
    return 10028010
end

function Messages:msgWatchModeTip()
    return 10028011
end

function Messages:msgGamePrepareTip(round)
    return 10028012, TextFormat:getTipType(round)
end

function Messages:msgGameStartedTip(round)
    return 10028013, TextFormat:getTipType(round)
end

function Messages:msgNoOpponentTip()
    return 10028014
end

function Messages:msgDefeatTip(winner, loser)
    return 10028015, winner .. TextFormat.argsSplit .. loser
end

function Messages:msgFleeTip()
    return 10028016
end

function Messages:msgGetScore(score)
    return 10028017, tostring(score)
end

function Messages:msgGetMoney(money)
    return 10028018, tostring(money)
end

function Messages:msgNoProps()
    return 10028019
end

function Messages:msgPkScoreTip(score)
    return 10028020, score
end

function Messages:msgWinTip()
    return 10028021
end

function Messages:msgLoseTip()
    return 10028022
end

return Messages