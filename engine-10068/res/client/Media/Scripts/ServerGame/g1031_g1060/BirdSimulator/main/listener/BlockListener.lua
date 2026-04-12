BlockListener = {}

function BlockListener:init()
    BlockBreakWithMetaEvent.registerCallBack(self.onBlockBreakWithMeta)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BirdSimulatorGatherEvent.registerCallBack(self.onBirdSimulatorGather)
    BirdSimulatorAddGatherScoreEvent.registerCallBack(self.onBirdSimulatorAddGatherScore)
end

function BlockListener.onBlockBreakWithMeta(rakssid, blockId, blockMeta, vec3)
    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    return false
end

function BlockListener.onBirdSimulatorGather(userId, num, vec3)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end

    local fieldInfo = FieldConfig:getFieldInfoByPos(vec3)

    local fieldId = 0
    local type = 0
    if fieldInfo ~= nil then
        fieldId = tonumber(fieldInfo.id)
        type = tonumber(fieldInfo.type)
    end

    if type == 0 and player:isBackpackFull() then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 2, Messages:backpackFull())
        return false
    end

    num.value = 1
    local toolInfo = ToolConfig:getToolByItemId(player.inHandItemId)
    if toolInfo ~= nil then
        num.value = toolInfo.num
    end

    if fieldId ~= 0 then
        FieldConfig:generateCoin(fieldId, vec3)
    end

    return true
end

function BlockListener.onBirdSimulatorAddGatherScore(userId, birdId, blockId, blockPos, score, maxStage, stage)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end

    if stage.value >= maxStage then
        return false
    end

    local fruitNum = 0
    local layer = 0
    local bird = nil

    local colorId = player:getFruitColorByBlockId(blockId)
    local fieldInfo = FieldConfig:getFieldInfoByPos(blockPos)

    local fieldId = 0
    local type = 0
    if fieldInfo ~= nil then
        fieldId = tonumber(fieldInfo.id)
        type = tonumber(fieldInfo.type)
    end

    if tostring(birdId) == "0" then
        fruitNum, layer = player:collectFruit(score, maxStage, stage, blockId)
    else
        if player.userBird == nil then
            return false
        end

        bird = player.userBird:getBirdById(birdId)
        if bird == nil then
            return false
        end

        local birdInfo = BirdConfig:getBirdInfoById(bird.birdId)
        if birdInfo == nil then
            return false
        end

        local rate = BirdConfig:getRateByIdAndLevel(tonumber(bird.birdId), tonumber(bird.level))
        local levelExtra = birdInfo.collectValue + birdInfo.collectValueGrow * (bird.level - 1) * rate

        local allCollectExtra = player:getAllCollectExtra()
        local colorCollectExtra = player:getColorCollectExtra(blockId)
        local birdColorExtra = BirdConfig:getExtraColorCollect(bird.birdId, colorId)

        fruitNum = math.ceil(score * levelExtra * (1 + birdColorExtra + allCollectExtra + colorCollectExtra))
        if fruitNum < 1 then
            fruitNum = 1
        end

        local remainLayer = maxStage - stage.value

        if remainLayer >= birdInfo.collectLayer then
            layer = birdInfo.collectLayer
        else
            layer = remainLayer
        end
    end

    if player.isVipArea == false then
        if player:isBackpackFull() then
            return false
        end
        player:addFruitNum(fruitNum, true)
        HostApi.sendBirdAddScore(player.rakssid, fruitNum, colorId)
    else
        if type == 1 then
            player:addMoney(fruitNum)
        else
            if player:isBackpackFull() then
                return false
            end
            player:addFruitNum(fruitNum, true)
            HostApi.sendBirdAddScore(player.rakssid, fruitNum, colorId)
        end
    end

    if bird ~= nil then
        player.userBird:createBuffCoin(birdId, 3)
        player.userBird:addExp(bird.id, 1)
    end

    stage.value = stage.value + layer

    player:addTreeScore(fieldId, fruitNum)

    SceneManager:changeTreeHp(fieldId, fruitNum)

    TaskHelper.dispatch(TaskType.CollectFieldFruitEvent,player.userId, fieldId, fruitNum)
    TaskHelper.dispatch(TaskType.CollectFruitByNum,player.userId, fruitNum)
    TaskHelper.dispatch(TaskType.CollectFruitByColor,player.userId, fruitNum, colorId)

    return true
end

return BlockListener

