IMessages = {}

IMessages.UNIT_MINUTE = 0
IMessages.UNIT_SECOND = 1

IMessages.TYPE_PLAYER = 0
IMessages.TYPE_TEAM = 1

function IMessages:msgWelcomePlayer(game)
    return 3, tostring(game)
end

function IMessages:msgWaitPlayerEnough(time, unit, ifHalf)
    return 4, self:msgTimeTick(time, unit, ifHalf)
end

function IMessages:msgGameStartTimeHint(time, unit, ifHalf)
    return 5, self:msgTimeTick(time, unit, ifHalf)
end

function IMessages:msgGameEndTimeHint(time, unit, ifHalf)
    return 6, self:msgTimeTick(time, unit, ifHalf)
end

function IMessages:msgPvpEnableAfter(time, unit, ifHalf)
    return 7, self:msgTimeTick(time, unit, ifHalf)
end

function IMessages:msgFightGameStartTimeHint(time, unit, ifHalf)
    return 8, self:msgTimeTick(time, unit, ifHalf)
end

function IMessages:msgCloseServerTimeHint(time, unit, ifHalf)
    return 9, self:msgTimeTick(time, unit, ifHalf)
end

function IMessages:msgGameStart()
    return 10
end

function IMessages:msgReadyHint(time)
    return 11, tostring(time)
end

function IMessages:msgGameOverByLittle()
    return 12
end

function IMessages:msgPlayerLeft(num)
    return 13, tostring(num)
end

function IMessages:msgPlayerCount(num)
    return 14, tostring(num)
end

function IMessages:msgPlayerQuit(name)
    return 15, name
end

function IMessages:msgPlayerKillPlayer(deceased, killer)
    return 16, deceased .. TextFormat.argsSplit .. killer
end

function IMessages:msgFirstKill(name)
    return 17, name
end

function IMessages:msgDoubleKill(name)
    return 18, name
end

function IMessages:msgThirdKill(name)
    return 19, name
end

function IMessages:msgFiveKill(name)
    return 20, name
end

function IMessages:msgFightGameStart()
    return 21
end

function IMessages:msgWinnerInfo(name)
    return 22, name
end

function IMessages:msgTeleportToDeadPlace(second)
    return 31, tostring(second)
end

function IMessages:msgGameOverWin()
    return 24
end

function IMessages:msgGameOverDeath()
    return 25
end

function IMessages:msgGameOver()
    return 23
end
function IMessages:msgGameOverByDBDataError()
    return 165
end

function IMessages:msgNoWinner(type)
    if type == self.TYPE_PLAYER then
        return 26
    else
        return 27
    end
end

function IMessages:msgGameScore(score)
    return 73, score
end

function IMessages:msgWaitPlayer()
    return 74
end

function IMessages:msgCanStartGame()
    return 75
end

function IMessages:msgGameRunning()
    return 82
end

function IMessages:msgGamePlayerNum(curPlayers, startPlayers)
    return 83, curPlayers .. TextFormat.argsSplit .. startPlayers
end

function IMessages:msgGamePlayerIsEnough()
    return 86
end

function IMessages:msgGameHasStarting()
    return 87
end

function IMessages:msgDontBuyGoodsByNoStart()
    return 88
end

function IMessages:msgRespawn(name)
    return 143, name
end

function IMessages:msgGameOverHint(seconds)
    return 28, tostring(seconds);
end

function IMessages:msgGameResetHint()
    return 29;
end

function IMessages:longWaitingHint()
    return 20053
end

-- unit 0:minute 1:second
function IMessages:msgTimeTick(time, unit, ifHalf)
    if unit == 0 then
        if ifHalf then
            return TextFormat.colorRed .. NumberUtil.getIntPart(time) .. TextFormat:getTipType(1) ..
                    TextFormat.argsSplit .. TextFormat.colorRed .. "30" .. TextFormat:getTipType(2);
        else
            return TextFormat.colorRed .. time .. TextFormat.argsSplit .. TextFormat:getTipType(1);
        end
    else
        return TextFormat.colorRed .. tostring(time) .. TextFormat.argsSplit .. TextFormat:getTipType(2);
    end
end

return IMessages
