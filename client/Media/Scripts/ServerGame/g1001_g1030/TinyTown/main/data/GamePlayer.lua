---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "Match"
require "config.MerchantConfig"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(0)

    self.config = {}

    self.money = 0
    self.score = 0
    self.modify_block = false

    self:initPlayerInfo()
    self:teleInitPos()

    local targetId = GameMatch.callOnTarget[tostring(self.userId)]
    if  targetId ~= nil then
        if self.userId ~= targetId then
            local manorIndex = GameMatch.userManorIndex[tostring(targetId)]
            if manorIndex ~= nil then
                local manor = ManorNpcConfig:getManorById(manorIndex)
                if manor ~= nil then
                    self:teleportPos(manor.telePosition)
                end
            end
        end
    end

    GameMatch.callOnTarget[tostring(self.userId)] = nil

    self:setCurrency(self.money)
    MerchantConfig:prepareStore(self)

end

function GamePlayer:teleInitPos()
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:restoreActor()
    EngineWorld:restorePlayerActor(self)
end

function GamePlayer:sendManorBtnVisible()
    HostApi.setManorBtnVisible(self.rakssid, true)
end

function GamePlayer:sendManorBtnInvisible()
    HostApi.setManorBtnVisible(self.rakssid, false)
end

function GamePlayer:onMoneyChange()
    self.money = self:getCurrency()
end

function GamePlayer:reward()
    RewardManager:getUserGoldReward(self.userId, self.score, function(data)

    end)
end

function GamePlayer:initPlayerInfo()
    self.manorIndex = 0
    self.manorId = ""
    self.yardLevel = 0

    self.isGetManor = false
    self.manorOrientation = 0
    self.isMaster = false
    self.manorValue = 0
    self.manorCharm = 0
    self.vegetableNum = 0
    self.praiseNum = 0

    self.payedFurniture = {}
    self.placeFurniture = {}
    self.placeFurnitureEditTemp = {}

    self.currentHouseId = 0
    self.unlockHouse = {}

    self.positionBeforePlaceFurniture = {}

end

function GamePlayer:savePlayerData()
    if self.manorId == "" then
        return
    end

    self:saveManorInventory()
    self:saveManorBlock()
end

function GamePlayer:saveManorInventory()
    local inv = self:getInventory()
    local array = {}
    local index = 1
    GameMatch.userBackpack[tostring(self.userId)] = {}
    for i = 1, 40 do
        local info = inv:getItemStackInfo(i - 1)
        if info.id ~= 0 then
            local id = info.id
            local num = info.num
            local meta = info.meta
            local price = 0
            for i, v in pairs(MerchantConfig.items) do
                if v.gid == info.id then
                    price = tonumber(v.ec / v.gc)
                    break
                end
            end

            local data = {}
            data.id = id
            data.index = index
            data.num = num
            data.meta = meta
            GameMatch:initUserBackPack(self.userId, data)

            local inventory = {}
            inventory.amount = num
            inventory.indexNum = i
            inventory.meta = meta
            inventory.price = price
            inventory.tempId = id
            array[index] = inventory
            index = index + 1
        end
    end

    local data = json.encode(array)

    HostApi.saveManorInventory(self.manorId, data)
end

function GamePlayer:saveManorBlock()
    if self.isMaster == false then
        return
    end
    if self.modify_block == false then
        return
    end
    self.modify_block = false
    local blockInfo = GameMatch.temporaryStore[tostring(self.userId)]
    local array = {}
    local index = 1
    if blockInfo ~= nil then
        for i, v in pairs(blockInfo) do
            local charm = 0
            local price = 0
            local block = {}
            block.tempId = tostring(v.blockId)
            block.meta = tostring(v.blockMeta)
            for j, item in pairs(MerchantConfig.items) do
                if item.gid == v.blockId then
                    price = tonumber(item.ec / item.gc)
                    charm = item.charmValue
                    break
                end
            end
            block.charm = charm
            block.price = price
            block.positionX = tonumber(v.offsetVec3.x)
            block.positionY = tonumber(v.offsetVec3.y)
            block.positionZ = tonumber(v.offsetVec3.z)
            array[index] = block
            index = index + 1
        end
    end

    local data = json.encode(array)

    HostApi.saveManorBlock(self.manorId, data)
end

function GamePlayer:initPlaceFurnitureInfo(data)
    self.placeFurniture[tonumber(data.tempId)] = {}
    self.placeFurniture[tonumber(data.tempId)].id = tonumber(data.tempId)
    self.placeFurniture[tonumber(data.tempId)].xOffset = data.offsetVec3.x
    self.placeFurniture[tonumber(data.tempId)].yOffset = data.offsetVec3.y
    self.placeFurniture[tonumber(data.tempId)].zOffset = data.offsetVec3.z
    self.placeFurniture[tonumber(data.tempId)].yaw = data.yaw
end

function GamePlayer:addScore(score)
    self.score = self.score + score
end

return GamePlayer

--endregion


