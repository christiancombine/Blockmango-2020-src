--region *.lua
Messages = {}

function Messages:gamename()
    return TextFormat:getTipType(1025001);
end

function Messages:selectRoleHint(name)
    return 20068, name
end

function Messages:longWaitingHint()
    return 20053
end

function Messages:hasDomainHint()
    return 1025002
end

function Messages:domainHasPlayer()
    return 1025003
end

function Messages:hintPlayerSelectOcc()
    return 1025004
end

function Messages:noMoney()
    return 1025005
end

function Messages:rewradLadder()
    return 1025006
end

function Messages:buildsucceed()
    return 1025007
end

function Messages:publicResourceHint()
    return 1025008
end

function Messages:enterResource()
    return 1025009
end

function Messages:hintGetResourceCd(second)
    return 1025010, second
end

function Messages:informResourceByStealed(name)
    return 1025011, name
end

function Messages:succeedBuildingOne(name)
    return 1025012, name
end

function Messages:succeedBuildingTwo(name)
    return 1025013, name
end

function Messages:succeedBuildingPortal(name)
    return 1025014, name
end

function Messages:noSelfItem()
    return 1025015
end

function Messages:enhanceResourceOutput()
    return 1025016
end

function Messages:enhanceAddHp()
    return 1025017
end

function Messages:enhanceAddMoveSpeed()
    return 1025018
end

function Messages:enhanceDurationReturnHp()
    return 1025019
end

function Messages:enhanceAddStealPercent()
    return 1025020
end

function Messages:enhanceAddDefense()
    return 1025021
end

function Messages:enterDomainArea()
    return 1025022
end

function Messages:quitDomainArea()
    return 1025023
end

function Messages:clothceHave()
    return 1025024
end

function Messages:killPlayer(killer, dier, money)
    return 1025025, killer .. TextFormat.argsSplit .. dier .. TextFormat.argsSplit .. money
end

function Messages:skillHave()
    return 1025026
end

function Messages:extraPortalStart()
    return 1025027
end

function Messages:portalOverload(second)
    return 1025028, second
end

function Messages:enhanceLowHpHeigthDefense()
    return 1025029
end

function Messages:enhanceLowHpHeigthLeech()
    return 1025030
end

function Messages:enhanceLowHpHeigthAttack()
    return 1025031
end

function Messages:enhanceLowHpTeleport()
    return 1025032
end

function Messages:enhanceAddAttackByKill()
    return 1025033
end

function Messages:enhanceAddResourceOutput()
    return 1025034
end

function Messages:enhanceGetMoneyByKill()
    return 1025035
end

function Messages:enhanceAddMaxhpByKill()
    return 1025036
end

function Messages:enhanceAddAttack()
    return 1025037
end

function Messages:noBuyCoin()
    return 1025038
end

function Messages:canBuyInGameRunning()
    return 1025039
end

function Messages:areUnlocked()
    return 1025040
end

function Messages:occSpecialHas()
    return 1025041
end

function Messages:noOwnOcc()
    return 1025042
end

function Messages:tryStage()
    return 1025043
end

function Messages:msgselectRoleStartTimeHint(time, unit, ifHalf)
    return 1025050, IMessages:msgTimeTick(time, unit, ifHalf)
end

function Messages:msgselectRoleEndTimeHint(time, unit, ifHalf)
    return 1025051, IMessages:msgTimeTick(time, unit, ifHalf)
end

function Messages:msgHeroHasBeenChosen()
    return 1025052
end

function Messages:msgAddStayAtHomeEffect()
    return 1025053
end

function Messages:msgRemoveStayAtHomeEffect()
    return 1025054
end

--endregion
