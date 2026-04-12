AIConfig = {}
AIConfig.Config = {}

function AIConfig:init(config)
    for _, v in pairs(config) do
        local item = {}
        item.id = tostring(v.id)
        item.tacticsType = tostring(v.tacticsType)
        item.activityRange = tonumber(v.activityRange)
        item.followRange = tonumber(v.followRange)
        item.archeryRange = tonumber(v.archeryRange)
        item.meleeRange = tonumber(v.meleeRange)
        item.canDoMelee = (tonumber(v.canDoMelee) > 0)
        item.canDoRangedAttack = (tonumber(v.canDoRangedAttack) > 0)
        item.jumpCd = StringUtil.split(tostring(v.jumpCd), "#", false, true)
        item.attackCd = StringUtil.split(tostring(v.attackCd), "#", false, true)
        item.archeryCd = StringUtil.split(tostring(v.archeryCd), "#", false, true)
        item.archeryHitRate = tonumber(v.archeryHitRate)
        item.archeryDamage = tonumber(v.archeryDamage)
        item.boomCd = tonumber(v.boomCd)
        item.followSpeed = tonumber(v.followSpeed)
        item.archeryRangeLow = tonumber(v.archeryRangeLow)
        item.checkFollowRange = tonumber(v.checkFollowRange)
        item.followTime = tonumber(v.followTime)
        item.dangerDistance = tonumber(v.dangerDistance)
        item.BreakBlockRange = tonumber(v.BreakBlockRange)
        item.needBreakBlockNum = tonumber(v.needBreakBlockNum)
        item.idleTime = tonumber(v.idleTime)
        item.initIdleTime = tonumber(v.initIdleTime)
        item.resourceActiveRange = tonumber(v.resourceActiveRange)
        self.Config[item.id] = item
    end
end

function AIConfig:getConfigById(id)
    return self.Config[tostring(id)]
end