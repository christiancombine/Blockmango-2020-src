--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

require "Match"
require "messages.Messages"

BlockListener = {}

function BlockListener:init()
    BlockBreakWithMetaEvent.registerCallBack(self.onBlockBreakWithMeta)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
    DamageOnFireByLavaTimeEvent.registerCallBack(self.onDamageOnFireByLavaTime)
    BlockStationaryNotStationaryEvent.registerCallBack(self.onBlockStationaryNotStationary)
    SchemeticPlaceBlockEvent.registerCallBack(self.onSchemeticPlaceBlock)
    BlockCropsPlaceEvent.registerCallBack(self.onBlockCropsPlace)
    BlockCropsUpdateEvent.registerCallBack(self.onBlockCropsUpdate)
    PlaceDoorBlockEvent.registerCallBack(self.onPlaceDoorBlock)
    BlockFLowingHighControlEvent.registerCallBack(self.onBlockFLowingHighControl)
    BlockNeighborChangeEvent.registerCallBack(self.onBlockNeighborChange)
    PlaceSlabBlockEvent.registerCallBack(self.onPlaceSlabBlock)
    BlockCropsNotDropEvent.registerCallBack(self.onBlockCropsNotDrop)
    PlaceFarmlandEvent.registerCallBack(self.onPlaceFarmland)
    BlockBreakWithGunEvent.registerCallBack(self.onBlockBreakWithGun)
    BlockCropsStealDeleteEvent.registerCallBack(self.onBlockCropsStealDelete)
    PlaceSignPostEvent.registerCallBack(self.onPlaceSignPost)
    CropsNotChangeNeighborEvent.registerCallBack(self.onCropsNotChangeNeighbor)
    PlaceItemBlockEvent.registerCallBack(self.onPlaceItemBlock)
end

function BlockListener.onBlockBreakWithMeta(rakssid, blockId, blockMeta, vec3, itemID)
    local integral = BlockSettingConfig:getIntegralByBlockId(blockId , blockMeta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    -- in the main hall
    if player.isGenisland then
        HostApi.log("onBlockBreak info1 userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        return false
    end

    if player.isLife == false then
        return false
    end

	if player.isNotInPartyCreateOrOver == false then
        return false
    end

    -- bed rock
    if blockId == BlockID.BEDROCK then
        HostApi.log("onBlockBreak info2 userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        return false
    end

    -- masterManor is not exist
    local masterManor = GameMatch:getUserManorInfo(player.target_user_id)
    if masterManor == nil then
        HostApi.log("onBlockBreak info3 userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        return false
    end

    -- master is not exist
    local master = PlayerManager:getPlayerByUserId(player.target_user_id)
    if master == nil then
        HostApi.log("onBlockBreak info4 userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        MsgSender.sendCenterTipsToTarget(player.rakssid, 1, Messages:msgCannotOperatorVistor())
        return false
    end

    -- do not have power
    if not masterManor:checkPower(player.userId, vec3, "onBlockBreakWithMeta") then
        HostApi.log("onBlockBreak info5 userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        MsgSender.sendCenterTipsToTarget(player.rakssid, 1, Messages:msgCannotOperatorVistor())
        return false
    end

    -- block is loading
    if masterManor:isLoadingBlock() then
        HostApi.log("onBlockBreak info6 userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        return false
    end

    -- subkey is not load
    if not player:checkChunkLoad() then
        HostApi.log("onBlockBreak info7 userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        return false
    end

    if blockId == BlockID.CHEST then
        -- just master can break
        if not GameMatch:isMaster(player.userId) then
            MsgSender.sendCenterTipsToTarget(rakssid, 1, Messages:msgBlockPlaceTip())
            HostApi.log("onBlockBreak info8 userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                    .. " blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
            return false
        else
            if not DbUtil.CanSavePlayerData(master, DbUtil.TAG_CHEST) then
                HostApi.log("onBlockBreak info9 userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                        .. " blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
                return false
            else
                local manorInfo = GameMatch:getUserManorInfo(player.userId)
                if not manorInfo then
                    return false
                end
                local inv = EngineWorld:getWorld():getInventory(blockId, vec3)
                if not inv then
                    return false
                end
                inv = TileEntityChest.dynamicCast(inv)
                if not inv then
                    return false
                end
                manorInfo:fillOpenChest(vec3, inv)
            end
        end
    end

    if not masterManor:isIn(vec3) then
        HostApi.log("onBlockBreak error : Weak net back home bug. userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        return false
    end

    if (blockId == BlockID.FURNACE_IDLE or blockId == BlockID.FURNACE_BURNING) then
        -- just master can break
        if not GameMatch:isMaster(player.userId) then
            MsgSender.sendCenterTipsToTarget(rakssid, 1, Messages:msgBlockPlaceTip())
            HostApi.log("onBlockBreak info10 userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                    .. " blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
            return false
        else
            if not DbUtil.CanSavePlayerData(master, DbUtil.TAG_FURNACE) then
                HostApi.log("onBlockBreak info11 userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                        .. " blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
                return false
            end
        end
    end
    if blockId == BlockID.SIGN_POST or blockId == BlockID.SIGN_WALL then
        if not DbUtil.CanSavePlayerData(master, DbUtil.TAG_SIGN) then
            return false
        end
        masterManor:recordSignPostText(vec3, "", true)
        integral = BlockSettingConfig:getIntegralByBlockId(blockId , 0)
    end

    if CropsConfig:checkCropsByBlockId(blockId) and blockMeta < 4 then
        ---果实必须成熟了才可以破坏
        return false
    end

    HostApi.log("onBlockBreak suc userId: " .. tostring(player.userId) .. " target_user_id: " .. tostring(player.target_user_id) .. " blockId: " .. blockId ..
            " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
    player:logPlayerBuildGroup("onBlockBreak suc")

    master:addExp(-integral)
    BlockSettingConfig:BreakBlockDropItem(master, blockId, blockMeta, vec3)
    masterManor:recordBlock(vec3, 0, 0)

    GameAnalytics:design(player.userId, 0, {"g1048blockbreak", blockId, blockMeta})

    -- remove crops info
    if CropsConfig:checkCropsByBlockId(blockId) then
        masterManor:removeCropsInfo(vec3)
    end

    -- remove chest info
    if blockId == BlockID.CHEST then
        masterManor:removeChest(vec3)
        DbUtil.SavePlayerChestData(player, true)
    end

    -- remove furnace info
    if blockId == BlockID.FURNACE_IDLE or blockId == BlockID.FURNACE_BURNING then
        masterManor:removeFurnace(vec3)
    end

    -- remove tree timer and tree info
    if blockId == BlockID.SAPLING  then
        local vec3Hash = vec3.x .. ":" .. vec3.y .. ":" .. vec3.z
        if GameMatch.treeTime[tostring(master.userId)] and GameMatch.treeTime[tostring(master.userId)][vec3Hash] then
            LuaTimer:cancel(GameMatch.treeTime[tostring(master.userId)][vec3Hash])
            GameMatch.treeTime[tostring(master.userId)][vec3Hash] = nil
            masterManor:removeTreeInfo(vec3)
        end
    end

    -- remove dirt timer and dirt info
    if blockId == BlockID.DIRT  then
        local vec3Hash = vec3.x .. ":" .. vec3.y .. ":" .. vec3.z
        if GameMatch.dirtTime[tostring(master.userId)] and GameMatch.dirtTime[tostring(master.userId)][vec3Hash] then
            LuaTimer:cancel(GameMatch.dirtTime[tostring(master.userId)][vec3Hash])
            GameMatch.dirtTime[tostring(master.userId)][vec3Hash] = nil
            masterManor:removeDirtInfo(vec3)
        end
    end

    -- home may change
    master:moveHome(vec3, false)

    return true
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    local integral = BlockSettingConfig:getIntegralByBlockId(itemId, meta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if player.isLife == false then
        return false
    end

    local yaw = player:getYaw()

    if player.isNotInPartyCreateOrOver == false then
        return false
    end

    -- in the main hall
    if player.isGenisland then
        MsgSender.sendCenterTipsToTarget(rakssid, 1, Messages:msgBlockPlaceTip())
        HostApi.log("onBlockPlace info1[isGenisland] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        return false
    end

    -- masterManor is not exist
    local masterManor = GameMatch:getUserManorInfo(player.target_user_id)
    if masterManor == nil then
        HostApi.log("onBlockPlace info2[masterManor] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        return false
    end

    -- master is not exist
    local master = PlayerManager:getPlayerByUserId(player.target_user_id)
    if master == nil then
        HostApi.log("onBlockPlace info3[master] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        MsgSender.sendCenterTipsToTarget(player.rakssid, 1, Messages:msgCannotOperatorVistor())
        return false
    end

    -- do not have power
    if not masterManor:checkPower(player.userId, toPlacePos, "onBlockPlace") then
        HostApi.log("onBlockPlace info4[checkPower] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        MsgSender.sendCenterTipsToTarget(player.rakssid, 1, Messages:msgCannotOperatorVistor())
        return false
    end

    -- block is loading
    if masterManor:isLoadingBlock() then
        HostApi.log("onBlockPlace info5[isLoadingBlock] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        return false
    end

    -- subkey is not load
    if not player:checkChunkLoad() then
        HostApi.log("onBlockPlace info6[checkChunkLoad] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        return false
    end

    if not masterManor:isIn(toPlacePos) then
        HostApi.log("onBlockPlace error : Weak net back home bug. userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        return false
    end

    -- tool
    if GameConfig:isTool(itemId) then
        if GameConfig:isHoe(itemId) then
            HostApi.log("onBlockPlace info7[isHoe] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                    .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
            return true
        end
        HostApi.log("onBlockPlace info8[isTool] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        return false
    end

    if itemId == BlockID.CHEST then
        -- just master can place
        if not GameMatch:isMaster(player.userId) then
            HostApi.log("onBlockPlace info9[CHEST Not Master] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                    .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
            return false
        else
            if masterManor:checkBoxLimit() then
                HostApi.log("onBlockPlace info10[checkBoxLimit] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                        .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
                MsgSender.sendCenterTipsToTarget(rakssid, 1, Messages:msgBoxLimit())
                return false
            end

            if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_CHEST) then
                HostApi.log("onBlockPlace info11[CanSavePlayerData TAG_CHEST] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                        .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
                return false
            end

            if not masterManor:canPlaceChest(toPlacePos) then
                HostApi.log("onBlockPlace info12[checkActorExistByPos] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                        .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
                return false
            end
        end
    end

    if (itemId == BlockID.FURNACE_IDLE or itemId == BlockID.FURNACE_BURNING) then
        -- just master can place
        if not GameMatch:isMaster(player.userId) then
            HostApi.log("onBlockPlace info12[FURNACE Not Master] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                    .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
            return false
        else
            if masterManor:checkFurnaceLimit() then
                HostApi.log("onBlockPlace info13[checkFurnaceLimit] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                        .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
                MsgSender.sendCenterTipsToTarget(rakssid, 1, Messages:msgFurnaceLimit())
                return false
            end

            if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_FURNACE) then
                HostApi.log("onBlockPlace info14[CanSavePlayerData TAG_FURNACE] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                        .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
                return false
            end
        end
    end

    -- area
    local isInArea = GameConfig:checkBlockInArea(toPlacePos, master, masterManor)
    if isInArea == false then
        player:checkExtendAreaTip()
        HostApi.log("onBlockPlace info15[isInArea] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        return false
    end

    -- door
    if GameConfig:isDoorItemBlock(itemId) then
        HostApi.log("onBlockPlace info16[isDoorItemBlock] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        return true
    end

    if CropsConfig:checkCropsByItemId(itemId) then
        HostApi.log("onBlockPlace info17[checkCropsByItemId] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        return true
    end

    -- sign
    if itemId == ItemID.SIGN then
        HostApi.log("onBlockPlace info18[ItemID.SIGN] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        return true
    end

    --custom item
    if CustomItemConfig:isCustomItem(itemId) then
        HostApi.log("onBlockPlace info19[isCustomItem] userId: " .. tostring(player.userId) .. " masterId: " .. tostring(player.target_user_id)
                .. " blockId: " .. itemId .. " blockMeta: " .. meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        return true
    end

    --half slab block less than 1000
    if GameConfig:isCommonHalfSlab(itemId) then
        HostApi.log("onBlockPlace isCommonHalfSlab " .. itemId)

        return true
    end

    --half slab block more than 1000
    if GameConfig:isMoreThan1000HalfSlabItem(itemId) then
        HostApi.log("onBlockPlace isMoreThan1000HalfSlabItem " .. itemId)

        return true
    end

    -- BoneMeal for the moment blockId is 1500
    if itemId == BlockID.BONEMEAL + 1000 then
        if player:removeCurItem(1) then

            local item = LotteryUtil:get(BoneMealConfig.ORDINARY_WEIGHTS_GROUP)
            local blockNewId = BoneMealConfig:getFlowerBlockIDById(item)
            local blockNewMeta = BoneMealConfig:getFlowerBlockMetaById(item)
            LuaTimer:scheduleTimer(function()
                EngineWorld:setBlock(toPlacePos, blockNewId, blockNewMeta)
            end, 1000, 1)
            masterManor:recordBlock(toPlacePos, blockNewId, blockNewMeta)

            GameAnalytics:design(player.userId, 0, {"g1048blockplace", blockNewId, blockNewMeta})
            HostApi.log("onBlockPlace suc userId: " .. tostring(player.userId) .. " target_user_id: " .. tostring(player.target_user_id) .. " blockId: " .. blockNewId
                    .. " blockMeta: " .. blockNewMeta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
            player:logPlayerBuildGroup("onBlockPlace suc BlockID.BONEMEAL")
        end

        return false
    end

    -- blockId more than 1000 handle
    if GameConfig:isMoreThan1000Block(itemId) then

        if player:removeCurItem(1) then

            local new_meta_1000 = meta
            if GameConfig:isStairsBlock(itemId - 1000) then
                new_meta_1000 = GameConfig:metaHandleStairs(yaw, new_meta_1000)
            end

            EngineWorld:setBlock(toPlacePos, itemId - 1000, new_meta_1000, 3)

            masterManor:recordBlock(toPlacePos, itemId - 1000, new_meta_1000)
            local exp = BlockSettingConfig:getIntegralByBlockId(itemId - 1000, meta)
            master:addExp(exp)

            GameAnalytics:design(player.userId, 0, { "g1048blockplace", itemId - 1000, new_meta_1000})
            HostApi.log("onBlockPlace suc userId: " .. tostring(player.userId) .. " target_user_id: " .. tostring(player.target_user_id) .. " blockId: " .. (itemId - 1000)
                    .. " blockMeta: " .. new_meta_1000 .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
            player:logPlayerBuildGroup("onBlockPlace suc isMoreThan1000Block")
        end

        return false
    end

    if player:removeCurItem(1) then
        -- task handle
        LuaTimer:scheduleTimer(function(rakssid)
            local player = PlayerManager:getPlayerByRakssid(rakssid)
            if player then
                local num = player:getItemNumById(itemId, meta)
                player.taskController:packingProgressData(player, Define.TaskType.collect, GameMatch.gameType, itemId, num, meta)
            end
        end, 1000, 1, rakssid)

        -- sapling handle
        if itemId == BlockID.SAPLING then

            local time = TreeSectionConfig:getTimeByBlockMeta(meta)
            if time then
                local vec3Hash = toPlacePos.x .. ":" .. toPlacePos.y .. ":" .. toPlacePos.z
                if GameMatch.treeTime[tostring(master.userId)] == nil then
                    GameMatch.treeTime[tostring(master.userId)] = {}
                end
                if GameMatch.treeTime[tostring(master.userId)][vec3Hash] == nil then
                    GameMatch.treeTime[tostring(master.userId)][vec3Hash] = LuaTimer:scheduleTimer(function(rakssid)
                        local master = PlayerManager:getPlayerByRakssid(rakssid)
                        if master then
                            TreeSectionConfig:theSchematicReplaceBlock(master.rakssid, meta, toPlacePos)
                            LuaTimer:cancel(GameMatch.treeTime[tostring(master.userId)][vec3Hash])
                            GameMatch.treeTime[tostring(master.userId)][vec3Hash] = nil

                            local masterManor = GameMatch:getUserManorInfo(master.userId)
                            if masterManor then
                                masterManor:removeTreeInfo(toPlacePos)
                            end
                        end
                    end, time * 1000, 1, master.rakssid)

                    masterManor:insertTreeInfo(toPlacePos, meta)
                end
            end
        end

        -- dirt handle
        if itemId == BlockID.DIRT then
            local time = GameConfig.dirtTime
            local vec3Hash = toPlacePos.x .. ":" .. toPlacePos.y .. ":" .. toPlacePos.z
            if GameMatch.dirtTime[tostring(master.userId)] == nil then
                GameMatch.dirtTime[tostring(master.userId)] = {}
            end
            if GameMatch.dirtTime[tostring(master.userId)][vec3Hash] == nil then
                GameMatch.dirtTime[tostring(master.userId)][vec3Hash] = LuaTimer:scheduleTimer(function(rakssid)
                    local master = PlayerManager:getPlayerByRakssid(rakssid)
                    if master then

                        local masterManor = GameMatch:getUserManorInfo(master.userId)
                        if masterManor then
                            EngineWorld:setBlock(toPlacePos, BlockID.GRASS)
                            masterManor:recordBlock(toPlacePos, BlockID.GRASS, 0)
                            masterManor:removeDirtInfo(toPlacePos)
                        end

                        if GameMatch.dirtTime[tostring(master.userId)][vec3Hash] then
                            LuaTimer:cancel(GameMatch.dirtTime[tostring(master.userId)][vec3Hash])
                            GameMatch.dirtTime[tostring(master.userId)][vec3Hash] = nil
                        end
                    end
                end, time * 1000, 1, master.rakssid)

                masterManor:insertDirtInfo(toPlacePos, itemId)
            end
        end

        -- block meta handle
        local new_meta = meta

        if itemId == BlockID.FURNACE_IDLE then
            new_meta = GameConfig:metaHandleFurnace(yaw, new_meta)
            masterManor:recordFurnace(toPlacePos)
        end

        if itemId == BlockID.FENCE_GATE
         or itemId == BlockID.WHITE_FENCE_GATE then
            new_meta = GameConfig:metaHandleGate(yaw, new_meta)
        end

        if GameConfig:isStairsBlock(itemId) then
            new_meta = GameConfig:metaHandleStairs(yaw, new_meta)
        end

        if itemId == BlockID.CHEST then
            -- just master can place
            masterManor:recordChest(toPlacePos)
        end

        GameAnalytics:design(player.userId, 0, { "g1048blockplace", itemId, new_meta})
        HostApi.log("onBlockPlace suc userId: " .. tostring(player.userId) .. " target_user_id: " .. tostring(player.target_user_id) .. " blockId: " .. itemId
                .. " blockMeta: " .. new_meta .. " pos: " .. toPlacePos.x .. " " .. toPlacePos.y .. " " .. toPlacePos.z)
        player:logPlayerBuildGroup("onBlockPlace suc")

        EngineWorld:setBlock(toPlacePos, itemId, new_meta, 3)

        masterManor:recordBlock(toPlacePos, itemId, new_meta)
        master:addExp(integral)

        master:moveHome(toPlacePos, true)
    end

    return false
end

function BlockListener.onBlockCropsPlace(userId, blockId, vec3, residueHarvestNum)

    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local masterManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
    if masterManorInfo == nil then
        return false
    end

    local master = PlayerManager:getPlayerByUserId(player.target_user_id)
    if master == nil then
        return false
    end

	-- do not have power
    if not masterManorInfo:checkPower(player.userId, vec3, "onBlockCropsPlace") then
        return false
    end

    local lastUpdateTime = os.time()

    if masterManorInfo.release_status == Define.manorRelese.Not then
        masterManorInfo:insertCropsInfo(vec3, blockId, 1, lastUpdateTime)
        masterManorInfo:recordBlock(vec3, blockId, 1)

        HostApi.log("onBlockCropsPlace suc userId: " .. tostring(player.userId) .. " target_user_id: " .. tostring(player.target_user_id) .. " blockId: " .. blockId
                .. " blockMeta: " .. 1 .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        player:logPlayerBuildGroup("onBlockCropsPlace")
    end

end

function BlockListener.onBlockCropsUpdate(userId, blockId, curState, curStateTime, fruitNum, vec3, residueHarvestNum)

    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local masterManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
    if masterManorInfo == nil then
        return false
    end

    local master = PlayerManager:getPlayerByUserId(player.target_user_id)
    if master == nil then
        return false
    end

    -- do not have power
    if not masterManorInfo:checkPower(player.userId, vec3, "onBlockCropsUpdate") then
        return false
    end

    local lastUpdateTime = os.time()

    if masterManorInfo.release_status == Define.manorRelese.Not then
        masterManorInfo:insertCropsInfo(vec3, blockId, curState, lastUpdateTime)
        masterManorInfo:recordBlock(vec3, blockId, curState)

        HostApi.log("onBlockCropsUpdate suc userId: " .. tostring(player.userId) .. " target_user_id: " .. tostring(player.target_user_id) .. " blockId: " .. blockId
                .. " blockMeta: " .. curState .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
        player:logPlayerBuildGroup("onBlockCropsUpdate")
    end

end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

function BlockListener.onDamageOnFireByLavaTime(entityId, num)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player ~= nil then
        num.value = GameConfig.lavaFireTime
    end
    return true
end

function BlockListener.onBlockStationaryNotStationary(blockId)
    return false
end

function BlockListener.onSchemeticPlaceBlock(rakssid, vec3, blockId, meta, placeBlockParam)
    -- clear block
    if placeBlockParam == -1 then
        local invChest = EngineWorld:getWorld():getInventory(BlockID.CHEST, vec3)
        if invChest then
            invChest:clear()
        end
        local invFurnaceIdle = EngineWorld:getWorld():getInventory(BlockID.FURNACE_IDLE, vec3)
        if invFurnaceIdle then
            invFurnaceIdle:clear()
        end
        local invFurnaceBurning = EngineWorld:getWorld():getInventory(BlockID.FURNACE_BURNING, vec3)
        if invFurnaceBurning then
            invFurnaceBurning:clear()
        end
        return true
    end

    -- init schmetic
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        -- HostApi.log("onSchemeticPlaceBlock " .. blockId .. " " .. meta .. " " .. placeBlockParam)
        if (blockId == BlockID.WATER_MOVING or blockId == BlockID.WATER_STILL) and meta == 0 then
            local targetManorInfo = GameMatch:getUserManorInfoByIndex(placeBlockParam)
            if targetManorInfo ~= nil then
                targetManorInfo:recordWaterSource(vec3)
            end
        end
        return true
    else
        if player.isNotInPartyCreateOrOver == false then
            return true
        end

        local masterManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
        if masterManorInfo then
            masterManorInfo:recordBlock(vec3, blockId, meta)
        end
    end

    return true
end

function BlockListener.onPlaceDoorBlock(userId, blockId, blockMeta, vec3)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

	if player.isNotInPartyCreateOrOver == false then
        return
    end

    local targetManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
    if targetManorInfo then
        -- do not have power
        if not targetManorInfo:checkPower(player.userId, vec3, "onPlaceDoorBlock") then
            return
        end
        targetManorInfo:recordBlock(vec3, blockId, blockMeta)
    end
end

function BlockListener.onBlockFLowingHighControl(blockId, pos, posDown)

    for k, v in pairs(GameMatch.userManorInfo) do
        if v:isIn(pos) then
            if v:checkFlowHigh(pos) then
                return true
            end
        end
    end

    return false
end

function BlockListener.onBlockNeighborChange(vec3, id, meta, m_blockID)
    local master = GameMatch:getMasterByPos(VectorUtil.toVector3(vec3))
    if master then
        local masterManorInfo = GameMatch:getUserManorInfo(master.userId)

        if masterManorInfo.release_status ~= Define.manorRelese.Not then
            HostApi.log("onBlockNeighborChange masterManorInfo status may be error" .. masterManorInfo.release_status
                .. " userId: " .. tostring(master.userId) .. " masterManorInfo status: " .. masterManorInfo.release_status .. " info: "
                .. vec3.x .. " " .. vec3.y .. " " .. vec3.z .. " " .. id .. " " .. meta .. " " .. m_blockID)
        end

        if masterManorInfo and masterManorInfo.loading == false and masterManorInfo.release_status == Define.manorRelese.Not then
            HostApi.log("onBlockNeighborChange userId: " .. tostring(master.userId) .. " info: "
                .. vec3.x .. " " .. vec3.y .. " " .. vec3.z .. " " .. id .. " " .. meta .. " " .. m_blockID)
            masterManorInfo:recordBlock(vec3, 0, 0)

            local integral = BlockSettingConfig:getIntegralByBlockId(id, meta)
            -- remove crops info
            if CropsConfig:checkCropsByBlockId(id) then
                masterManorInfo:removeCropsInfo(vec3)
            end

            -- remove signpost
			if id == BlockID.SIGN_POST or id == BlockID.SIGN_WALL then
                masterManorInfo:recordSignPostText(vec3, "", true)
                HostApi.log("onBlockNeighborChange ---------- recordSignPostText")
                integral = BlockSettingConfig:getIntegralByBlockId(id, 0)
            end
            -- remove tree timer and tree info
            if id == BlockID.SAPLING  then
                local vec3Hash = vec3.x .. ":" .. vec3.y .. ":" .. vec3.z
                if GameMatch.treeTime[tostring(master.userId)] and GameMatch.treeTime[tostring(master.userId)][vec3Hash] then
                    LuaTimer:cancel(GameMatch.treeTime[tostring(master.userId)][vec3Hash])
                    GameMatch.treeTime[tostring(master.userId)][vec3Hash] = nil
                    masterManorInfo:removeTreeInfo(vec3)
                end
            end
            master:addExp(-integral)
        end
    end
end

function BlockListener.onPlaceSlabBlock(userId, blockId, blockMeta, vec3, halfBlockId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

	if player.isNotInPartyCreateOrOver == false then
        return
    end

    local targetManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
    if targetManorInfo then
        -- do not have power
        if not targetManorInfo:checkPower(player.userId, vec3, "onPlaceSlabBlock") then
            return
        end

        --targetManorInfo:recordBlock(vec3, blockId, blockMeta)
        --targetManorInfo:recordBlock(VectorUtil.newVector3i(vec3.x, vec3.y + 1, vec3.z), 0, 0)

        if GameConfig:isCommonHalfSlab(halfBlockId) or GameConfig:isMoreThan1000HalfSlabBlock(halfBlockId) then
            HostApi.log("onPlaceSlabBlock blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " halfBlockId: " .. halfBlockId
                    .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)

            -- add single half slab exp
            local expSlab = BlockSettingConfig:getIntegralByBlockId(halfBlockId, blockMeta)
            local master = PlayerManager:getPlayerByUserId(player.target_user_id)
            if master then
                master:addExp(expSlab)
                targetManorInfo:recordBlock(vec3, blockId, blockMeta)
            end
        end
    end
end

function BlockListener.onPlaceItemBlock(userId, blockId, blockMeta, vec3)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    if player.isNotInPartyCreateOrOver == false then
        return
    end

    local targetManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
    if targetManorInfo then
        -- do not have power
        if not targetManorInfo:checkPower(player.userId, vec3, "onPlaceItemBlock") then
            return
        end

        if GameConfig:isCommonHalfSlab(blockId) or GameConfig:isMoreThan1000HalfSlabBlock(blockId) then

            HostApi.log("onPlaceItemBlock blockId: " .. blockId .. " blockMeta: " .. blockMeta .. " pos: " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)

            local expSlab = BlockSettingConfig:getIntegralByBlockId(blockId, blockMeta)
            local master = PlayerManager:getPlayerByUserId(player.target_user_id)
            if master then
                master:addExp(expSlab)
                targetManorInfo:recordBlock(vec3, blockId, blockMeta)
            end
        end
    end
end

function BlockListener.onBlockCropsNotDrop(rakssid, blockId, meta, vec3, owner_user_id)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return true
    end

    local owner = PlayerManager:getPlayerByUserId(owner_user_id)
    if owner == nil then
        return true
    end

    local master = PlayerManager:getPlayerByUserId(player.target_user_id)
    if master == nil then
        return true
    end

    if tostring(owner.userId) ~= tostring(master.userId) then
        return true
    end

    local ownerManorInfo = GameMatch:getUserManorInfo(owner.userId)
    if ownerManorInfo == nil then
        return true
    end

    if not ownerManorInfo:checkPower(player.userId, vec3, "onBlockCropsNotDrop") then
        return true
    end
    HostApi.log("onBlockCropsNotDrop blockId: " .. blockId .. " " .. tostring(player.userId) .. " " .. tostring(owner_user_id))
    player:logPlayerBuildGroup("onBlockCropsNotDrop")

    return false
end

function BlockListener.onPlaceFarmland(userId, blockId, blockMeta, vec3)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    if player.isNotInPartyCreateOrOver == false then
        return
    end

    local targetManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
    if targetManorInfo then
        -- do not have power
        if not targetManorInfo:checkPower(player.userId, vec3, "onPlaceFarmland") then
            return
        end
        targetManorInfo:recordBlock(vec3, blockId, blockMeta)
    end
end

function BlockListener.onBlockBreakWithGun(rakssid, blockId, blockPos, gunId)
    return false
end

function BlockListener.onBlockCropsStealDelete(userId, blockId, productionId, quantity, vec3, residueHarvestNum)
    HostApi.log("onBlockCropsSteal " .. tostring(userId) .. blockId)
    return false
end

function BlockListener.onPlaceSignPost(userId, blockId, blockMeta, vec3)
    -- 这里仅判断留言板放置成功后的操作
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local master = PlayerManager:getPlayerByUserId(player.target_user_id)
    if master == nil then
        return
    end

    local targetManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
    if not targetManorInfo then
        return
    end

    if not targetManorInfo:checkPower(player.userId, vec3, "onPlaceSignPost") then
        return
    end

    targetManorInfo:recordBlock(vec3, blockId, blockMeta)
    local data = {
        hashVec3i = vec3.x .. ":" .. vec3.y .. ":" .. vec3.z,
        oldText = ""
    }
    GamePacketSender:sendSignPostData(player.rakssid, json.encode(data))
    HostApi.log("onPlaceSignPost userId: " .. tostring(player.userId) .. " targetId: " .. tostring(player.target_user_id)
            .. " " .. blockId .. " " .. blockMeta .. " " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
    player:logPlayerBuildGroup("onPlaceSignPost")

    local exp = BlockSettingConfig:getIntegralByBlockId(blockId, 0)
    master:addExp(exp)
end

function BlockListener.onCropsNotChangeNeighbor(rakssid, blockId, pos)
    return false
end

return BlockListener
--endregion
