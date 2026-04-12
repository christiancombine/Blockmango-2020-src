
GamePlayerControl = class("GamePlayerControl", BaseBtTree)

GamePlayerControl.noBreakMap = {}
GamePlayerControl.printId = 0

local rakssidIndex = 0

function GamePlayerControl:ctor(platformId, name, teamId, config)
    rakssidIndex = rakssidIndex + 1
    if not name or name == "" then
        name = rakssidIndex
    end
    EngineWorld:addEntityPlayerAI(VectorUtil.ZERO, name, rakssidIndex, function(entity)
        self.entity = entity
        entity:sendLogin(platformId)
    end) --entityId
    self.rakssid = self.entity:getRaknetID()
    self.userId = platformId
    self.luaPlayer = PlayerManager:getPlayerByUserId(self.userId)
    self.luaPlayer:initDBDataFinished()
    self:updateConfig(config)
    self:setTeamId(teamId)
    self.entity:sendReady()

    ---初始化 工具
    self.swordId = ItemID.SWORD_WOOD
    self.axeId = ItemID.AXE_WOOD
    self.pickaxesId = ItemID.PICKAXE_WOOD
    self.shearsId = ItemID.SHEARS
    self.bowId = ItemID.BOW

    self.breakBlockTask = {}
    self.needBreakBlockList = {}

    self.activityRangePlayer = {}
    self.isLife = false
    self.tick = 0
    self.idleTick = 0
    self.oldPos = nil
    self.blockedTick = 0
    self.WaitDead = false

    if self.actionGroup then
        self.targetTreeId = self.actionGroup[1].defaultTree
        self.tickKey = LuaTimer:scheduleTimer(function()
            self:onTick()
        end, 1000)
    end
    AIManager:addAI(self)
end

function GamePlayerControl:updateConfig(config)
    self.config = config
    self.targetTreeId = nil
    self.actionGroup = TableUtil.copyTable(AIActionConfig:getActionGroup(config.Id))
end

function GamePlayerControl:onTick()
    self.watchId = tostring(0)
    if GameMatch.allowPvp then
        self.tick = self.tick + 1
    end
    self.watchTreeId = 0
    if tostring(self.rakssid) == self.watchId then
        self.watchTreeId = self.treeId
    end
    local curAction = self.actionGroup[1]
    local curIndex
    for index, action in pairs(self.actionGroup) do
        if action.startTick <= self.tick then
            curAction = action
            curIndex = index
        end
    end
    if self.curIndex ~= curIndex then
        self.curIndex = curIndex
        self.idleTick = 0
    end

    if self.idleTick >= curAction.idleTime then
        self.targetTreeId = curAction.idleTree
    else
        self.targetTreeId = curAction.defaultTree
    end

    local team = GameMatch.Teams[self.luaPlayer.teamId]
    if team and team.isHaveBed == false then
        self.targetTreeId = curAction.noBedTree
    end

    if self.curTreeId ~= self.targetTreeId then
        self:changeBtTree(self.targetTreeId)
    end

    if self.isMove then
        self:onCheckStuck() --检测是否卡住了
    end
end

function GamePlayerControl:onCheckStuck()

end

function GamePlayerControl:changeBtTree(treeId)
    local config = AIConfig:getConfigById(treeId)
    if not config then
        return
    end
    self:stopMove()
    self:removeBtTree()
    self.curTreeId = treeId
    self.aiConfig = config
    self.enemyId = nil
    self.targetResourcePos = nil
    self.needDefense = false
    self.isActivity = false
    self.archeryCd = false
    self.BoomCd = false
    self.navigateCd = false

    self.canDoMelee = self.aiConfig.canDoMelee
    self.canDoRangedAttack = self.aiConfig.canDoRangedAttack

    self.isInIdle = true
    self.needRushBed = false
    self.needActiveAttack = false
    self.needCollect = false
    self.needDefense = false
    self.breakBlockNum = 0
    self.idleTime = 0
    self.followTime = 0
    if self.initIdleTime == nil then
        self.initIdleTime = self.aiConfig.initIdleTime
    end

    self:initBtTree(self.aiConfig.tacticsType, 1)
end

function GamePlayerControl:setTeamId(teamId)
    self.luaPlayer.teamId = teamId
    self.luaPlayer.equip_dress = self.config.dressGroup or {}
end

function GamePlayerControl:onRespawn()
    if self.isLife or not GameMatch.hasStartGame then
        return
    end
    self.isLife = true
    self.idleTick = 0
    self.curPath = {}
    self.needMoveBlock = 0
    --give config items
    LuaTimer:schedule(function()
        self:refreshInv()
    end, 0)

    if self.curTreeId ~= self.targetTreeId then
        self:changeBtTree(self.targetTreeId)
    end
end

function GamePlayerControl:refreshInv()
    self.luaPlayer:clearInv()
    self.luaPlayer:initItem()
    for _, item in pairs(self.config.itemGroup or {}) do
        self.luaPlayer:tryAddItem(item.itemId, item.num)
    end
end

function GamePlayerControl:moveTo(targetPos)
    if not self.entity then
        return
    end
    ---检测目标格子有没有玩家
    LuaTimer:cancel(self.moveKey or 0)
    local target = VectorUtil.hashVector3(targetPos)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.realLife and player.entityPlayerMP.onGround == true and player.isMove == true and player.lastGroundPos and VectorUtil.hashVector3(player.lastGroundPos) == target then
            self.entity:stopMove()
            self.moveKey = LuaTimer:schedule(function()
                self:moveTo(targetPos)
            end, 1000)
            return
        end
    end
    local curPos = self.entity:getPosition()
    curPos = VectorUtil.newVector3(math.floor(curPos.x), math.floor(curPos.y), math.floor(curPos.z))
    --local blockId = EngineWorld:getBlockId(targetPos)
    --if blockId ~= targetPos.blockId then
    --    self:stopMove()
    --    return
    --end
    ---判断是否有方块阻挡. 破坏方块
    self:checkBreakBlock(targetPos)
    self:checkBreakBlock(VectorUtil.newVector3(targetPos.x, targetPos.y + 1, targetPos.z))
    self:checkBreakBlock(VectorUtil.newVector3(targetPos.x, targetPos.y + 2, targetPos.z))
    self:checkBreakBlock(curPos)
    self:checkBreakBlock(VectorUtil.newVector3(curPos.x, curPos.y + 1, curPos.z))
    self:checkBreakBlock(VectorUtil.newVector3(curPos.x, curPos.y + 2, curPos.z))
    if #self.breakBlockTask > 0 then
        self:breakBlock(self.breakBlockTask[1])
        return
    end

    ---如果是往上移动,则跳起来脚下搭方块,
    if self.entity.onGround and curPos.y + 1 < targetPos.y then
        local clothBlockId = self.luaPlayer:getClothBlockId()
        if self.luaPlayer:getInventory():getInventorySlotContainItem(clothBlockId) == -1 then
            self.luaPlayer:tryAddItem(clothBlockId, 64)
        end
        self.entity:stopMove()
        self.entity:setHandItemId(clothBlockId)
        self:jump()
        LuaTimer:schedule(function()
            self.entity:placeBlock(curPos)

            LuaTimer:schedule(function()
                self.entity:moveToPos(curPos)
            end, 1500)
        end, 300)
        return
    end

    ---判断目标脚下是否为空气. 然后放置方块
    local targetFloorPos = VectorUtil.newVector3(targetPos.x, targetPos.y - 1, targetPos.z)
    local floorBlockId = EngineWorld:getBlockId(targetFloorPos)
    if floorBlockId == 0 then
        self.entity:stopMove()
        local clothBlockId = self.luaPlayer:getClothBlockId()
        if self.luaPlayer:getInventory():getInventorySlotContainItem(clothBlockId) == -1 then
            self.luaPlayer:tryAddItem(clothBlockId, 64)
        end
        self.entity:setHandItemId(clothBlockId)
        self.entity:startSneak()
        LuaTimer:schedule(function()
            local placeSuccess = self.entity:placeBlock(targetFloorPos)
            if placeSuccess then
                self.entity:moveToPos(targetPos)
            else
                self:stopMove()
            end
        end, 200)
        return
    end

    ---取到目标方块的中心,移动到中心位置
    self.entity:moveToPos(targetPos)
end

function GamePlayerControl:checkBreakBlock(pos)
    if GamePlayerControl.noBreakMap[VectorUtil.hashVector3(pos)] then
        return
    end
    local blockId = EngineWorld:getBlockId(pos)
    if BtUtil.canBreakBlock(blockId) then
        table.insert(self.breakBlockTask, pos)
    end
end

function GamePlayerControl:breakBlock(pos)
    self.entity:stopMove()
    local blockId = EngineWorld:getBlockId(pos)
    blockId = ItemMapping:getOldItemId(ItemMapping:getOldItemId(blockId))
    if blockId == BlockID.CLOTH then
        self.entity:setHandItemId(self.shearsId)
    elseif blockId == BlockID.WHITE_STONE or blockId == BlockID.STAIND_GLASS or blockId == BlockID.GLASS then
        self.entity:setHandItemId(self.pickaxesId)
    elseif blockId == BlockID.PLANKS then
        self.entity:setHandItemId(self.axeId)
    elseif blockId == BlockID.OBSIDIAN then
        self.entity:setHandItemId(self.luaPlayer:tryItemId2DressId(278))
    elseif blockId == BlockID.BED then
        self.entity:setHandItemId(self.axeId)
    end
    self.entity:breakBlock(pos)
end

function GamePlayerControl:setPlayerRotation(yaw, pitch)
    self.entity:sendPlayerRotationChanged(yaw, pitch)

end

function GamePlayerControl:jump()
    self.entity:sendJumpChanged(true)
    LuaTimer:schedule(function()
        self.entity:sendJumpChanged(false)
    end, 100)
end

function GamePlayerControl:attack(target)
    ---近战攻击
    self.entity:setHandItemId(self.swordId)

    if AIManager:isAI(target.rakssid) then
        self.entity:attackEntity(target:getEntityId(), true)
    else
        self:abortBreakBlock()
        if self.attackLock then
            self.entity:attackEntity(target:getEntityId(), false)
        else
            self.entity:attackEntity(target:getEntityId(), true)
            self.attackLock = true
            local cd = math.random(self.aiConfig.attackCd[1], self.aiConfig.attackCd[2])
            LuaTimer:schedule(function()
                self.attackLock = false
            end, 1000 * cd)
        end
    end
end

function GamePlayerControl:findPath(targetPos)
    self.targetPos = VectorUtil.newVector3(math.floor(targetPos.x), math.floor(targetPos.y), math.floor(targetPos.z))
    if #self.curPath == 0 then
        self.needMoveBlock = 0
        self:updatePath()
    end
    if false and tostring(self.rakssid) == tostring(0) then
        for _, pos in pairs(self.curPath or {}) do
            EngineWorld:addSimpleEffect("141_red2.effect", VectorUtil.newVector3(pos.x + 0.5, pos.y, pos.z + 0.5), 0, 1000, 0, 0.5)
        end
        EngineWorld:addSimpleEffect("141_red2.effect", VectorUtil.newVector3(self.targetPos.x + 0.5, self.targetPos.y, self.targetPos.z + 0.5), 0, 1000, 0, 2)
    end
    return true
end

function GamePlayerControl:updatePath()
    ---移动到指定坐标
    if self.entity.onGround == false and not self.entity:isInWater() then
        return self.curPath
    end
    local targetPos = self.targetPos
    local entityPos = self.entity:getPosition()
    self.pos = VectorUtil.newVector3(math.floor(entityPos.x), math.floor(entityPos.y), math.floor(entityPos.z))

    if self.needMoveBlock == 0 then

        self.curPath = BtUtil.getCurPath(self.pos, targetPos)
        table.remove(self.curPath, #self.curPath)
        self.needMoveBlock = math.ceil(#self.curPath / 2)
    end

    if not self.isMove and #self.curPath > 0 then
        ---从停止到开始移动
        self.isMove = true
        self.luaPlayer.isMove = true
        self.breakBlockTask = {}
        self:moveTo(self.curPath[#self.curPath])
        return self.curPath
    end
    return self.curPath
end

function GamePlayerControl:stopMove()
    LuaTimer:cancel(self.moveKey or 0)
    self.entity:stopMove()
    self.curPath = {}
    self.isMove = false
    self.luaPlayer.isMove = false
end

function GamePlayerControl:changePatrol()
    if self.changePatrolTimer == nil then
        local x = self.luaPlayer.team.initPos.x + math.random(-2, 2)
        local z = self.luaPlayer.team.initPos.z + math.random(-2, 2)
        local patrolPos = VectorUtil.newVector3(x, self.luaPlayer.team.initPos.y, z)
        if BtUtil.checkBlockCanMove(patrolPos) then
            self.patrolPos = patrolPos
        end

        self.entity:faceToEntity(0)
        self:stopMove()
        self.changePatrolTimer = LuaTimer:schedule(function()
            self.changePatrolTimer = nil
        end, 5000)
    end
end

function GamePlayerControl:onHurt()
    self:abortBreakBlock()
end

function GamePlayerControl:abortBreakBlock()
    if #self.breakBlockTask > 0 then
        self:stopMove()
        self.entity:abortBreakBlock()
    end
end

function GamePlayerControl:logout()
    LuaTimer:schedule(function()
        self.entity:sendLogout()
        self.entity = nil
    end, 5000)
    LuaTimer:cancel(self.tickKey or 0)
    self:removeBtTree()
    self:stopMove()
    AIManager:subAI(self)
end

function GamePlayerControl:onLogout()
    self.entity = nil
    LuaTimer:cancel(self.tickKey or 0)
    LuaTimer:cancel(self.moveKey or 0)
    self:removeBtTree()
end