GameBird = class()

local ConvertState = {
    STOP_CONVERT = 0,           -- 停止转换
    START_CONVERT = 1,          -- 开始转换
    INTERRUPT_CONVERT = 2,      -- 中断转换
}

local DressType = {
    GLASS = 1,      -- 1眼镜
    HAT = 2,        -- 2帽子
    BEAK = 3,       -- 3嘴巴
    WING = 4,       -- 4翅膀
    TAIL = 5,       -- 5尾巴
    EFFECT = 6      -- 6特效
}

function GameBird:ctor(userId)
    self.userId = userId
    self.birds = {}
    self.discovered = {}
    self.buffCoin = {}
    self.dress = {}
    self.index = {}

    self.birdAreaLength = 2 -- 巡逻范围 初始化为 2

    self.lastIndex = 0

    self:initPotralArean()
end

function GameBird:initPotralArean()
    self.positionInfo = {}

    self.areaBegin = -self.birdAreaLength
    self.areaEnd = self.birdAreaLength

    for xPos = self.areaBegin, self.areaEnd, 1 do
        for zPos = self.areaBegin + math.abs(xPos), self.areaEnd - math.abs(xPos), 1 do
            local pos = { x = xPos, z = zPos }
            if self.positionInfo[pos] == nil then
                self.positionInfo[pos] = false
            end
        end
    end
end

function GameBird:getPotralPosNum()
    local num = 0
    for _, v in pairs(self.positionInfo) do
        num = num + 1
    end

    return num
end

function GameBird:isCanCollect()
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return false
    end

    local fieldInfo = FieldConfig:getFieldInfoByPos(player:getPosition())
    local type = 0
    local fieldId = 0
    if fieldInfo ~= nil then
        fieldId = tonumber(fieldInfo.id)
        type = tonumber(fieldInfo.type)
    end

    if fieldId == 0 then
        return false
    end

    if type == 0 and player:isBackpackFull() then
        return false
    end

    return true
end

-- AI 需要对接: 获取小鸟的属性
function GameBird:getFloatAttributeById(id, name)
    -- name : attack        攻击力
    -- name : attackCD      攻击CD
    -- name : flyHigh       飞行高度
    -- name : convertCD     转换速度
    -- name : collectCD     采集CD
    -- name : flySpeed      飞行速度

    for i, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) then
            return self:getValue(v, name)
        end
    end

    print( self.userId .. "'s  bird[" .. id .."] not found : " .. name .. " is 0 ")
    return 0
end

function GameBird:getValue(bird, name)
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return 0
    end

    local birdInfo = BirdConfig:getBirdInfoById(bird.birdId)
    local rate = BirdConfig:getRateByIdAndLevel(tonumber(bird.birdId), tonumber(bird.level))

    if name == "attack" then
        local levelExtra = birdInfo.attackGrow * (bird.level - 1) * rate
        return birdInfo.attack + player.attackExtra + levelExtra
    end

    if name == "attackCD" then
        local levelExtra = birdInfo.attackCDGrow * (bird.level - 1) * rate
        return (birdInfo.attackCD + player.attackCDExtra + levelExtra) * 1000
    end

    if name == "flyHigh" then
        return GameConfig.flyHigh
    end

    if name == "convertCD" then
        local levelExtra = birdInfo.convertCDGrow * (bird.level - 1) * rate
        return (birdInfo.convertCD + player.fruitConvertCDExtra + levelExtra) * 1000
    end

    if name == "collectCD" then
        local levelExtra = birdInfo.collectCDGrow * (bird.level - 1) * rate
        return (birdInfo.collectCD + player.allCollectCDExtra + levelExtra) * 1000
    end

    if name == "flySpeed" then
        return birdInfo.flySpeed + birdInfo.flySpeedGrow * (bird.level - 1) * rate
    end
end

--  AI需要对接:  巡逻接口
function GameBird:patrol(id)
    self:createBuffCoin(id, 4)
end

function GameBird:selectPotralPositionForBirdById(birdId)
    local bird = self:getBirdById(birdId)

    if not bird then
        return VectorUtil.ZERO
    end

    return self:selectPotralPositionForBird(bird)
end

function GameBird:selectPotralPositionForBird(bird)
    if self:getPotralPosNum() <= 2*self:getBirdNumInWorld() then
        self.birdAreaLength = self.birdAreaLength + 2
        self:initPotralArean()
    end

    for _, v in pairs(self.birds) do
        if v.entityId == 0 and v.posIndex ~= nil then
            self.positionInfo[v.posIndex] = false
        end
    end

    local latestPos
    if bird and bird.posIndex ~= nil then
        latestPos = bird.posIndex
    end

    local posIndex = HostApi.random("GameBird_Potrol_Pos", 0, self:getPotralPosNum() - 2)
    local pos
    while not pos do
        local k = 0
        for i, v in pairs(self.positionInfo) do
            k = k + 1
            if k > posIndex and v == false then
                self.positionInfo[i] = true
                bird.posIndex = i
                pos = i

                if latestPos ~= nil then
                    self.positionInfo[latestPos] = false
                end

                break
            end
        end

        if not pos then
            posIndex = 0
        end
    end

    return VectorUtil.newVector3(pos.x, 0, pos.z)
end

function GameBird:initDataFromDB(data)
    self:initDressFromDB(data.dress or {})
    self:initDiscoveredFromDB(data.discovered or {})
    self:initLastIndexFromDB(data.index or 0)
    self:initBirdsFromDB(data.birds or {})
end

function GameBird:initDiscoveredFromDB(discovered)
    for _, v in pairs(discovered) do
        self:addDiscoverBird(v)
    end
end

function GameBird:initLastIndexFromDB(lastIndex)
    self.lastIndex = lastIndex
end

function GameBird:initBirdsFromDB(data)
    for i, v in pairs(data) do
        self:createBird(v.id, v.birdId, v.level, v.exp, v.status, v.partIds)
    end
end

function GameBird:initDressFromDB(dress)
    for _, v in pairs(dress) do
        local data = {}
        data.itemId = tostring(v.itemId)
        data.type = tostring(v.type)
        data.bodyId = tostring(v.bodyId)
        data.bird = tostring(v.bird)
        table.insert(self.dress, data)
    end
end

function GameBird:prepareDataSaveToDB()
    local data = {
        dress = self:prepareDressToDB(),
        birds = self:prepareBirdsInfoToDB(),
        discovered = self:prepareDiscoveredToDB(),
        index = self:prepareLastIndexToDB(),
    }
    return data
end

function GameBird:prepareBirdsInfoToDB()
    local data = {}
    for i, v in pairs(self.birds) do
        local item = {}
        item.id = v.id
        item.birdId = v.birdId
        item.level = v.level
        item.exp = v.exp
        item.status = v.status
        local partIds = {
            glassesId = v.glassesId,
            hatId = v.hatId,
            beakId = v.beakId,
            wingId = v.wingId,
            tailId = v.tailId,
            effectId = v.effectId,
            bodyId = v.bodyId
        }
        item.partIds = partIds
        table.insert(data, item)
    end

    return data
end

function GameBird:prepareDiscoveredToDB()
    return self.discovered
end

function GameBird:prepareLastIndexToDB()
    return self.lastIndex
end

function GameBird:prepareDressToDB()
    return self.dress
end

function GameBird:createBird(id, birdId, level, exp, status, partIds)
    local birdInfo = BirdConfig:getBirdInfoById(birdId)

    if birdInfo ~= nil then
        local bird = {}
        bird.id = id
        bird.birdId = birdId
        bird.entityId = 0
        bird.level = level
        bird.exp = exp
        bird.status = 0
        bird.glassesId = partIds.glassesId
        bird.hatId = partIds.hatId
        bird.beakId = partIds.beakId
        bird.wingId = partIds.wingId
        bird.tailId = partIds.tailId
        bird.effectId = partIds.effectId
        bird.bodyId = partIds.bodyId
        bird.collectValue = 0
        bird.convertState = 0 -- 0 结束转换， 1 转换中 ， 2 中断转换

        local player = PlayerManager:getPlayerByUserId(self.userId)
        if player == nil or player.userBirdBag == nil then
            return
        end

        if status == 1 then
            local index = 0
            bird.status = status
            for i, birdId in pairs(self.index) do
                if tostring(birdId) == "0" then
                    self.index[i] = bird.id
                    index = tonumber(i)
                    break
                end
            end

            if index == 0 then
                self.index[#self.index + 1] = bird.id
                index = #self.index
            end

            local vec3 = player:getPosition()
            local vec3i = NestConfig:getNestPosByIndex(player.userNest.nestIndex, index)

            if vec3 ~= nil and vec3i ~= nil then
                player.userBirdBag:addCurCarryBirdNum(1)
                player:setBirdSimulatorBag()
                bird.entityId = EngineWorld:getWorld():addEntityBird(vec3, self, vec3i, player.userId, id, birdInfo.actorName, birdInfo.actorBody, birdInfo.actorBodyId)
                EngineWorld:getWorld():setBirdDress(bird.entityId, bird.glassesId, bird.hatId, bird.beakId, bird.wingId, bird.tailId, bird.effectId)
            else
                print(tostring(self.userId) .. "'s nestIndex = ".. player.userNest.nestIndex .. ", index = " .. index .. ":  vec3 = nil")
            end
        end
        player.userBirdBag:addCurCapacityBirdNum(1)
        player:setBirdSimulatorBag()

        --添加到小鸟图鉴
        self:addDiscoverBird(birdId)
        player:setBirdSimulatorAtlas()
        table.insert(self.birds, bird)

        -- 同步到抽蛋机图鉴
        DrawLuckyManager:SyncDrawLucky(player)

        TaskHelper.dispatch(TaskType.HaveLevelBirdEvent, player.userId)
    end
end

function GameBird:respawnBirds()
    local player = PlayerManager:getPlayerByUserId(self.userId)

    for i, v in pairs(self.birds) do
        if v.entityId ~= 0 then
            EngineWorld:removeEntity(v.entityId)
            for index, birdId in pairs(self.index) do
                if tostring(birdId) == tostring(v.id) then
                    self.index[index] = 0
                end
            end
        end
    end

    for _, v in pairs(self.birds) do
        if v.status == 1 then
            local birdInfo = BirdConfig:getBirdInfoById(v.birdId)
            if birdInfo == nil then
                return
            end

            if player ~= nil then
                local index = 0
                for i, birdId in pairs(self.index) do
                    if tostring(birdId) == "0" then
                        self.index[i] = v.id
                        index = tonumber(i)
                        break
                    end
                end

                if index == 0 then
                    self.index[#self.index + 1] = v.id
                    index = #self.index
                end

                local vec3i = NestConfig:getNestPosByIndex(player.userNest.nestIndex, index)
                if vec3i ~= nil then
                    v.entityId = EngineWorld:getWorld():addEntityBird(player:getPosition(), self, vec3i, player.userId, v.id, birdInfo.actorName, birdInfo.actorBody, birdInfo.actorBodyId)
                    EngineWorld:getWorld():setBirdDress(v.entityId, v.glassesId, v.hatId, v.beakId, v.wingId, v.tailId, v.effectId)
                end
            end
        end
    end

    if player ~= nil then
        player:setBirdSimulatorBag()
    end

end

function GameBird:getBirdsNumByLevel(level)
    local num = 0
    for i, v in pairs(self.birds) do
        if tonumber(v.level) >= tonumber(level) then
            num = num + 1
        end
    end
    return num
end

function GameBird:getLastBirdIndex()
    self.lastIndex = self.lastIndex + 1
    return self.lastIndex
end

function GameBird:isHasThisBirdId(id)
    for i, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) then
            return true
        end
    end
    return false
end

function GameBird:getBirdById(id)
    for _, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) then
            return v
        end
    end

    return nil
end

function GameBird:getConvertStateById(id)
    for _, v in pairs(self.birds) do
        if tostring(id) == tostring(v.id) then
            return v.convertState
        end
    end

    return 0
end

function GameBird:setStopConvertState()
    local player = PlayerManager:getPlayerByUserId(self.userId)
    for _, v in pairs(self.birds) do
        if v.status == 1 then
            v.convertState = ConvertState.STOP_CONVERT
            if player ~= nil and v.collectValue > 0 then
                player:addFruitNum(v.collectValue, false)
                v.collectValue = 0
            end
        end
    end
end

function GameBird:setStartConvertState()
    for _, v in pairs(self.birds) do
        v.convertState = ConvertState.START_CONVERT
    end
end

function GameBird:setInterruptState()
    for _, v in pairs(self.birds) do
        v.convertState = ConvertState.INTERRUPT_CONVERT
    end
end

function GameBird:carryBirdToWorld(id)
    for _, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) then
            if v.status == 1 then
                return
            end

            local birdInfo = BirdConfig:getBirdInfoById(v.birdId)
            if birdInfo == nil then
                return
            end

            v.status = 1
            local player = PlayerManager:getPlayerByUserId(self.userId)
            if player ~= nil then
                local index = 0
                for i, birdId in pairs(self.index) do
                    if tostring(birdId) == "0" then
                        self.index[i] = v.id
                        index = tonumber(i)
                        break
                    end
                end

                if index == 0 then
                    self.index[#self.index + 1] = v.id
                    index = #self.index
                end

                local vec3i = NestConfig:getNestPosByIndex(player.userNest.nestIndex, index)
                if vec3i ~= nil then
                    player.userBirdBag:addCurCarryBirdNum(1)
                    player:setBirdSimulatorBag()
                    v.entityId = EngineWorld:getWorld():addEntityBird(player:getPosition(), self, vec3i, player.userId, id, birdInfo.actorName, birdInfo.actorBody, birdInfo.actorBodyId)
                    EngineWorld:getWorld():setBirdDress(v.entityId, v.glassesId, v.hatId, v.beakId, v.wingId, v.tailId, v.effectId)
                end

            end
        end
    end
end

function GameBird:callBackBirdToBag(id)
    for i, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) and v.status == 1 then
            local player = PlayerManager:getPlayerByUserId(self.userId)
            if player ~= nil then
                player.userBirdBag:subCurCarryBirdNum(1)
            end
            EngineWorld:removeEntity(v.entityId)
            v.status = 0
            v.entityId = 0
            for index, birdId in pairs(self.index) do
                if tostring(birdId) == tostring(id) then
                    self.index[index] = 0
                end
            end
        end
    end
end

function GameBird:deleteBirdById(id)
    for i, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) then
            local player = PlayerManager:getPlayerByUserId(self.userId)
            if v.entityId ~= 0 then
                EngineWorld:removeEntity(v.entityId)
                for index, birdId in pairs(self.index) do
                    if tostring(birdId) == tostring(v.id) then
                        self.index[index] = 0
                    end
                end
                if player ~= nil and player.userBirdBag ~= nil then
                    player.userBirdBag:subCurCarryBirdNum(1)
                end
            end

            if player ~= nil and player.userBirdBag ~= nil then
                player.userBirdBag:subCurCapacityBirdNum(1)
            end

            for _, d in pairs(self.dress) do
                if tostring(d.bird) == tostring(v.id) then
                    d.bird = "0"
                end
            end

            table.remove(self.birds, i)
            if player ~= nil then
                player:setBirdSimulatorBirdDress()
                player:setBirdSimulatorBag()
                TaskHelper.dispatch(TaskType.HaveLevelBirdEvent, player.userId)
            end
        end
    end
end

function GameBird:deleteBirds()
    for _, v in pairs(self.birds) do
        if v.entityId ~= 0 then
            EngineWorld:removeEntity(v.entityId)
        end
    end
end

function GameBird:isLevelMaximum(id)
    for i, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) then
            local maxLevel = BirdConfig:getMaxLevel()
            local maxExp = BirdConfig:getExpByLevel(maxLevel)
            if tonumber(v.exp) >= tonumber(maxExp) then
                return true
            end
        end
    end

    return false
end

function GameBird:getCurExp(id)
    for i, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) then
            return v.exp
        end
    end

    return 0
end

function GameBird:addExp(id, exp)
    if exp <= 0 then
        return
    end

    for i, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) then
            v.exp = v.exp + exp
            local maxLevel = BirdConfig:getMaxLevel()
            local maxExp = BirdConfig:getExpByLevel(maxLevel)
            if v.exp >= maxExp then
                v.exp = maxExp
            end

            local level = BirdConfig:getLevelByExp(v.exp)
            if v.level ~= level then
                v.level = level
                local player = PlayerManager:getPlayerByUserId(self.userId)
                if player ~= nil then
                    TaskHelper.dispatch(TaskType.HaveLevelBirdEvent, player.userId)
                end
            end
            break
        end
    end

    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player ~= nil then
        player:setBirdSimulatorBag()
    end
end

function GameBird:attackMonster(id, entityId)
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end

    for _, v in pairs(self.birds) do
        if tostring(id) == tostring(v.id) then
            local birdInfo = BirdConfig:getBirdInfoById(v.birdId)
            local rate = BirdConfig:getRateByIdAndLevel(tonumber(v.birdId), tonumber(v.level))
            local levelExtra = birdInfo.attackGrow * (v.level - 1) * rate
            local damageHp = birdInfo.attack + player.attackExtra + levelExtra
            if damageHp <= 0 then
                return
            end

            local players = PlayerManager:getPlayers()
            for _, p in pairs(players) do
                if p.userMonster ~= nil then
                    local monster = p.userMonster:getMonsterByEntityId(entityId)
                    if monster ~= nil then
                        HostApi.sendBirdAddScore(player.rakssid, damageHp, -1)  --  -1代表怪物，其他代表花的颜色
                        monster.hp = monster.hp - damageHp
                        if monster.hp <= 0 then
                            monster.hp = 0
                            p.userMonster:setMonsterDead(entityId)
                            if monster.monsterId == 4009 or monster.monsterId == 4010 then
                                HostApi.sendPlaySound(p.rakssid, 317)
                            end
                        end
                        EngineWorld:getWorld():setEntityHealth(entityId, tonumber(monster.hp), tonumber(monster.maxHp))
                    end
                end
            end
        end
    end
end

function GameBird:createBuffCoin(id, status)
    local bird = {}
    for _, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) and v.entityId ~= 0 then
            bird = v
        end
    end

    if next(bird) == nil then
        return
    end

    local birdId = bird.birdId
    local birdInfo = BirdConfig:getBirdInfoById(birdId)
    if birdInfo == nil then
        return
    end

    local isAction = false
    for _, v in pairs(birdInfo.action) do
        if tonumber(v) == tonumber(status) then
            isAction = true
        end
    end

    if birdInfo.chance == 0 or birdInfo.coinId == 0 or isAction == false then
        return
    end

    local randomNum = HostApi.random("birdBuff", 1, 10000)
    if randomNum > birdInfo.chance then
        return
    end

    local coinId = birdInfo.coinId
    local num = 1
    local meta = 0
    local lifeTime = 10
    local pos = EngineWorld:getWorld():getPositionByEntityId(bird.entityId)
    pos.y = pos.y + 1
    local entityId = EngineWorld:addEntityItem(coinId, num, meta, lifeTime, pos)

    if coinId >= 1057 and coinId <= 1066 then
        SceneManager:addCoinLevel(entityId, bird.level)
        local name = "addBuffCoin:" .. tostring(entityId)
        TimerManager.AddDelayListener(name, 10, SceneManager.removeCoinLevelByKey, SceneManager)
    end
end

function GameBird:getBirdUIInfo()
    local birds = {}
    for i, v in pairs(self.birds) do
        local attack = 0
        local gather = 0
        local quality = 0
        local speed = 0
        local convert = 0
        local convertTime = 0
        local gatherTime = 0
        local name = ""
        local icon = ""
        local skill = ""
        local talent = ""
        local convertVipExtra = 0
        local player = PlayerManager:getPlayerByUserId(self.userId)
        if player ~= nil then
            convertVipExtra = player.convertVipExtra
        end

        local birdInfo = BirdConfig:getBirdInfoById(v.birdId)
        local rate = BirdConfig:getRateByIdAndLevel(tonumber(v.birdId), tonumber(v.level))
        if birdInfo ~= nil then
            attack = birdInfo.attack + birdInfo.attackGrow * (v.level - 1) * rate
            gather = birdInfo.collectValue + birdInfo.collectValueGrow * (v.level - 1) * rate
            name = birdInfo.name
            icon = birdInfo.icon
            quality = birdInfo.quality
            speed = birdInfo.flySpeed  + birdInfo.flySpeedGrow * (v.level - 1) * rate
            convert = (birdInfo.convert + birdInfo.convertGrow * (v.level - 1) * rate) * (1 + convertVipExtra)
            skill = birdInfo.skill
            talent = birdInfo.talent
            convertTime = birdInfo.convertCD + birdInfo.convertCDGrow * (v.level - 1) * rate
            gatherTime = birdInfo.collectCD + birdInfo.collectCDGrow * (v.level - 1) * rate
        end

        local nextExp = BirdConfig:getExpByLevel(v.level + 1)

        local bird = BirdInfo.new()
        bird.id = v.id
        bird.level = v.level
        bird.maxLevel = BirdConfig:getMaxLevel()
        bird.exp = v.exp
        bird.nextExp = nextExp
        bird.quality = quality
        bird.speed = speed
        bird.gather = gather
        bird.attack = attack
        bird.conver = convert
        bird.convertTime = convertTime
        bird.gatherTime = gatherTime
        bird.isCarry = false
        if v.status == 1 then
            bird.isCarry = true
        end

        bird.name = name
        bird.icon = icon
        bird.skill = skill
        bird.talent = talent

        bird.glassesId = v.glassesId
        bird.hatId = v.hatId
        bird.beakId = v.beakId
        bird.wingId = v.wingId
        bird.tailId = v.tailId
        bird.effectId = v.effectId
        bird.bodyId = v.bodyId

        table.insert(birds, bird)
    end

    return birds
end

function GameBird:feed(birdId, foodId, num)
    if num <= 0 then
        return
    end

    local exp = BirdConfig:getExpByFoodId(foodId) * num
    if exp <= 0 then
        return
    end

    self:addExp(birdId, exp)
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player ~= nil then
        TaskHelper.dispatch(TaskType.FeedBirdEvent, player.userId, foodId, num)
    end
end

function GameBird:addDress(itemId, type, bodyId)
    local data = {}
    data.itemId = tostring(itemId)
    data.type = tostring(type)
    data.bodyId = tostring(bodyId)
    data.bird = tostring(0)
    table.insert(self.dress, data)
end

function GameBird:useDress(birdId, itemId)
    for i, v in pairs(self.dress) do
        if tostring(v.itemId) == tostring(itemId) then
            v.bird = tostring(birdId)
            return true
        end
    end

    return false
end

function GameBird:unusedDress(itemId)
    for i, v in pairs(self.dress) do
       if tostring(v.index) == tostring(itemId) then
            v.bird = "0"
            return true
        end
    end
    return false
end

function GameBird:setDress(birdId, itemId, dressType)

    for _, v in pairs(self.birds) do
        if tostring(v.id) == tostring(birdId) then

            if tonumber(dressType) == DressType.GLASS then
                for _, dress in pairs(self.dress) do
                    if tostring(v.glassesId) == tostring(dress.bodyId) and tostring(dress.type) == tostring(dressType) then
                        dress.bird = "0"
                        local parts = BirdConfig:getPartIdsById(v.birdId)
                        v.glassesId = tostring(parts.glassesId)
                        if v.entityId ~= 0 then
                            EngineWorld:getWorld():updateBirdDress(v.entityId, "glasses", v.glassesId)
                        end
                    end
                end

                if tostring(itemId) ~= "0" then
                    local partInfo = BirdConfig:getPartInfoById(itemId)
                    if partInfo == nil then
                        return
                    end
                    if self:useDress(birdId, itemId) then
                        v.glassesId = tostring(partInfo.bodyId)
                        if v.entityId ~= 0 then
                            EngineWorld:getWorld():updateBirdDress(v.entityId, "glasses", v.glassesId)
                        end
                    end
                end
            end


            if tonumber(dressType) == DressType.HAT then
                for _, dress in pairs(self.dress) do
                    if tostring(v.hatId) == tostring(dress.bodyId) and tostring(dress.type) == tostring(dressType) then
                        dress.bird = "0"
                        local parts = BirdConfig:getPartIdsById(v.birdId)
                        v.hatId = tostring(parts.hatId)
                        if v.entityId ~= 0 then
                            EngineWorld:getWorld():updateBirdDress(v.entityId, "hat", v.hatId)
                        end
                    end
                end

                if tostring(itemId) ~= "0" then
                    local partInfo = BirdConfig:getPartInfoById(itemId)
                    if partInfo == nil then
                        return
                    end
                    if self:useDress(birdId, itemId) then
                        v.hatId = tostring(partInfo.bodyId)
                        if v.entityId ~= 0 then
                            EngineWorld:getWorld():updateBirdDress(v.entityId, "hat", v.hatId)
                        end
                    end
                end
            end


            if tonumber(dressType) == DressType.BEAK then
                for _, dress in pairs(self.dress) do
                    if tostring(v.beakId) == tostring(dress.bodyId) and tostring(dress.type) == tostring(dressType) then
                        dress.bird = "0"
                        local parts = BirdConfig:getPartIdsById(v.birdId)
                        v.beakId = tostring(parts.beakId)
                        if v.entityId ~= 0 then
                            EngineWorld:getWorld():updateBirdDress(v.entityId, "beak", v.beakId)
                        end
                    end
                end

                if tostring(itemId) ~= "0" then
                    local partInfo = BirdConfig:getPartInfoById(itemId)
                    if partInfo == nil then
                        return
                    end
                    if self:useDress(birdId, itemId) then
                        v.beakId = tostring(partInfo.bodyId)
                        if v.entityId ~= 0 then
                            EngineWorld:getWorld():updateBirdDress(v.entityId, "beak", v.beakId)
                        end
                    end
                end
            end


            if tonumber(dressType) == DressType.WING then
                for _, dress in pairs(self.dress) do
                    if tostring(v.wingId) == tostring(dress.bodyId) and tostring(dress.type) == tostring(dressType) then
                        dress.bird = "0"
                        local parts = BirdConfig:getPartIdsById(v.birdId)
                        v.wingId = tostring(parts.wingId)
                        if v.entityId ~= 0 then
                            EngineWorld:getWorld():updateBirdDress(v.entityId, "wing", v.wingId)
                        end
                    end
                end

                if tostring(itemId) ~= "0" then
                    local partInfo = BirdConfig:getPartInfoById(itemId)
                    if partInfo == nil then
                        return
                    end
                    if self:useDress(birdId, itemId) then
                        v.wingId = tostring(partInfo.bodyId)
                        if v.entityId ~= 0 then
                            EngineWorld:getWorld():updateBirdDress(v.entityId, "wing", v.wingId)
                        end
                    end
                end
            end


            if tonumber(dressType) == DressType.TAIL then
                for _, dress in pairs(self.dress) do
                    if tostring(v.tailId) == tostring(dress.bodyId) and tostring(dress.type) == tostring(dressType) then
                        dress.bird = "0"
                        local parts = BirdConfig:getPartIdsById(v.birdId)
                        v.tailId = tostring(parts.tailId)
                        if v.entityId ~= 0 then
                            EngineWorld:getWorld():updateBirdDress(v.entityId, "tail", v.tailId)
                        end
                    end
                end

                if tostring(itemId) ~= "0" then
                    local partInfo = BirdConfig:getPartInfoById(itemId)
                    if partInfo == nil then
                        return
                    end
                    if self:useDress(birdId, itemId) then
                        v.tailId = tostring(partInfo.bodyId)
                        if v.entityId ~= 0 then
                            EngineWorld:getWorld():updateBirdDress(v.entityId, "tail", v.tailId)
                        end
                    end
                end
            end


            if tonumber(dressType) == DressType.EFFECT then
                for _, dress in pairs(self.dress) do
                    if tostring(v.effectId) == tostring(dress.bodyId) and tostring(dress.type) == tostring(dressType) then
                        dress.bird = "0"
                        local parts = BirdConfig:getPartIdsById(v.birdId)
                        v.effectId = tostring(parts.effectId)
                        if v.entityId ~= 0 then
                            EngineWorld:getWorld():updateBirdDress(v.entityId, "effect", v.effectId)
                        end
                    end
                end

                if tostring(itemId) ~= "0" then
                    local partInfo = BirdConfig:getPartInfoById(itemId)
                    if partInfo == nil then
                        return
                    end
                    if self:useDress(birdId, itemId) then
                        v.effectId = tostring(partInfo.bodyId)
                        if v.entityId ~= 0 then
                            EngineWorld:getWorld():updateBirdDress(v.entityId, "effect", v.effectId)
                        end
                    end
                end
            end

        end
    end

    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player ~= nil then
        player:setBirdSimulatorBirdDress()
        player:setBirdSimulatorBag()
    end
end

function GameBird:getBirdInfoById(id)
    for i, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) then
            return v
        end
    end

    return nil
end

function GameBird:getBirdNumByQualityId(qualityId)
    -- 根据品质获取小鸟数量
    local num = 0
    for _, v in pairs(self.birds) do
        local birdInfo = BirdConfig:getBirdInfoById(v.birdId)
        if birdInfo ~= nil and tostring(birdInfo.quality) == tostring(qualityId) then
            num = num + 1
        end
    end

    return num
end

function GameBird:getBirdNumByColorId(colorId)
    -- 根据颜色获取小鸟数量
    local num = 0
    for _, v in pairs(self.birds) do
        local birdInfo = BirdConfig:getBirdInfoById(v.birdId)
        if birdInfo ~= nil and tostring(birdInfo.color) == tostring(colorId) then
            num = num + 1
        end
    end

    return num
end

function GameBird:getBirdNumInWorld()
    -- 获取在世界场景中的小鸟数量
    local num = 0
    for _, v in pairs(self.birds) do
        if v.entityId ~= 0 then
            num = num + 1
        end
    end

    return num
end

function GameBird:getBirdNum()
    -- 获取所有小鸟数量
    return #self.birds
end

function GameBird:addDiscoverBird(birdId)
    for _, v in pairs(self.discovered) do
        if tostring(v) == tostring(birdId) then
            return
        end
    end

    table.insert(self.discovered, birdId)
end

function GameBird:isAlreadyDiscovered(birdId)
    for i, v in pairs(self.discovered) do
        if tostring(birdId) == tostring(v) then
            return true
        end
    end

    return false
end

function GameBird:getAtlasInfo(birds)
    local items = {}
    for _, birdId in pairs(birds) do
        local item = BirdEgg.new()
        item.id = tonumber(birdId)
        item.quality = 1
        item.icon = ""
        item.isHave = self:isAlreadyDiscovered(birdId)
        local birdInfo = BirdConfig:getBirdInfoById(birdId)
        if birdInfo ~= nil then
            item.quality = birdInfo.quality
            item.icon = birdInfo.icon
        end
        table.insert(items, item)
    end

    return items
end

function GameBird:convertFruit(id)
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end

    if player.isShowArrow == true then
        player.isShowArrow = false
        if player.userNest ~= nil then
            local nestInfo = NestConfig:getNestInfoById(player.userNest.nestIndex)
            local pos = VectorUtil.newVector3(nestInfo.pos.x, nestInfo.pos.y, nestInfo.pos.z)
            HostApi.changeGuideArrowStatus(player.rakssid, pos, false)
        end
    end

    for _, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) then
            if v.status == 1 then
                if player.curFruitNum > 0 then
                    if player:getConvert() then
                        local birdInfo = BirdConfig:getBirdInfoById(v.birdId)
                        if birdInfo ~= nil then
                            local rate = BirdConfig:getRateByIdAndLevel(tonumber(v.birdId), tonumber(v.level))
                            local levelExtra = birdInfo.convertGrow * (v.level - 1) * rate
                            local convertValue = (birdInfo.convert + levelExtra) * (1 + player.convertVipExtra)
                            if convertValue < 1 then
                                convertValue = 1
                            end
                            if player.curFruitNum > convertValue then
                                v.collectValue = convertValue
                            else
                                v.collectValue = player.curFruitNum
                            end

                            player:subFruitNum(v.collectValue)
                        end
                    end
                else
                    v.convertState = ConvertState.STOP_CONVERT
                    player:setConvert(false)
                    player:setBirdSimulatorPlayerInfo()
                end
            end
        end
    end
end

function GameBird:interruptConvert(id)
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end

    for _, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) then
            player:addFruitNum(v.collectValue, false)
            v.convertState = ConvertState.STOP_CONVERT
            v.collectValue = 0
        end
    end
end

function GameBird:finishConvert(id)
    local player = PlayerManager:getPlayerByUserId(self.userId)
    if player == nil then
        return
    end

    for _, v in pairs(self.birds) do
        if tostring(v.id) == tostring(id) then
            player:addMoney(v.collectValue)
            v.collectValue = 0
        end
    end
end