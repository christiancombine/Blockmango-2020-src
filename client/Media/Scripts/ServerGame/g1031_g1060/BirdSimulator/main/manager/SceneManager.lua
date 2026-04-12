SceneManager = {}
SceneManager.userNestIndex = {}
SceneManager.coin = {}
SceneManager.tree = {}
SceneManager.treeRewards = {}
SceneManager.releaseTime = {}

function SceneManager:deleteUserInfoByUid(userId)
    self:deleteUserNestIndex(userId)
end

function SceneManager:deleteReleaseTimeByUserId(userId)
    if self.releaseTime[tostring(userId)] then
        self.releaseTime[tostring(userId)] = nil
    end
end

function SceneManager:initUserNestIndex(userId, index)
    self.userNestIndex[tostring(userId)] = index
    self.releaseTime[tostring(userId)] = 90
end

function SceneManager:getUserNestIndexByUid(userId)
    if self.userNestIndex[tostring(userId)] ~= nil then
        return self.userNestIndex[tostring(userId)]
    end

    return 0
end

function SceneManager:deleteUserNestIndex(userId)
    if self.userNestIndex[tostring(userId)] ~= nil then
        self.userNestIndex[tostring(userId)] = nil
    end
end

function SceneManager:addCoinLevel(entityId, level)
    self.coin[tostring(entityId)] = level
end

function SceneManager:getCoinLevel(entityId)
    if self.coin[tostring(entityId)] ~= nil then
        return self.coin[tostring(entityId)]
    end

    return nil
end

function SceneManager:removeCoinLevel(entityId)
    if self.coin[tostring(entityId)] ~= nil then
        self.coin[tostring(entityId)] = nil
    end
end

function SceneManager:removeCoinLevelByKey(key)
    local str = StringUtil.split(key, ":")
    if str[2] == nil then
        return
    end

    local entityId = str[2]
    if self.coin[tostring(entityId)] ~= nil then
        self.coin[tostring(entityId)] = nil
    end
end

-- 只用于定时器(定时器目前不支持冒号方法)
function SceneManager.callBackAddTreeInScene()
    SceneManager:addTreeInScene()
end

function SceneManager:addTreeInScene()
    local level = FieldConfig:getGenerateTreeLevel()
    if level == 0 then
        return
    end

    local fieldId = FieldConfig:getGenerateFieldId(level)
    if fieldId == 0 then
        return
    end

    local treeId = TreeConfig:generateTreeId()
    if treeId == 0 then
        return
    end

    local treeInfo = TreeConfig:getTreeInfo(treeId)
    if treeInfo == nil then
        return
    end

    local fieldInfo = FieldConfig:getFieldInfo(fieldId)
    if fieldInfo == nil then
        return
    end

    local hp = treeInfo.basicPoint * fieldInfo.treeRatio
    hp = Tools:formatNumberThousands(hp)

    local entityId = EngineWorld:addSessionNpc(fieldInfo.treePos, fieldInfo.treeYaw, 104105, treeInfo.actor, function(entity)
        entity:setNameLang(hp)
        entity:setActorBody(treeInfo.body or "body")
        entity:setActorBodyId(treeInfo.bodyId1 or "")
        entity:setCanCollided(false)
    end)

    self.tree[tonumber(fieldId)] = {}
    self.tree[tonumber(fieldId)].entityId = entityId
    self.tree[tonumber(fieldId)].treeId = treeId
    self.tree[tonumber(fieldId)].hp = treeInfo.basicPoint * fieldInfo.treeRatio
    self.tree[tonumber(fieldId)].currentHp = treeInfo.basicPoint * fieldInfo.treeRatio
    self.tree[tonumber(fieldId)].remainTime = GameConfig.treeRemainTime
    self.tree[tonumber(fieldId)].curState = 1
    self.tree[tonumber(fieldId)].changeHp = false
    self.tree[tonumber(fieldId)].bodyId = treeInfo.bodyId1

    -- 提示大树生成
    MsgSender.sendCenterTips(3, Messages:treeAppearInField(tonumber(treeInfo.tipId), tonumber(fieldInfo.tipId)))
end

function SceneManager:changeTreeHp(fieldId, fruitNum)
    if self.tree[fieldId] == nil then
        return
    end

    self.tree[fieldId].currentHp = self.tree[fieldId].currentHp - fruitNum
    self.tree[fieldId].remainTime = GameConfig.treeRemainTime
    self.tree[fieldId].changeHp = true

    SceneManager:changeActorByTreeHp(fieldId)

    if self.tree[fieldId].currentHp <= 0 then
        local treeInfo = TreeConfig:getTreeInfo(self.tree[fieldId].treeId)
        local fieldInfo = FieldConfig:getFieldInfo(fieldId)

        if treeInfo ~= nil and fieldInfo ~= nil then
            self.treeRewards[fieldId] = {}
            self.treeRewards[fieldId].rewards = TreeConfig:getRewards(fieldId, self.tree[fieldId].treeId)
            self.treeRewards[fieldId].count = math.ceil(#self.treeRewards[fieldId].rewards / treeInfo.waveAmount)

            local num = treeInfo.waveAmount
            local count = self.treeRewards[fieldId].count - 1
            local waveInterval = treeInfo.waveInterval

            local players = PlayerManager:getPlayers()
            for i, player in pairs(players) do
                HostApi.sendPlaySound(player.rakssid, 319)
            end

            -- 为了避免定时器调用延时，先手动调用SceneManager.treeDropReward函数，再用定时器调用，并且把调用次数-1
            SceneManager:treeDropRewards(fieldId, num, fieldInfo.minPos, fieldInfo.maxPos)
            LuaTimer:scheduleTimer(SceneManager.callBackTreeDropRewards, waveInterval * 1000, count, fieldId, num, fieldInfo.minPos, fieldInfo.maxPos)
        end

        -- remove tree
        SceneManager:removeTree(fieldId)

    end

end

-- 只用于定时器(定时器目前不支持冒号方法)
function SceneManager.callBackTreeDropRewards(fieldId, num, pos1, pos2)
    SceneManager:treeDropRewards(fieldId, num, pos1, pos2)
end

function SceneManager:treeDropRewards(fieldId, num, pos1, pos2)
    if self.treeRewards[fieldId] == nil then
        return
    end

    local count = #self.treeRewards[fieldId].rewards
    if count >= num then
        count = num
    end

    local minX = math.min(pos1.x, pos2.x)
    local maxX = math.max(pos1.x, pos2.x)
    local minZ = math.min(pos1.z, pos2.z)
    local maxZ = math.max(pos1.z, pos2.z)

    for _ = 1, count do
        local x = HostApi.random("pos", minX + 1, maxX - 1)
        local y = pos1.y + 2
        local z = HostApi.random("pos", minZ + 1, maxZ - 1)
        local pos = VectorUtil.newVector3(x, y, z)

        if self.treeRewards[fieldId].rewards[1].itemId ~= nil then
            EngineWorld:addEntityItem(self.treeRewards[fieldId].rewards[1].itemId, 1, 0, 10, pos)
            table.remove(self.treeRewards[fieldId].rewards, 1)
        end
    end

    self.treeRewards[fieldId].count = self.treeRewards[fieldId].count - 1

    if self.treeRewards[fieldId].count <= 0 then
        self.treeRewards[fieldId] = nil
    end
end

function SceneManager:changeActorByTreeHp(fieldId)
    local treeInfo = TreeConfig:getTreeInfo(self.tree[fieldId].treeId)
    if treeInfo ~= nil then
        if self.tree[fieldId].currentHp < self.tree[fieldId].hp * 0.25 and self.tree[fieldId].curState < 4 then
            SceneManager:updateTreeModelAndReward(fieldId, 4)
            self.tree[fieldId].bodyId = treeInfo.bodyId4
        elseif self.tree[fieldId].currentHp < self.tree[fieldId].hp * 0.5 and self.tree[fieldId].curState < 3 then
            SceneManager:updateTreeModelAndReward(fieldId, 3)
            self.tree[fieldId].bodyId = treeInfo.bodyId3
        elseif self.tree[fieldId].currentHp < self.tree[fieldId].hp * 0.75 and self.tree[fieldId].curState < 2 then
            SceneManager:updateTreeModelAndReward(fieldId, 2)
            self.tree[fieldId].bodyId = treeInfo.bodyId2
        end
    end
end

function SceneManager:updateTreeModelAndReward(fieldId, state)
    self.tree[fieldId].curState = state
    local entityId = self.tree[fieldId].entityId
    local currentHp = self.tree[fieldId].currentHp
    local treeInfo = TreeConfig:getTreeInfo(self.tree[fieldId].treeId)

    local bodyId = treeInfo.bodyId1
    if state == 4 then
        bodyId = treeInfo.bodyId4
    elseif state == 3 then
        bodyId = treeInfo.bodyId3
    elseif state == 2 then
        bodyId = treeInfo.bodyId2
    end

    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:rewardByTreeScore(fieldId)
        currentHp = Tools:formatNumberThousands(currentHp)
        EngineWorld:getWorld():updateSessionNpc(entityId, player.rakssid, currentHp, treeInfo.actor, treeInfo.body, bodyId, "", "", 0, false)
    end
end

function SceneManager:removeTree(fieldId)
    if self.tree[tonumber(fieldId)] ~= nil then
        local entityId = self.tree[tonumber(fieldId)].entityId
        EngineWorld:removeEntity(entityId)
        self.tree[tonumber(fieldId)] = nil

        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            player:removeTreeScoreByFieldId(fieldId)
        end
    end
end

function SceneManager:isTreeInField(fieldId)
    if self.tree[tonumber(fieldId)] == nil then
        return false
    end

    return true
end

function SceneManager:onTick(ticks)
    for userId, index in pairs(self.userNestIndex) do
        if not self.releaseTime[tostring(userId)] then
            self.releaseTime[tostring(userId)] = 0
        end
        local player = PlayerManager:getPlayerByUserId(userId)
        if not player then
            self.releaseTime[tostring(userId)] = self.releaseTime[tostring(userId)] - 1
        else
            self.releaseTime[tostring(userId)] = 90
        end

        if self.releaseTime[tostring(userId)] < 0 then
            self:deleteUserInfoByUid(userId)
            self:deleteReleaseTimeByUserId(userId)
            HostApi.sendReleaseManor(userId)
            print("sendReleaseManor:", tostring(userId))
        end
    end
end