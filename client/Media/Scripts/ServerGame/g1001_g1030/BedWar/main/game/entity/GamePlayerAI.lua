
GamePlayerAI = class("GamePlayerAI", GamePlayerControl)

function GamePlayerAI:onCheckStuck()
    if not self.entity then
        return
    end
    if not GameMatch:isGameStart() then
        return
    end
    if self:isInHome() == Define.BtStatus.SUCCESS then
        self.lastCheckStuckTime = 0
        return
    end
    if self:checkMeleeRange() == Define.BtStatus.SUCCESS then
        self.lastCheckStuckTime = 0
        return
    end
    if self:checkRangedAttackRange() == Define.BtStatus.SUCCESS then
        self.lastCheckStuckTime = 0
        return
    end
    local position = self.entity:getPosition()
    self.lastCheckStuckTime = self.lastCheckStuckTime or 0
    self.lastCheckStuckPos = self.lastCheckStuckPos or VectorUtil.ZERO
    if VectorUtil.distance(position, self.lastCheckStuckPos) < GameConfig.checkAIStuckDistance then
        self.lastCheckStuckTime = self.lastCheckStuckTime + 1
    else
        self.lastCheckStuckTime = 0
        self.lastCheckStuckPos = position
    end
    if self.lastCheckStuckTime >= GameConfig.checkAIStuckTime then
        self.luaPlayer:subHealth(10000)
        LuaTimer:cancel(self.moveKey)
    end
end

function GamePlayerAI:isInHome()
    ---是否在家（出生点）
    if self.initPos then
        if BtUtil.calculateDistance(self.initPos, self.entity:getPosition()) < 5 then
            return Define.BtStatus.SUCCESS
        end
        return Define.BtStatus.FAILURE
    end
    return Define.BtStatus.FAILURE
end

function GamePlayerAI:followEnemy()
    LogUtil.log(" GamePlayerAI:followEnemy", LogUtil.LogLevel.Debug)
    ---追击敌人
    local target = PlayerManager:getPlayerByUserId(self.enemyId)
    if target == nil or target.lastGroundPos == nil then
        self.enemyId = nil
        self:stopMove()
        return Define.BtStatus.FAILURE
    end
    local targetPos = target.lastGroundPos

    local distance = BtUtil.calculateDistance(targetPos, self.entity:getPosition())

    if self.needActiveAttack == false and self.needDefense == false then
        return Define.BtStatus.FAILURE
    end

    if self.needActiveAttack == true and distance > self.aiConfig.meleeRange and distance < self.aiConfig.checkFollowRange then
        self.followTime = self.followTime + Define.Time.AIRefresh
        if self.followTime >= self.aiConfig.followTime then
            self.enemyId = nil
        end
    else
        self.followTime = 0
    end

    if targetPos and self:findPath(targetPos) then
        return Define.BtStatus.SUCCESS
    end
    return Define.BtStatus.FAILURE
end

function GamePlayerAI:goHome()
    LogUtil.log(" GamePlayerAI:goHome", LogUtil.LogLevel.Debug)
    ---设置出生点为移动目标点并移动
    if self.luaPlayer.team ~= nil then
        self.initPos = self.luaPlayer.team.initPos
        self.enemyId = nil
        self:findPath(self.initPos)
        if BtUtil.calculateDistance(self.initPos, self.entity:getPosition()) < 10 then
            self.isInIdle = true
        end
        return Define.BtStatus.SUCCESS
    end
    return Define.BtStatus.FAILURE
end

function GamePlayerAI:getEnemy()
    LogUtil.log(" GamePlayerAI:getEnemy", LogUtil.LogLevel.Debug)
    ---将接近我方床的敌人设置为攻击目标

    if self.enemyId ~= nil then
        local player = PlayerManager:getPlayerByUserId(self.enemyId)
        if player and player.lifeStatus == true and player.entityPlayerMP ~= nil and player.entityPlayerMP:isPotionActive(14) == true then
            self.enemyId = nil
        end
    end

    local dangerEnemy = BlackBoard:getValue(tostring(self.luaPlayer.teamId) .. "dangerEnemy")
    if dangerEnemy == nil then
        self.enemyId = nil
        return Define.BtStatus.FAILURE
    end

    local minDistancePlayer = PlayerManager:getPlayerByUserId(dangerEnemy.userId)
    if minDistancePlayer == nil then
        return Define.BtStatus.FAILURE
    end

    if self.enemyId ~= nil and PlayerManager:getPlayerByUserId(self.enemyId) ~= nil then
        local nowDistance = BtUtil.calculateDistance(PlayerManager:getPlayerByUserId(self.enemyId):getPosition(), self.luaPlayer.team.bedPos[1])
        local minDistance = BtUtil.calculateDistance(minDistancePlayer:getPosition(), self.luaPlayer.team.bedPos[1])
        if minDistance < nowDistance * self.aiConfig.dangerDistance then
            self.enemyId = dangerEnemy.userId
        end
    else
        self.enemyId = dangerEnemy.userId
    end

    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:selectPlayer()
    LogUtil.log(" GamePlayerAI:selectPlayer", LogUtil.LogLevel.Debug)
    ---选择目标敌人
    local minDis = 10000
    local minDisEnemyId
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player ~= nil and player.entityPlayerMP ~= nil and player.entityPlayerMP:isPotionActive(14) == false and player.teamId == self.targetTeamId then
            if player.lifeStatus == true then
                local distance = BtUtil.calculateDistance(self.entity:getPosition(), player:getPosition())
                if distance < minDis then
                    minDisEnemyId = player.userId
                    minDis = distance
                end
            end
        end
    end

    self.enemyId = minDisEnemyId
    if self.enemyId ~= nil then
        self.needActiveAttack = true
        return Define.BtStatus.SUCCESS
    end
    return Define.BtStatus.FAILURE
end

function GamePlayerAI:checkMeleeRange()
    ---检查敌人是否在近战攻击范围内
    local enemy = PlayerManager:getPlayerByUserId(self.enemyId)
    if enemy == nil or BtUtil.calculateDistance(enemy:getPosition(), self.entity:getPosition()) > self.aiConfig.meleeRange then
        return Define.BtStatus.FAILURE
    end

    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:checkRangedAttackRange()
    ---检查敌人是否在远程攻击范围内
    ---远程攻击AI的远程攻击范围为一个实心圆
    ---混合型攻击AI的远程攻击范围为一个同心圆
    local enemy = PlayerManager:getPlayerByUserId(self.enemyId)
    if enemy == nil then
        return Define.BtStatus.FAILURE
    end

    local dis = BtUtil.calculateDistance(enemy:getPosition(), self.entity:getPosition())

    if enemy.lifeStatus == true and dis < self.aiConfig.archeryRange then
        if self.canDoMelee == true and dis < self.aiConfig.archeryRangeLow then
            return Define.BtStatus.FAILURE
        end
        return Define.BtStatus.SUCCESS
    end
    return Define.BtStatus.FAILURE
end

function GamePlayerAI:attackPlayer()
    ---近战攻击敌人
    local target = PlayerManager:getPlayerByUserId(self.enemyId)
    if target == nil then
        return Define.BtStatus.FAILURE
    end

    self:attack(target)
    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:checkHaveWall()
    ---检查与敌人见是否有墙
    local enemy = PlayerManager:getPlayerByUserId(self.enemyId)
    if enemy == nil then
        return Define.BtStatus.FAILURE
    end

    local result = EngineWorld:getWorld():rayTraceBlocks(VectorUtil.add3(self.entity:getPosition(), VectorUtil.newVector3(0, 1.62, 0)),
            VectorUtil.add3(enemy:getPosition(), VectorUtil.newVector3(0, 1.62, 0)), true, true)
    if result.result == true then
        return Define.BtStatus.SUCCESS
    end
    return Define.BtStatus.FAILURE
end

function GamePlayerAI:checkWallCanBreak()
    ---检查与敌人间的墙壁是否可破坏
    local enemy = PlayerManager:getPlayerByUserId(self.enemyId)
    if enemy == nil then
        return Define.BtStatus.FAILURE
    end

    local result = EngineWorld:getWorld():rayTraceBlocks(self.entity:getPosition(), enemy:getPosition(), true, true)
    if result.result == true then
        local blockId = EngineWorld:getWorld():getBlockId(result.blockPos)
        if blockId == BlockID.PLANKS or blockId == BlockID.CLOTH or blockId == BlockID.GLASS then
            return Define.BtStatus.SUCCESS
        end
    end

    return Define.BtStatus.FAILURE
end

function GamePlayerAI:readyArchery()
    ---射箭是否冷却好了
    if self.archeryCd == false then
        self.entity:setHandItemId(self.bowId)
        self:stopMove()
        return Define.BtStatus.SUCCESS
    end

    return Define.BtStatus.FAILURE
end

function GamePlayerAI:archery()
    ---射箭
    if self.archeryCd == false then
        local targetEntityId = 0
        local target = PlayerManager:getPlayerByUserId(self.enemyId)
        if target ~= nil then
            targetEntityId = target:getEntityId()
        end
        local randomValue = math.random(1, 1000)
        local isHit = false
        if randomValue < self.aiConfig.archeryHitRate * 1000 then
            isHit = true
        end
        self:abortBreakBlock()
        self.entity:startShootArrow(targetEntityId, isHit)

        local cd = math.random(self.aiConfig.archeryCd[1], self.aiConfig.archeryCd[2])
        self.archeryCd = true
        LuaTimer:schedule(function()
            self.archeryCd = false
        end, cd * 1000)
        return Define.BtStatus.SUCCESS
    end

    return Define.BtStatus.FAILURE
end

function GamePlayerAI:readyFlareBomb()
    ---火焰弹是否冷却好了
    if self.BoomCd == false then
        self.entity:setHandItemId(506)
        self:stopMove()
        return Define.BtStatus.SUCCESS
    end

    return Define.BtStatus.FAILURE
end

function GamePlayerAI:useFlareBomb()
    ---使用火焰弹
    local skill = SkillConfig:getSkill(506)
    if skill ~= nil then
        self:stopMove()
        self.entity:relaseItemSkill(506)
    end

    self.BoomCd = true

    LuaTimer:schedule(function()
        self.BoomCd = false
    end, self.aiConfig.boomCd * 1000)

    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:onDie()
    self:stopMove()
    self.enemyId = nil

    self.needDefense = false
    self.isActivity = false
    self.archeryCd = false
    self.BoomCd = false
    self.navigateCd = false
    self.isInIdle = true
    self.needRushBed = false
    self.needActiveAttack = false
    self.needCollect = false
    self.needDefense = false

    self.breakBlockNum = 0
    self.idleTime = 0

    self:cancelResourcePointOwn()
    LuaTimer:cancel(self.collectTime)
    self.collectTime = nil

    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:idle()
    LogUtil.log(" GamePlayerAI:idle", LogUtil.LogLevel.Debug)
    if self.initIdleTime > 0 then
        self.initIdleTime = self.initIdleTime - 1
    else
        self.idleTime = self.idleTime + 1
        self.idleTick = self.idleTick + 1
        if self.curTreeId ~= self.targetTreeId then
            self:changeBtTree(self.targetTreeId)
            return Define.BtStatus.SUCCESS
        end

        if self.idleTime ~= -1 and self.idleTime > self.aiConfig.idleTime then
            self.idleTime = 0
            self.isInIdle = false
            return Define.BtStatus.FAILURE
        end
    end

    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:randomPatrol()
    self:changePatrol()
    if self.patrolPos ~= nil then
        self:findPath(self.patrolPos)
    end
    return Define.BtStatus.SUCCESS
end

------------------------------------------------------------------------------------------

function GamePlayerAI:selectBed()
    LogUtil.log(" GamePlayerAI:selectBed", LogUtil.LogLevel.Debug)
    ---选择要破坏的床
    self.targetTeam = nil
    for _, team in pairs(GameMatch.Teams) do
        if tonumber(team.id) == tonumber(self.targetTeamId) and team.isHaveBed then
            self.targetTeam = team
            break
        end
    end

    if self.targetTeam == nil then
        local nearBedDistance
        for _, team in pairs(GameMatch.Teams) do
            if tonumber(team.id) ~= tonumber(self.luaPlayer.teamId) and team.isHaveBed then
                local distance = BtUtil.calculateDistance(self.entity:getPosition(), team.bedPos[1])
                if nearBedDistance == nil or nearBedDistance > distance then
                    self.targetTeam = team
                    nearBedDistance = distance
                end
            end
        end
    end

    if self.targetTeam == nil then
        self.needRushBed = false
        return Define.BtStatus.FAILURE
    end

    self.needRushBed = true
    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:targetNotBreak()
    LogUtil.log("GamePlayerAI:targetNotBreak", LogUtil.LogLevel.Debug)
    ---目标床是否已被破坏
    if self.targetTeam ~= nil then
        if self.targetTeam.isHaveBed == true then
            self.needRushBed = true
            return Define.BtStatus.SUCCESS
        end
    end
    self.needRushBed = false
    return Define.BtStatus.FAILURE
end

function GamePlayerAI:inRushRange()
    LogUtil.log("GamePlayerAI:inRushRange", LogUtil.LogLevel.Debug)
    ---是否在rush范围内
    if self.targetTeam ~= nil then
        if BtUtil.calculateDistance(self.entity:getPosition(), self.targetTeam.bedPos[1]) < self.aiConfig.BreakBlockRange then
            return Define.BtStatus.SUCCESS
        end
    end
    return Define.BtStatus.FAILURE
end

function GamePlayerAI:goRushBed()
    LogUtil.log(" GamePlayerAI:goRushBed", LogUtil.LogLevel.Debug)
    ---设置敌人床为移动目标点并移动
    if self.luaPlayer.team ~= nil then
        self.enemyId = nil
        self:findPath(self.targetTeam.bedPos[1])
        self.breakBlockNum = 0
    end
    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:needBreakBlock()
    LogUtil.log(" GamePlayerAI:needBreakBlock", LogUtil.LogLevel.Debug)
    ---没有防御工事，或对手是Ai的情况下可以直接去拆床
    if self.breakBlockNum > self.aiConfig.needBreakBlockNum or #self.needBreakBlockList == 0 or #AIManager:getAIsByTeamId(self.targetTeam.id) ~= 0 then
        return Define.BtStatus.FAILURE
    end
    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:selectBlockList()
    LogUtil.log(" GamePlayerAI:selectBlockList", LogUtil.LogLevel.Debug)
    self.needBreakBlockList = {}
    for x = self.targetTeam.bedPos[1].x - 3, self.targetTeam.bedPos[1].x + 3 do
        for z = self.targetTeam.bedPos[1].z - 3, self.targetTeam.bedPos[1].z + 3 do
            for y = self.targetTeam.bedPos[1].y + 1, self.targetTeam.bedPos[1].y, -1 do
                local blockPos = VectorUtil.newVector3i(x, y, z)
                local blockId = EngineWorld:getBlockId(blockPos)
                if (blockId == BlockID.CLOTH or blockId == BlockID.WHITE_STONE or blockId == BlockID.STAIND_GLASS or blockId == BlockID.PLANKS or blockId == BlockID.OBSIDIAN)
                        and not GamePlayerControl.noBreakMap[VectorUtil.hashVector3(blockPos)]
                then
                    table.insert(self.needBreakBlockList, blockPos)
                end

            end
        end
    end
    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:moveToNextPos()
    LogUtil.log(" GamePlayerAI:moveToNextPos", LogUtil.LogLevel.Debug)
    if self.needBreakBlockList[1] ~= nil then
        self:findPath(self.needBreakBlockList[1])
    end
    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:moveToTargetBed()
    LogUtil.log(" GamePlayerAI:moveToTargetBed", LogUtil.LogLevel.Debug)
    if self.targetTeam ~= nil then
        if BtUtil.calculateDistance(self.entity:getPosition(), self.targetTeam.bedPos[1]) < 1.5 then
            self:breakBlock(self.targetTeam.bedPos[1])
            if EngineWorld:getBlockId(self.targetTeam.bedPos[1]) ~= BlockID.BED then
                self.needRushBed = false
            end
            return Define.BtStatus.SUCCESS
        end
        self:findPath(self.targetTeam.bedPos[1])
    end
    return Define.BtStatus.SUCCESS
end

-------------------------------------------------------------------------------------------------

function GamePlayerAI:selectPoint()
    LogUtil.log(" GamePlayerAI:selectPoint", LogUtil.LogLevel.Debug)

    local resourcePoint = BlackBoard:getValue(tostring(self.luaPlayer.teamId) .. "resourcePoint")

    while self.targetResourcePoint == nil do
        local i = math.random(1, #resourcePoint)
        if resourcePoint[i].haveAI == false then
            resourcePoint[i].haveAI = true
            self.targetResourcePoint = TableUtil.copyTable(resourcePoint[i])
            BlackBoard:setValue(tostring(self.luaPlayer.teamId) .. "resourcePoint", resourcePoint)
            self.needCollect = true
        end
    end
    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:moveToResource()
    LogUtil.log(" GamePlayerAI:moveToResource", LogUtil.LogLevel.Debug)
    self:findPath(self.targetResourcePoint.pos)

    if self.collectTime == nil and BtUtil.calculateDistance(self.entity:getPosition(), self.targetResourcePoint.pos) < 3 then
        self.collectTime = LuaTimer:schedule(function()
            self:cancelResourcePointOwn()
            self.collectTime = nil
        end, math.random(30, 50) * 1000)
    end

    return Define.BtStatus.SUCCESS
end

function GamePlayerAI:havePlayerInMeleeRange()
    local players = PlayerManager:getPlayers()
    local selfPos = self.entity:getPosition()
    self.closestEnemy = nil
    for _, player in pairs(players) do
        if player.lifeStatus == true and player.entityPlayerMP ~= nil and player.entityPlayerMP:isPotionActive(14) == false and player.teamId ~= self.luaPlayer.teamId then
            local distance = BtUtil.calculateDistance(selfPos, player:getPosition())
            if distance < self.aiConfig.meleeRange then
                if self.closestEnemy == nil then
                    self.closestEnemy = { userId = player.userId, minDistance = distance }
                elseif distance < self.closestEnemy.minDistance then
                    self.closestEnemy = { userId = player.userId, minDistance = distance }
                end
            end
        end
    end
    if self.closestEnemy ~= nil then
        return Define.BtStatus.SUCCESS
    end
    return Define.BtStatus.FAILURE
end

function GamePlayerAI:attackClose()
    ---攻击离自己最近的玩家
    local target = PlayerManager:getPlayerByUserId(self.closestEnemy.userId)
    if target ~= nil then
        self:stopMove()
        self.entity:faceToEntity(target:getEntityId())
        self:attack(target)
        return Define.BtStatus.SUCCESS
    end

    return Define.BtStatus.FAILURE
end

function GamePlayerAI:cancelResourcePointOwn()
    local resourcePoint = BlackBoard:getValue(tostring(self.luaPlayer.teamId) .. "resourcePoint")
    if resourcePoint ~= nil and self.targetResourcePoint ~= nil then
        for _, point in pairs(resourcePoint) do
            if VectorUtil.equal(point.pos, self.targetResourcePoint.pos) == true then
                point.haveAI = false
            end
        end
        BlackBoard:setValue(tostring(self.luaPlayer.teamId) .. "resourcePoint", resourcePoint)
    end
    self.targetResourcePoint = nil
end

return GamePlayerAI