---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "messages.Messages"
require "config.YardConfig"
require "config.VehicleConfig"

GameMatch = {}

GameMatch.gameType = "g1020"

GameMatch.curTick = 0

GameMatch.userManorIndex = {}
GameMatch.userYardLevel = {}
GameMatch.userHouseId = {}
GameMatch.userInfo = {}
GameMatch.userBackpack = {}

GameMatch.temporaryStore = {}
GameMatch.furnitureEntityId = {}
GameMatch.blockCrops = {}
GameMatch.yardLevelInfo = {}

GameMatch.callOnTarget = {}


GameMatch.charmRank = {}
GameMatch.potentialRank = {}

GameMatch.charmRankCount = 3
GameMatch.potentialRankCount = 3

DBManager:setPlayerDBSaveFunction(function(player, immediate)
    player:savePlayerData()
end)

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:addPlayerScore()
    self:checkVehicle()
end

function GameMatch:addPlayerScore()
    if self.curTick == 0 or self.curTick % 60 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:addScore(10)
    end
end

function GameMatch:checkVehicle()
    if self.curTick == 0 or self.curTick % 600 ~= 0 then
        return
    end
    VehicleConfig:checkAndResetPosition()
end

function GameMatch:onPlayerQuit(player)
    player:initPlayerInfo()
    player:reward()
    GameMatch.userBackpack[tostring(player.userId)] = nil
end

function GameMatch:initUserInfo(userId, manorId, orientation, state, vegetableNum, praiseNum, charmTotal, potentialTotal)
    GameMatch.userInfo[tostring(userId)] = {}
    GameMatch.userInfo[tostring(userId)].manorId = manorId
    GameMatch.userInfo[tostring(userId)].orientation = orientation
    GameMatch.userInfo[tostring(userId)].state = state
    GameMatch.userInfo[tostring(userId)].isMaster = true
    GameMatch.userInfo[tostring(userId)].vegetableNum = vegetableNum
    GameMatch.userInfo[tostring(userId)].praiseNum = praiseNum
    GameMatch.userInfo[tostring(userId)].manorCharm = charmTotal
    GameMatch.userInfo[tostring(userId)].manorValue = potentialTotal
end

function GameMatch:initUserManorIndex(userId, manorIndex)
    GameMatch.userManorIndex[tostring(userId)] = manorIndex
end

function GameMatch:initUserYardLevel(userId, yardLevel)
    GameMatch.userYardLevel[tostring(userId)] = yardLevel
end

function GameMatch:initUserHouseId(userId, house)
    if GameMatch.userHouseId[tostring(userId)] == nil then
        GameMatch.userHouseId[tostring(userId)] = {}
    end
    GameMatch.userHouseId[tostring(userId)][tonumber(house.tempId)] = {}
    GameMatch.userHouseId[tostring(userId)][tonumber(house.tempId)].tempId = house.tempId
    GameMatch.userHouseId[tostring(userId)][tonumber(house.tempId)].price = house.price
    GameMatch.userHouseId[tostring(userId)][tonumber(house.tempId)].charm = house.charm
    GameMatch.userHouseId[tostring(userId)][tonumber(house.tempId)].state = house.state
end

function GameMatch:initUserBackPack(userId, backpack)
    if GameMatch.userBackpack[tostring(userId)] == nil then
        GameMatch.userBackpack[tostring(userId)] = {}
    end

    GameMatch.userBackpack[tostring(userId)][tonumber(backpack.index)] = {}
    GameMatch.userBackpack[tostring(userId)][tonumber(backpack.index)].id = backpack.id
    GameMatch.userBackpack[tostring(userId)][tonumber(backpack.index)].num = backpack.num
    GameMatch.userBackpack[tostring(userId)][tonumber(backpack.index)].meta = backpack.meta

end

function GameMatch:initTemporaryStore(userId, temporaryStore)
    local sign = VectorUtil.hashVector3(temporaryStore.offsetVec3)
    if GameMatch.temporaryStore[tostring(userId)] == nil then
        GameMatch.temporaryStore[tostring(userId)] = {}
    end

    GameMatch.temporaryStore[tostring(userId)][sign] = {}
    GameMatch.temporaryStore[tostring(userId)][sign].blockId = tonumber(temporaryStore.id)
    GameMatch.temporaryStore[tostring(userId)][sign].blockMeta = tonumber(temporaryStore.meta)
    GameMatch.temporaryStore[tostring(userId)][sign].offsetVec3 = temporaryStore.offsetVec3
end

function GameMatch:initFurnitureEntityId(userId, furniture)
    if GameMatch.furnitureEntityId[tostring(userId)] == nil then
        GameMatch.furnitureEntityId[tostring(userId)] = {}
    end
    GameMatch.furnitureEntityId[tostring(userId)][tonumber(furniture.tempId)] = {}
    GameMatch.furnitureEntityId[tostring(userId)][tonumber(furniture.tempId)].tempId = furniture.tempId
    GameMatch.furnitureEntityId[tostring(userId)][tonumber(furniture.tempId)].yaw = furniture.yaw
    GameMatch.furnitureEntityId[tostring(userId)][tonumber(furniture.tempId)].offsetVec3 = furniture.offsetVec3
    GameMatch.furnitureEntityId[tostring(userId)][tonumber(furniture.tempId)].state = furniture.state
    GameMatch.furnitureEntityId[tostring(userId)][tonumber(furniture.tempId)].price = furniture.price
    GameMatch.furnitureEntityId[tostring(userId)][tonumber(furniture.tempId)].entityId = furniture.entityId
    GameMatch.furnitureEntityId[tostring(userId)][tonumber(furniture.tempId)].actorName = furniture.actorName
end

function GameMatch:initBlockCrops(userId, blockCrops)
    if GameMatch.blockCrops[tostring(userId)] == nil then
        GameMatch.blockCrops[tostring(userId)] = {}
    end

    local sign = VectorUtil.hashVector3(blockCrops.vec3)
    GameMatch.blockCrops[tostring(userId)][sign] = {}
    GameMatch.blockCrops[tostring(userId)][sign].userId = blockCrops.userId
    GameMatch.blockCrops[tostring(userId)][sign].blockId = blockCrops.blockId
    GameMatch.blockCrops[tostring(userId)][sign].vec3 = blockCrops.vec3
    GameMatch.blockCrops[tostring(userId)][sign].offsetVec3 = blockCrops.offsetVec3
    GameMatch.blockCrops[tostring(userId)][sign].curState = blockCrops.curState
    GameMatch.blockCrops[tostring(userId)][sign].curStateTime = blockCrops.curStateTime
    GameMatch.blockCrops[tostring(userId)][sign].lastUpdateTime = blockCrops.lastUpdateTime
    GameMatch.blockCrops[tostring(userId)][sign].fruitNum = blockCrops.fruitNum
    GameMatch.blockCrops[tostring(userId)][sign].manorIndex = blockCrops.manorIndex
    GameMatch.blockCrops[tostring(userId)][sign].level = blockCrops.level
    GameMatch.blockCrops[tostring(userId)][sign].manorId = blockCrops.manorId
    GameMatch.blockCrops[tostring(userId)][sign].pluckingState = blockCrops.pluckingState
end

function GameMatch:updateBlockCropsInfo(player)
    local blockCrops = GameMatch.blockCrops[tostring(player.userId)]
    local datas = {}
    if blockCrops ~= nil then
        for i, v in pairs(blockCrops) do
            if v ~= nil then
                local data = {}
                data.blockId = tonumber(v.blockId)
                data.userId  = player.userId
                data.curState = v.curState
                data.curStateTime = v.curStateTime
                data.lastUpdateTime = v.lastUpdateTime
                data.fruitNum = v.fruitNum
                data.pluckingState = v.pluckingState
                local offsetVec3 = v.offsetVec3
                local position = YardConfig:getSeedPositionByOffset(player.manorIndex, player.yardLevel, offsetVec3)
                data.vec3 = position
                data.offsetVec3 = offsetVec3
                data.manorIndex = player.manorIndex
                data.level = player.yardLevel
                data.manorId = player.manorId
                datas[i] = data
            end
        end
    end

    GameMatch.blockCrops[tostring(player.userId)] = nil

    for i, v in pairs(datas) do
        GameMatch:initBlockCrops(player.userId, v)
    end
end

function GameMatch:updateManorOwnerInfo(rakssid)
    local owners = ManorNpcConfig:newManorOwners()
    EngineWorld:getWorld():updateManorOwner(rakssid, owners)
end

function GameMatch:deleteManorInfo(userId)
    GameMatch.userInfo[tostring(userId)] = nil
    GameMatch.userManorIndex[tostring(userId)] = nil
    GameMatch.userYardLevel[tostring(userId)] = nil
    GameMatch.userHouseId[tostring(userId)] = nil
    GameMatch.userBackpack[tostring(userId)] = nil
    GameMatch.temporaryStore[tostring(userId)] = nil
    GameMatch.furnitureEntityId[tostring(userId)] = nil
    GameMatch.blockCrops[tostring(userId)] = nil
    HostApi.log("==== LuaLog: GameMatch:deleteManorInfo : [".. tostring(userId) .. "] 's information is all delete")
end

return GameMatch