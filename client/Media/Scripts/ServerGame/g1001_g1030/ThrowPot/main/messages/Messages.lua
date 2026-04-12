---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:11
---

Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(10026001)
end

function Messages:msgUseSkillNoRunning()
    return 10026002
end

function Messages:msgBuySkillHaveSkill()
    return 10026003
end

function Messages:msgUseSkillHint(name, tip)
    return 10026004, name .. TextFormat.argsSplit .. TextFormat:getTipType(tip)
end

function Messages:msgBecomeTNTPlayer(name)
    return 10026012, name
end

function Messages:msgGetChipHint(tip)
    return 10026013, TextFormat:getTipType(tip)
end

function Messages:msgHitTNTHint(target, throw)
    return 10026021, target .. TextFormat.argsSplit .. throw
end

function Messages:msgEnterGame(name)
    return 10026022, name
end

function Messages:msgGameRule()
    return 10026023
end

function Messages:msgGameStartNoTNT()
    return 10026024
end

function Messages:msgGameStartHasTNT()
    return 10026025
end

function Messages:msgBecomeNormalPlayer(name)
    return 10026026, name
end

function Messages:msgBoomToTarget()
    return 10026027
end

function Messages:msgBoomToAll(name)
    return 10026028, name
end

function Messages:msgDeathmatchMode()
    return 10026029
end

function Messages:msgSelectSkillHint(tip)
    return 10026030, TextFormat:getTipType(tip)
end

function Messages:msgGameData(rounds, action, lifes)
    if action == 0 then
        return 10026031, tostring(rounds) .. TextFormat.argsSplit ..
        self:msgActionRun() .. TextFormat.argsSplit ..
        tostring(lifes)
    else
        return 10026031, tostring(rounds) .. TextFormat.argsSplit ..
        self:msgActionAttack() .. TextFormat.argsSplit ..
        tostring(lifes)
    end

end

function Messages:msgActionRun()
    return TextFormat:getTipType(10026032)
end

function Messages:msgActionAttack()
    return TextFormat:getTipType(10026033)
end

function Messages:msgRoundStartHint(rounds)
    return 10026034, tostring(rounds)
end

function Messages:msgTNTPlayersHint(names)
    return 10026035, names
end

function Messages:msgRewardHint(score, money)
    return 10026036, tostring(score) .. TextFormat.argsSplit .. tostring(money)
end

return Messages