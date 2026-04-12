GameMonster = class()

function GameMonster:ctor(userId)
    self.userId = userId
    self.monsters = {}
    self.countDown = {}
end

function GameMonster:initDataFromDB(data)
    self:initCountDownFromDB(data.CD or {})
    self:showMonsterCD()
end

function GameMonster:initCountDownFromDB(data)
    for _, v in pairs(data) do
        self:setCountDown(v.sign, v.time)
    end
end

function GameMonster:prepareDataSaveToDB()
    local data = {
        CD = self:prepareCountDownToDB()
    }
    return data
end

function GameMonster:prepareCountDownToDB()
    local data = {}
    for i, v in pairs(self.countDown) do
        local item = {}
        item.sign = tostring(i)
        item.time = v.time
        table.insert(data, item)
    end

    return data
end

function GameMonster:generateMonster(vec3)
    local fieldInfo = FieldConfig:getFieldInfoByPos(vec3)
    if fieldInfo == nil then
        return
    end

    if #fieldInfo.monsterIds ~= #fieldInfo.monsterPos then
        return
    end

    for i, v in pairs(fieldInfo.monsterPos) do
        local pos = StringUtil.split(v, ",")
        if pos[1] == nil or pos[2] == nil or pos[3] == nil then
            break
        end

        local monsterInfo = MonsterConfig:getMonsterInfoById(fieldInfo.monsterIds[i])
        if monsterInfo == nil then
            break
        end

        local sign = pos[1] .. ":" .. pos[2] .. ":" .. pos[3]
        local monster = self:getMonsterBySign(sign)
        if monster ~= nil then
            -- monster already in the world. do nothing...
            break
        end

        local monsterPos = VectorUtil.newVector3(pos[1], pos[2], pos[3])
        local countDown = self:getCountDown(sign)
        if countDown == nil then
            self:addMonsterToWorld(sign, fieldInfo.monsterIds[i], monsterPos, monsterInfo, fieldInfo.minPos, fieldInfo.maxPos)
        else
            local cd = monsterInfo.refreshCD
            if os.time() - countDown.time >= cd then
                self:addMonsterToWorld(sign, fieldInfo.monsterIds[i], monsterPos, monsterInfo, fieldInfo.minPos, fieldInfo.maxPos)
            end
        end
    end

end

function GameMonster:addMonsterToWorld(sign, monsterId, monsterPos, monsterInfo, minPos, maxPos)
    local entityId = EngineWorld:addCreatureNpc(monsterPos, monsterId, 0, monsterInfo.actorName, function(entity)
        entity:setNameLang(monsterInfo.name or "")
        entity:setUserId(self.userId)
    end)
    local homePos = VectorUtil.newVector3i(monsterPos.x, monsterPos.y, monsterPos.z)
    EngineWorld:getWorld():setCreatureHome(entityId, homePos)
    self:addMonster(sign, entityId, monsterId, monsterInfo, minPos, maxPos)
    EngineWorld:getWorld():setEntityHealth(entityId, tonumber(monsterInfo.hp), tonumber(monsterInfo.hp))
end

function GameMonster:showMonsterCD()
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end

    for key, value in pairs(self.countDown) do
        local pos = StringUtil.split(key, ":")
        if pos[1] == nil or pos[2] == nil or pos[3] == nil then
            break
        end

        local vec3 = VectorUtil.newVector3(pos[1], pos[2], pos[3])
        local fieldInfo = FieldConfig:getFieldInfoByPos(vec3)
        if fieldInfo ~= nil then
            for i, v in pairs(fieldInfo.monsterPos) do
                local bornPos = StringUtil.split(v, ",")
                if bornPos[1] == nil or bornPos[2] == nil or bornPos[3] == nil then
                    break
                end

                if pos[1] == bornPos[1] and pos[2] == bornPos[2] and pos[3] == bornPos[3] then
                    local monsterInfo = MonsterConfig:getMonsterInfoById(fieldInfo.monsterIds[i])
                    if monsterInfo ~= nil then
                        local remainTime = monsterInfo.refreshCD - (os.time() - value.time)
                        if remainTime > 0 then
                            local countDownPos = StringUtil.split(fieldInfo.countDownPos[i], ",")
                            local textPos = VectorUtil.newVector3(countDownPos[1], countDownPos[2], countDownPos[3])
                            HostApi.deleteWallText(player.rakssid, textPos)
                            HostApi.addWallText(player.rakssid, 1, remainTime * 1000, monsterInfo.name, textPos, 3, 90, 0, { 1, 1, 1, 1 })
                        end
                    end
                end

            end
        end
    end
end

function GameMonster:setMonsterLostTarget()
    for i, v in pairs(self.monsters) do
        v.isLostTarget = true
    end
end

function GameMonster:removeMonster()
    for i, v in pairs(self.monsters) do
        self:removeMonsterByEntityId(v.entityId)
    end
end

function GameMonster:removeMonsterByEntityId(entityId)
    local monster = self:getMonsterByEntityId(entityId)
    if monster ~= nil then
        self.monsters[monster.sign] = nil
    end
    EngineWorld:removeEntity(entityId)
end

function GameMonster:getMonsterBySign(sign)
    if self.monsters[sign] ~= nil then
        return self.monsters[sign]
    end

    return nil
end

function GameMonster:getMonsterByEntityId(entityId)
    for i, v in pairs(self.monsters) do
        if v.entityId == entityId then
            return v
        end
    end

    return nil
end

function GameMonster:addMonster(sign, entityId, monsterId, monsterInfo, minPos, maxPos)
    if entityId ~= 0 then
        self.monsters[sign] = {}
        self.monsters[sign].sign = sign
        self.monsters[sign].entityId = tonumber(entityId)
        self.monsters[sign].monsterId = tonumber(monsterId)
        self.monsters[sign].hp = tonumber(monsterInfo.hp)
        self.monsters[sign].maxHp = tonumber(monsterInfo.hp)
        self.monsters[sign].attack = tonumber(monsterInfo.attack)
        self.monsters[sign].attackCD = tonumber(monsterInfo.attackCD)
        self.monsters[sign].moveSpeed = tonumber(monsterInfo.moveSpeed)
        self.monsters[sign].minPos = minPos
        self.monsters[sign].maxPos = maxPos
        self.monsters[sign].userId = self.userId
        self.monsters[sign].isLostTarget = false
    end
end

function GameMonster:setMonsterDead(entityId)
    local monster = self:getMonsterByEntityId(entityId)
    if monster ~= nil then
        self.monsters[monster.sign] = nil
        EngineWorld:getWorld():killCreature(entityId)
        self:setCountDown(monster.sign, os.time())
        local player = PlayerManager:getPlayerByUserId(self.userId)
        if player ~= nil then
            -- reward
            local monsterInfo = MonsterConfig:getMonsterInfoById(monster.monsterId)
            if monsterInfo ~= nil then
                local exp = monsterInfo.exp
                if player.userBird ~= nil then
                    for _, v in pairs(player.userBird.birds) do
                        -- add exp
                        if v.entityId ~= 0 then
                            player.userBird:addExp(v.id, exp)
                        end
                    end
                end

                -- reward
                for i, itemId in pairs(monsterInfo.reward) do
                    local num = monsterInfo.num[i]
                    if tonumber(itemId) > 0 and tonumber(num) > 0 then
                        player:addProp(tonumber(itemId), tonumber(num))
                    end
                end

                -- rewardItems
                local birdGains = {}
                for _, v in pairs(monsterInfo.rewardItems) do
                    local items = StringUtil.split(v, ":")
                    if items[1] ~= nil and items[2] ~= nil and items[3] ~= nil then
                        local itemId = tonumber(items[1])
                        local num = tonumber(items[2])
                        local weight = tonumber(items[3])

                        if itemId > 0 and num > 0 then
                            local randNum = HostApi.random("rewardItems", 1, 100)
                            if randNum <= weight then
                                player:addProp(itemId, num)
                                local itemInfo = BirdConfig:getGiftInfoByItemId(itemId)
                                if itemInfo ~= nil then
                                    local birdGain = BirdGain.new()
                                    birdGain.itemId = itemId
                                    birdGain.num = num
                                    birdGain.icon = itemInfo.icon
                                    birdGain.name = itemInfo.name
                                    table.insert(birdGains, birdGain)
                                end
                            end
                        end
                    end
                end
                if next(birdGains) then
                    HostApi.sendBirdGain(player.rakssid, birdGains)
                end

                -- add rankFightPoint to redis
                player.rankFightPoint = player.rankFightPoint + monsterInfo.fightPoint
                MsgSender.sendRightTipsToTarget(player.rakssid, 5, Messages:getFightPoint(monsterInfo.fightPoint))
            end

            -- show countDown
            self:showMonsterCD()

            -- task
            TaskHelper.dispatch(TaskType.BeaMonsterTaskEvent, player.userId, monster.monsterId, 1)
        end
    end
end

function GameMonster:attackPlayer(entityId, player)
    for i, v in pairs(self.monsters) do
        if v.entityId == entityId then
            player.isInBattle = true
            local name = "playerHit:" .. tostring(player.userId)
            TimerManager.RemoveListenerById(name)
            TimerManager.AddDelayListener(name, GameConfig.withoutHurtSecond, player.resetBattleState, player)
            player:subHealth(v.attack)
        end
    end
end

function GameMonster:getCountDown(sign)
    if self.countDown[sign] ~= nil then
        return self.countDown[sign]
    end

    return nil
end

function GameMonster:setCountDown(sign, time)
    if self.countDown[sign] == nil then
        self.countDown[sign] = {}
    end

    self.countDown[sign].time = time
end