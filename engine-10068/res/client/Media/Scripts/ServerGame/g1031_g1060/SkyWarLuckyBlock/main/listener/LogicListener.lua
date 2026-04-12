---
LogicListener = {}
LogicListener.goldAppleTimeId = {}
LogicListener.goldAppleAddHpTimeId = {}
LogicListener.goldAppleFreshTimeId = {}

function LogicListener:init()
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)
    ItemSkillAttackEvent.registerCallBack(self.onItemSkillAttack)
    SkillThrowableMoveEvent.registerCallBack(self.onSkillThrowableMove)
    FoodEatenEvent.registerCallBack(self.onFoodEaten)

    EntityHitEvent.registerCallBack(self.onEntityHit)
    PotionConvertGlassBottleEvent.registerCallBack(self.onPotionConvertGlassBottle)
    ItemSkillConsumptionItemEvent.registerCallBack(self.onItemSkillConsumptionItem)
    UseItemBucketEvent.registerCallBack(self.onUseItemBucket)
end

function LogicListener.onEntityItemSpawn(itemId, itemMeta, behavior)
    if not GameMatch:isGameRunning() then
        return false
    end
    if behavior == "player.drop" then
        return true
    end

    if behavior == "break.block" then
        return false
    end

    return true
end

function LogicListener.onItemSkillAttack(entityId, itemId, position)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil then
        return
    end
    if player.isLife == false then
        return
    end
    local skill = SkillConfig:getSkill(itemId)
    if skill ~= nil then
        SkillManager:createSkill(skill, player, position)
    end
end

function LogicListener.onEntityHit(entityId, vec3)

end

function LogicListener.onPotionConvertGlassBottle(entityId, itemId)
    return false
end

function LogicListener.onItemSkillConsumptionItem(rakssid, itemId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if not GameMatch:isGameRunning() then
        return false
    end

    local skill = SkillConfig:getSkill(itemId)
    if skill ~= nil and skill.type ~= SkillManager.SkillType.DevilPact
            and skill.type ~= SkillManager.SkillType.HidingSkill then
        player:removeCurItem()
        player:onDropItem(itemId)
    end
end

function LogicListener.onUseItemBucket(entityId, itemId)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil then
        return true
    end
    if not GameMatch:isGameRunning() then
        return false
    end

    if ItemID.BUCKET_WATER == itemId then
        return false
    end

    return true
end

function LogicListener.onFoodEaten(entityId, itemId)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil then
        return true
    end
    if not GameMatch:isGameRunning() then
        return false
    end
    if ItemID.GOLD_APPLE == itemId then
        player:resetApple()
        player.entityPlayerMP:appleFresh(-1)

        local hash = tostring(tostring(player.rakssid) .. os.time())
        -- add hp
        local hpCountDown = tonumber(GameConfig.goldApple[1].hpCountDown)
        local armorDuration = tonumber(GameConfig.goldApple[1].armorDuration)
        local armoraddDuration = tonumber(GameConfig.goldApple[1].armoraddDuration)

        LogicListener.goldAppleAddHpTimeId[hash] = LuaTimer:scheduleTimer(function(userId)
            local c_player = PlayerManager:getPlayerByUserId(userId)
            if c_player == nil then
                return
            end
            c_player:addHealth(tonumber(GameConfig.goldApple[1].addHpValue))

        end, hpCountDown * 1000, armorDuration / hpCountDown, player.userId)

        -- add apple
        if player.hasGoldApple == false or player:canAddApple() then
            if player.canAddMaxApple then
                player:addMaxApple(tonumber(GameConfig.goldApple[1].addarmor))
                player.canAddMaxApple = false
            else
                player:addApple(tonumber(GameConfig.goldApple[1].addarmor))
            end
        end
            player.hasGoldApple = true
            LogicListener.goldAppleTimeId[hash] = LuaTimer:schedule(function(userId)
                local c_player = PlayerManager:getPlayerByUserId(userId)
                if c_player == nil then
                    return
                end

                c_player:subMaxApple(tonumber(GameConfig.goldApple[1].addarmor))
                c_player.hasGoldApple = false
                c_player.canAddMaxApple = true

                if LogicListener.goldAppleTimeId[hash] ~= nil then
                    LuaTimer:cancel(LogicListener.goldAppleTimeId[hash])
                    LogicListener.goldAppleTimeId[hash] = nil
                end
            end, armoraddDuration * 1000, nil, player.userId)


        local freshTime = armoraddDuration - 3
        if freshTime > 0 then
            LogicListener.goldAppleFreshTimeId[hash] = LuaTimer:schedule(function(userId)
                local c_player = PlayerManager:getPlayerByUserId(userId)
                if c_player == nil then
                    return
                end
                if c_player.maxApple - GameConfig.goldApple[1].addarmor == 0 then
                    c_player.entityPlayerMP:appleFresh(3)
                end

                if LogicListener.goldAppleFreshTimeId[hash] ~= nil then
                    LuaTimer:cancel(LogicListener.goldAppleFreshTimeId[hash])
                    LogicListener.goldAppleFreshTimeId[hash] = nil
                end
            end, freshTime * 1000, nil, player.userId)
        end
        return false
    end
    return true
end

function LogicListener.onSkillThrowableMove(entityId, itemId, position)
    if itemId == 2811 then
        local blockPos = VectorUtil.toBlockVector3(position.x, position.y - 2, position.z)
        if EngineWorld:getBlockId(blockPos) ~= BlockID.AIR then
            return
        end
        EngineWorld:setBlock(blockPos, BlockID.CLOTH, 0)
    end
end

return LogicListener