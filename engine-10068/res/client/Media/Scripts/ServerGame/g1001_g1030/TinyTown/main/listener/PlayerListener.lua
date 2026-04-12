--region *.lua
require "messages.Messages"
require "data.GamePlayer"
require "Match"
require "config.ManorNpcConfig"
require "config.YardConfig"
require "config.HouseConfig"
require "config.FurnitureConfig"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerDynamicAttrEvent, self.onDynamicAttrInit)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerBuyCommodityEvent.registerCallBack(self.onPlayerBuyCommodity)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerGetManorEvent.registerCallBack(self.onPlayerGetManor)
    PlayerSellManorEvent.registerCallBack(self.onPlayerSellManor)
    PlayerUpdateManorResultEvent.registerCallBack(self.onPlayerUpdateManorResult)
    PlayerBuyHouseResultEvent.registerCallBack(self.onPlayerBuyHouseResult)
    PlayerBuyFurnitureResultEvent.registerCallBack(self.onPlayerBuyFurnitureResult)
    PlayerBuildHouseEvent.registerCallBack(self.onPlayerBuildHouse)
    PlayerPutFurnitureEvent.registerCallBack(self.onPlayerPutFurniture)
    PlayerRecycleFurnitureEvent.registerCallBack(self.onPlayerRecycleFurniture)
    PlayerManorTeleportEvent.registerCallBack(self.onPlayerManorTeleport)
    PlayerLoadManorInfoEvent.registerCallBack(self.onPlayerLoadManorInfo)
    PlayerLoadManorMasterInfoEvent.registerCallBack(self.onPlayerLoadManorMasterInfo)
    PlayerCallOnTargetManorEvent.registerCallBack(self.onPlayerCallOnTargetManor)
    PlayerPraiseResultEvent.registerCallBack(self.onPlayerPraiseResult)
    UserManorReleaseEvent.registerCallBack(self.onUserManorRelease)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))

    HostApi.loadGameMoney(player.userId)
    HostApi.loadManorMessage(player.userId, 1)
    HostApi.loadMyManorRank(player.userId)

    player.yardLevel = 0
    player.currentHouseId = 0

    local userManorIndex = GameMatch.userManorIndex[tostring(player.userId)]
    if userManorIndex ~= nil then
        player.manorIndex = userManorIndex
    end

    local userYardLevel = GameMatch.userYardLevel[tostring(player.userId)]
    if userYardLevel ~= nil then
        player.yardLevel = userYardLevel
    end

    local userHouseId = GameMatch.userHouseId[tostring(player.userId)]
    if userHouseId ~= nil then
        for i, v in pairs(userHouseId) do
            player.unlockHouse[tonumber(v.tempId)] = tonumber(v.tempId)

            if v.state == 1 then
                player.currentHouseId = tonumber(v.tempId)
            end
        end
    end

    local userInfo = GameMatch.userInfo[tostring(player.userId)]
    if userInfo ~= nil then
        if userInfo.state == 0 then
            player.isGetManor = true
            player.manorId = userInfo.manorId
            player.manorOrientation = userInfo.orientation
            player.isMaster = userInfo.isMaster
            player.vegetableNum = userInfo.vegetableNum
            player.praiseNum = userInfo.praiseNum
            player.manorCharm = userInfo.manorCharm
            player.manorValue = userInfo.manorValue

            player:sendManorBtnVisible()
        end
    end

    local furnitures = GameMatch.furnitureEntityId[tostring(player.userId)]
    if furnitures ~= nil then
        for i, v in pairs(furnitures) do
            player.payedFurniture[tonumber(v.tempId)] = tonumber(v.tempId)

            if v.state == 1 then
                local data = {}
                data.tempId = v.tempId
                data.offsetVec3 = v.offsetVec3 or VectorUtil.ZERO
                data.yaw = v.yaw
                player:initPlaceFurnitureInfo(data)
            end
        end
    end

    if GameMatch.userBackpack[tostring(player.userId)] ~= nil then
        for i, v in pairs(GameMatch.userBackpack[tostring(player.userId)]) do
            player:addItem(v.id, v.num, v.meta)
        end
    end

    HostApi.loadManorInfo(player.userId, false)

    GameMatch:updateManorOwnerInfo(player.rakssid)

    ManorNpcConfig:updateManorInfo(player)
    for i, v in pairs(MerchantConfig.merchants) do
        v:syncPlayer(player)
    end
end

function PlayerListener.onDynamicAttrInit(attr)
    if attr.manorId < 0 then
        return
    end
    local platformId = attr.userId
    local targetUserId = attr.targetUserId
    local manorIndex = attr.manorId
    HostApi.log("==== LuaLog: PlayerListener.onDynamicAttrInit  --- start --- : platformId = " .. tostring(platformId) .. ", targetUserId = " .. tostring(targetUserId) .. ", manorIndex = " .. manorIndex)
    if tostring(targetUserId) ~= "0" and manorIndex > 0 then
        if targetUserId ~= platformId then
            GameMatch.callOnTarget[tostring(platformId)] = targetUserId
        end
        if GameMatch.userManorIndex[tostring(targetUserId)] == nil then
            GameMatch:initUserManorIndex(targetUserId, manorIndex)
            HostApi.loadManorInfo(targetUserId, true)
        else
            HostApi.log("==== LuaLog: PlayerListener.onDynamicAttrInit :  use memory data ...  GameMatch.userManorIndex[" .. tostring(targetUserId) .. "] = " .. GameMatch.userManorIndex[tostring(targetUserId)])
            local userInfo = GameMatch.userInfo[tostring(targetUserId)]
            if userInfo ~= nil then
                HostApi.log("==== LuaLog: ManorInformation player[" .. tostring(targetUserId) .. "] : manorIndex = " .. manorIndex .. ", orientation = " .. userInfo.orientation .. ", praiseNum = " .. userInfo.praiseNum .. ", vegetableNum = " .. userInfo.vegetableNum)
            else
                HostApi.log("==== LuaLog: PlayerListener.onDynamicAttrInit : memory data is nil,  program has error  !!!!! ")
            end
        end
    else
        HostApi.log("==== LuaLog: PlayerListener.onDynamicAttrInit [targetUserId == 0] or [manorIndex <= 0] ")
    end
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if hurtFrom == nil or hurtPlayer == nil then
        return false
    end
    local targetPlayer = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    if targetPlayer ~= nil then
        local rakssId = hurtFrom:getRaknetID()
        local targetId = targetPlayer.userId
        if rakssId ~= nil and targetId ~= nil then
            HostApi.showPlayerOperation(rakssId, targetId)
        else
            HostApi.log("==== LuaLog: HostApi.showPlayerOperation  rakssId == nil or targetId == nil ")
        end
    end
    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    return false
end

function PlayerListener.onPlayerBuyCommodity(rakssid, type, goodsInfo, attr, msg)
    local goodsId = goodsInfo.goodsId
    local goodsNum = goodsInfo.goodsNum
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if goodsId == 10000 then
        attr.isAddGoods = false
        player:addCurrency(goodsNum)
        HostApi.manorTrade(player.userId, goodsNum)
    end
    return true
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerGetManor(rakssid, response)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    local data = json.decode(response)

    local manor = data.manor
    local id = manor.id
    local userId = manor.userId
    local name = manor.name
    local orientation = manor.orientation
    local level = manor.level
    local charm = manor.charm                    -- 院子等级魅力值
    local potential = manor.potential            -- 院子等级价值(游戏币)
    local praiseRatio = manor.praiseRatio        -- 点赞等级系数(20%)
    local ploughRow = manor.ploughRow            -- 可用耕地行数
    local ploughColumn = manor.ploughColumn      -- 可用耕地列数
    local vegetableNum = manor.vegetableNum      -- 玩家可偷菜次数,每天(UTC)重置
    local praiseNum = manor.praiseNum            -- 玩家可点赞次数,每天(UTC)重置
    local state = manor.state                    -- 0.领取，1.已出售给系统，4.已出售给玩家，8.回收
    local praise = manor.praise                  -- 被赞总数
    local visit = manor.visit                    -- 受访总数
    local charmBase = manor.charmBase            -- 装修魅力值 = 物品魅力值总和 + 房屋魅力值
    local charmTotal = manor.charmTotal          --  院子魅力值总和 = 装修魅力值 + 院子等级魅力值 + 点赞数量*系数
    local potentialUsed = manor.potentialUsed    -- 花费估值总和(游戏币)
    local potentialTotal = manor.potentialTotal  -- 院子评估值总和(游戏币) = 花费估值总和(游戏币) * (1 + 魅力值加成)

    if state == 0 then
        player.isGetManor = false
        player.manorId = id
        player.yardLevel = level
        player.currentHouseId = 0
        player.manorOrientation = orientation
        player.manorCharm = charmTotal
        player.manorValue = potentialTotal
        player.appIntegral = player.manorCharm * 4 + player.manorValue
        player.isMaster = false

        ManorNpcConfig:updateManorInfo(player)
        for i, v in pairs(MerchantConfig.merchants) do
            v:syncPlayer(player)
        end

        player:sendManorBtnVisible()
        player:addItem(900, 8, 0)
        HostApi.sendManorOperationTip(player.rakssid, "gui_manor_build_house_message")
    end
end

function PlayerListener.onPlayerSellManor(platformId, isSuccess, response)
    local player = PlayerManager:getPlayerByUserId(platformId)
    if player == nil then
        return
    end

    if isSuccess then
        if player.isGetManor then
            YardConfig:sendPlayerToTelePosition(player.manorIndex, player.yardLevel)
            FurnitureConfig:recycleFurniture(player)
            YardConfig:destroyFence(player.manorIndex, player.yardLevel)
            YardConfig:destroyGarden(player.manorIndex, player.yardLevel)
            HouseConfig:recycleBlock(player)
            HouseConfig:destroyHouse(player.manorIndex, player.currentHouseId)
            HouseConfig:repairLandBeforeBuild(player.manorIndex, player.currentHouseId)
            HouseConfig:destroySignPost(player.manorIndex)
            player:sendManorBtnInvisible()
            player:clearInv()
            player:saveManorInventory()
            player:saveManorBlock()
            player:initPlayerInfo()
            for i, v in pairs(MerchantConfig.merchants) do
                v:syncPlayer(player)
            end

            GameMatch:deleteManorInfo(player.userId)
            GameMatch:updateManorOwnerInfo(0)

        else
            HostApi.log("==== LuaLog: PlayerListener.onPlayerSellManor: [player.isGetManor = false] ")
        end
    else
        HostApi.log("==== LuaLog: PlayerListener.onPlayerSellManor: [isSuccess = false] ")
    end
end

function PlayerListener.onPlayerUpdateManorResult(rakssid, paySuccess, response)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if paySuccess then
        if player.yardLevel < YardConfig.maxLevel then
            local data = json.decode(response)
            local manor = data.manor
            YardConfig:destroyFence(player.manorIndex, player.yardLevel)
            YardConfig:destroyGarden(player.manorIndex, player.yardLevel)
            player.yardLevel = manor.level
            YardConfig:buildFence(player.manorIndex, player.yardLevel)
            YardConfig:buildGarden(player.manorIndex, player.yardLevel)
            ManorNpcConfig:updateManorInfo(player)
            GameMatch:updateBlockCropsInfo(player)
            GameMatch:initUserYardLevel(player.userId, player.yardLevel)
            YardConfig:rebuildGardenSeeds(player)
            HostApi.loadManorInfo(player.userId, false)
            for i, v in pairs(MerchantConfig.merchants) do
                v:syncPlayer(player)
            end
        end
    else
        HostApi.log("==== LuaLog: PlayerListener.onPlayerUpdateManorResult [" .. tostring(player.userId) .. "] update manor failure . ")
    end

end

function PlayerListener.onPlayerBuyHouseResult(rakssid, paySuccess, id, price, charm)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if paySuccess then
        player.unlockHouse[id] = id
        ManorNpcConfig:updateManorInfo(player)
        local data = {}
        data.tempId = id
        data.price = price
        data.charm = charm
        data.state = 0
        GameMatch:initUserHouseId(player.userId, data)
        HostApi.loadManorInfo(player.userId, false)
        HostApi.log("==== LuaLog: PlayerListener.onPlayerBuyHouseResult [" .. tostring(player.userId) .. "] buy house [id=" .. id .. "] successful")
    end
end

function PlayerListener.onPlayerBuyFurnitureResult(rakssid, paySuccess, id, price)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if paySuccess then
        player.payedFurniture[id] = id
        ManorNpcConfig:updateManorInfo(player)

        local furniture = FurnitureConfig:getFurnitureById(tonumber(id))
        local data = {}
        data.tempId = id
        data.yaw = 0
        data.offsetVec3 = VectorUtil.newVector3(0, 0, 0)
        data.state = 0
        data.price = price
        data.entityId = 0
        data.actorName = furniture.name
        GameMatch:initFurnitureEntityId(player.userId, data)
        HostApi.loadManorInfo(player.userId, false)
        HostApi.log("==== LuaLog: PlayerListener.onPlayerBuyFurnitureResult [" .. tostring(player.userId) .. "] buy Furniture [id=" .. id .. "] successful")
    end
end

function PlayerListener.onPlayerBuildHouse(rakssid, houseId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if GameMatch.userManorIndex[tostring(player.userId)] == nil then
        HostApi.log("==== LuaLog: PlayerListener.onPlayerBuildHouse : GameMatch.userManorIndex[" .. tostring(player.userId) .. "] = nil, build failure. ")
        return
    end

    if YardConfig:isInYard(player.manorIndex, player.yardLevel, player:getPosition()) == false then
        HostApi.sendManorOperationTip(player.rakssid, "gui_manor_place_and_build_message")
        return
    end

    local house = HouseConfig:getHouseById(houseId)

    if player.yardLevel < house.yardLevel then
        HostApi.log("==== LuaLog: PlayerListener.onPlayerBuildHouse : player.yardLevel < house.yardLevel, build failure. ")
        HostApi.sendManorOperationTip(player.rakssid, "gui_manor_lack_of_level_message")
        return
    end

    HouseConfig:recycleBlock(player)
    FurnitureConfig:recycleFurniture(player)
    YardConfig:sendPlayerToTelePosition(player.manorIndex, player.yardLevel)
    VehicleConfig:resetPositionBeforeBuildHouse(player.manorIndex, player.yardLevel)

    if player.currentHouseId == 0 then
        HostApi.sendManorOperationTip(player.rakssid, "gui_manor_make_money_buy_furniture_message")
    end

    if player.currentHouseId ~= 0 then
        HouseConfig:destroyHouse(player.manorIndex, player.currentHouseId)
    end

    LuaTimer:schedule(function()
        if not player:checkReady() then
            return
        end
        if player.currentHouseId ~= 0 then
            HouseConfig:repairLandBeforeBuild(player.manorIndex, player.currentHouseId)
        end
        player.currentHouseId = houseId
        HouseConfig:buildHouse(player.manorIndex, player.currentHouseId)
        HostApi.buildHouse(player.userId, player.manorId, player.currentHouseId)
        ManorNpcConfig:updateManorInfo(player)

        for _, v in pairs(GameMatch.userHouseId[tostring(player.userId)]) do
            if v ~= nil then
                v.state = 0
            end
        end

        local data = {}
        data.tempId = house.id
        data.price = house.price
        data.charm = house.charm
        data.state = 1
        GameMatch:initUserHouseId(player.userId, data)
    end, 1000)
end

function PlayerListener.onPlayerPutFurniture(rakssid, furnitureId, status)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if GameMatch.userManorIndex[tostring(player.userId)] == nil then
        return
    end

    local furniture = FurnitureConfig:getFurnitureById(furnitureId)
    if furniture == nil then
        return
    end

    if FurnitureConfig:isFurniturePayed(player, furnitureId) == false then
        HostApi.log("==== LuaLog: onPlayerPutFurniture  player[" .. tostring(player.userId) .. "] -- furniture[" .. furnitureId .. "] not pay, put failure... ")
        return
    end

    if status == FurnitureConfig.putStatus.START then
        FurnitureConfig:placeFurnitureStart(player, furniture)
    elseif status == FurnitureConfig.putStatus.SURE then
        FurnitureConfig:placeFurnitureSure(player, furnitureId, furniture)
    elseif status == FurnitureConfig.putStatus.CANCEL then
        FurnitureConfig:placeFurnitureCancel(player)
    elseif status == FurnitureConfig.putStatus.EDIT_START then
        FurnitureConfig:editFurnitureStart(player, furnitureId, furniture)
    elseif status == FurnitureConfig.putStatus.EDIT_SURE then
        FurnitureConfig:editFurnitureSure(player, furnitureId, furniture)
    elseif status == FurnitureConfig.putStatus.EDIT_CANCEL then
        FurnitureConfig:editFurnitureCancel(player, furnitureId, furniture)
    end

end

function PlayerListener.onPlayerRecycleFurniture(rakssid, furnitureId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if GameMatch.userManorIndex[tostring(player.userId)] == nil then
        return
    end

    local furnitures = GameMatch.furnitureEntityId[tostring(player.userId)]
    if furnitures ~= nil then
        if furnitures[tonumber(furnitureId)] ~= nil then
            if furnitures[tonumber(furnitureId)].entityId ~= nil then
                EngineWorld:removeEntity(furnitures[tonumber(furnitureId)].entityId)
            end

            local vec3 = VectorUtil.newVector3(0, 0, 0)
            HostApi.operationFurniture(player.userId, player.manorId, furnitureId, vec3, 0, 0)

            GameMatch.furnitureEntityId[tostring(player.userId)][tonumber(furnitureId)].yaw = 0
            GameMatch.furnitureEntityId[tostring(player.userId)][tonumber(furnitureId)].offsetVec3 = vec3
            GameMatch.furnitureEntityId[tostring(player.userId)][tonumber(furnitureId)].state = 0
            GameMatch.furnitureEntityId[tostring(player.userId)][tonumber(furnitureId)].entityId = 0

        end
    end

    if player.placeFurniture[furnitureId] ~= nil then
        player.placeFurniture[furnitureId] = nil
    end
    ManorNpcConfig:updateManorInfo(player)
end

function PlayerListener.onPlayerManorTeleport(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    ManorNpcConfig:teleportEvent(player)
end

function PlayerListener.onPlayerLoadManorInfo(userId, response)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if response ~= nil and string.len(response) ~= 0 then
        local data = json.decode(response)
        local manor = data.manor
        local backpacks = data.backpacks
        if manor ~= nil then
            if manor.id ~= nil then
                player.manorId = tostring(manor.id)
                player.vegetableNum = tonumber(manor.vegetableNum)
                player.praiseNum = tonumber(manor.praiseNum)
                player.yardLevel = tonumber(manor.level)
                player.manorValue = manor.potentialTotal
                player.manorCharm = manor.charmTotal
                player.appIntegral = player.manorCharm * 4 + player.manorValue
                player:sendManorBtnVisible()
                ManorNpcConfig:updateManorInfo(player)
                for i, v in pairs(MerchantConfig.merchants) do
                    v:syncPlayer(player)
                end

                if player.isMaster then
                    GameMatch:initUserInfo(userId, manor.id, manor.orientation, manor.state, manor.vegetableNum, manor.praiseNum, manor.charmTotal, manor.potentialTotal)
                end
            end

            local userBackpack = GameMatch.userBackpack[tostring(player.userId)]
            if userBackpack ~= nil then
                -- 领地升级， 不添加物品
                return
            end

            if backpacks ~= nil then
                for i, v in pairs(backpacks) do
                    if v ~= nil then
                        local data = {}
                        data.id = tonumber(v.tempId)
                        data.index = tonumber(v.indexNum)
                        data.meta = tonumber(v.meta)
                        data.num = tonumber(v.amount)
                        player:addItem(data.id, data.num, data.meta)
                    end
                end
            end

        end
    end
end

function PlayerListener.onPlayerLoadManorMasterInfo(userId, response)
    if response ~= nil and string.len(response) ~= 0 then
        local manorIndex = GameMatch.userManorIndex[tostring(userId)]
        local data = json.decode(response)
        local manor = data.manor
        local nextLevel = data.nextLevel
        local houses = data.houses
        local furnitures = data.furnitures
        local blocks = data.blocks
        local ploughs = data.ploughs
        local backpacks = data.backpacks

        if manor ~= nil then
            if manor.id ~= nil then
                local manorId = tostring(manor.id)
                local userId = manor.userId
                local name = manor.name
                local orientation = manor.orientation
                local level = manor.level
                local charm = manor.charm
                local potential = manor.potential
                local praiseRatio = manor.praiseRatio
                local ploughRow = manor.ploughRow
                local ploughColumn = manor.ploughColumn
                local vegetableNum = manor.vegetableNum
                local praiseNum = manor.praiseNum
                local state = manor.state
                local praise = manor.praise
                local visit = manor.visit
                local charmBase = manor.charmBase
                local charmTotal = manor.charmTotal
                local potentialUsed = manor.potentialUsed
                local potentialTotal = manor.potentialTotal

                GameMatch:initUserInfo(userId, manorId, orientation, state, vegetableNum, praiseNum, charmTotal, potentialTotal)
                GameMatch:initUserYardLevel(userId, level)
                YardConfig:buildFence(manorIndex, level)
                YardConfig:buildGarden(manorIndex, level)

                -- HostApi.log("==== LuaLog: ManorInformation player[".. tostring(userId) .. "] : manorId = " .. manorIndex .. ", orientation = " .. orientation .. ", praiseNum = " ..praiseNum .. ", vegetableNum = " .. vegetableNum)

                local manorConfig = ManorNpcConfig:getManorById(manorIndex)
                if manorConfig ~= nil then
                    -- 门牌分南北， meta = 0 和 meta = 24 刚好是180度
                    local meta = 0
                    if manorIndex > 5 then
                        meta = 24
                    end
                    EngineWorld:setBlock(manorConfig.signPosition, BlockID.SIGN_POST, meta)
                    EngineWorld:getWorld():resetSignText(manorConfig.signPosition, 1, name .. "\'s_House")
                end


                -- houses
                if houses ~= nil then
                    for i, v in pairs(houses) do
                        if v ~= nil then
                            local house = {}
                            house.tempId = v.tempId
                            house.charm = v.charm
                            house.state = v.state
                            house.price = v.price
                            if v.state == 1 then
                                HouseConfig:buildHouse(manorIndex, tonumber(v.tempId))
                            end
                            GameMatch:initUserHouseId(userId, house)
                        end
                    end
                end

                -- furniture
                if furnitures ~= nil then
                    for i, v in pairs(furnitures) do
                        if v ~= nil then
                            local furniture = FurnitureConfig:getFurnitureById(tonumber(v.tempId))
                            if furniture ~= nil then
                                local offsetVec3 = VectorUtil.newVector3(v.positionX, v.positionY, v.positionZ)
                                local vec3 = YardConfig:getVec3ByOffset(manorIndex, offsetVec3)
                                local data = {}
                                data.tempId = v.tempId
                                data.offsetVec3 = offsetVec3
                                data.yaw = v.offset
                                data.state = v.state
                                data.price = v.price
                                data.entityId = 0
                                data.actorName = furniture.name
                                if v.state == 1 then
                                    local entityId = EngineWorld:addSessionNpc(vec3, v.offset, ManorNpcConfig.SessionType_MANOR_FURNITURE, furniture.name, function(entity)
                                        entity:setActorBodyId(furniture.bodyId or "")
                                        entity:setSessionContent(tostring(furniture.id) or "")
                                    end)
                                    data.entityId = entityId
                                end
                                GameMatch:initFurnitureEntityId(userId, data)
                            end
                        end
                    end
                end

                -- block
                LuaTimer:schedule(function()
                    if blocks ~= nil then
                        local index = 0
                        local specialBlock = {}
                        for i, v in pairs(blocks) do
                            if v ~= nil then
                                local data = {}
                                data.id = tonumber(v.tempId)
                                data.meta = tonumber(v.meta)
                                data.offsetVec3 = VectorUtil.newVector3i(v.positionX, v.positionY, v.positionZ)
                                if data.id == 65 or data.id == 50 then
                                    index = index + 1
                                    specialBlock[index] = data
                                else
                                    local vec3 = YardConfig:getVec3iByOffset(manorIndex, data.offsetVec3)
                                    EngineWorld:setBlock(vec3, data.id, data.meta)
                                    GameMatch:initTemporaryStore(userId, data)
                                end
                            end
                        end

                        if specialBlock ~= nil then
                            for i, v in pairs(specialBlock) do
                                local vec3 = YardConfig:getVec3iByOffset(manorIndex, v.offsetVec3)
                                EngineWorld:setBlock(vec3, v.id, v.meta)
                                GameMatch:initTemporaryStore(userId, v)
                            end
                        end
                        specialBlock = {}
                    end
                end, 3000)

                -- ploughs
                if ploughs ~= nil then
                    for i, v in pairs(ploughs) do
                        if v ~= nil then
                            local data = {}
                            data.blockId = tonumber(v.blockId)
                            data.row = v.ploughRow
                            data.userId = userId
                            data.column = v.ploughColumn
                            data.curState = v.curState
                            data.curStateTime = v.curStateTime
                            data.lastUpdateTime = v.lastUpdateTime
                            data.fruitNum = v.fruitNum
                            data.pluckingState = v.pluckingState
                            local offsetVec3 = VectorUtil.newVector3i(data.column, 1, data.row)
                            local position = YardConfig:getSeedPositionByOffset(manorIndex, level, offsetVec3)
                            data.vec3 = position
                            data.offsetVec3 = offsetVec3
                            data.manorIndex = manorIndex
                            data.level = level
                            data.manorId = manorId

                            GameMatch:initBlockCrops(userId, data)

                            EngineWorld:getWorld():setCropsBlock(userId, position, data.blockId, data.curState, data.curStateTime, data.fruitNum, data.lastUpdateTime, 1)
                        end
                    end
                end

                GameMatch:updateManorOwnerInfo(0)

            end
        end

        -- backpacks not rely on manor
        if backpacks ~= nil then
            GameMatch.userBackpack[tostring(userId)] = nil
            for i, v in pairs(backpacks) do
                if v ~= nil then
                    local data = {}
                    data.id = tonumber(v.tempId)
                    data.index = tonumber(v.indexNum)
                    data.meta = tonumber(v.meta)
                    data.num = tonumber(v.amount)
                    GameMatch:initUserBackPack(userId, data)
                end
            end
        end
    else
        HostApi.log("==== LuaLog: PlayerListener.onPlayerLoadManorMasterInfo : response == nil , set memory data failure !!!! ")
    end
end

function PlayerListener.onPlayerCallOnTargetManor(userId, targetId)
    local manorIndex = GameMatch.userManorIndex[tostring(targetId)]
    if manorIndex == nil then
        return false
    end

    local manor = ManorNpcConfig:getManorById(manorIndex)
    if manor == nil then
        return false
    end

    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end

    player:teleportPos(manor.telePosition)
    return true
end

function PlayerListener.onPlayerPraiseResult(userId, praiseNum)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

end

function PlayerListener.onUserManorRelease(userId)
    HostApi.log("==== LuaLog: PlayerListener.onUserManorRelease start")
    if GameMatch.userManorIndex[tostring(userId)] ~= nil then
        local manorIndex = GameMatch.userManorIndex[tostring(userId)]
        local yardLevel = GameMatch.userYardLevel[tostring(userId)]
        local userHouseId = GameMatch.userHouseId[tostring(userId)]

        local currentHouseId = 0
        if userHouseId ~= nil then
            for i, v in pairs(userHouseId) do
                if v.state == 1 then
                    currentHouseId = tonumber(v.tempId)
                    break
                end
            end
        end

        local furnitures = GameMatch.furnitureEntityId[tostring(userId)]
        if furnitures ~= nil then
            for i, v in pairs(furnitures) do
                EngineWorld:removeEntity(v.entityId)
            end
        end

        if GameMatch.temporaryStore[tostring(userId)] ~= nil then
            for i, v in pairs(GameMatch.temporaryStore[tostring(userId)]) do
                local vec3 = YardConfig:getVec3iByOffset(manorIndex, v.offsetVec3)
                EngineWorld:setBlockToAir(vec3)
            end
        end

        HouseConfig:destroySignPost(manorIndex)
        YardConfig:destroyFence(manorIndex, yardLevel)
        YardConfig:destroyGarden(manorIndex, yardLevel)
        HouseConfig:destroyHouse(manorIndex, currentHouseId)
        HouseConfig:repairLandBeforeBuild(manorIndex, currentHouseId)
        GameMatch:deleteManorInfo(userId)
        GameMatch:updateManorOwnerInfo(0)
    else
        HostApi.log("==== LuaLog: PlayerListener.onUserManorRelease GameMatch.userManorIndex[" .. tostring(userId) .. "] = nil , ManorRelease failure")
    end
end

return PlayerListener
--endregion
