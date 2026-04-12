GamePacketSender = {}

function GamePacketSender:sendWalkingDeadBuffInfo(rakssid, type, duration)
    local buffType = tonumber(type)
    if buffType ~= 13 then
        local params = {
            type = type,
            duration = duration
        }
        local data = DataBuilder.new():fromTable(params):getData()
        PacketSender:sendServerCommonData(rakssid, "PlayerInfoData", "WalkingDeadBuffInfo", data)
    end
end

function GamePacketSender:sendWalkingDeadCanJump(rakssid, canJump)
    local params = {
        canJump = canJump
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(rakssid, "MainControlData", "WalkingDeadIsCanJumpInfo", data)
end

function GamePacketSender:sendWalkingDeadShowStoreId(rakssid, storeId)
    local params = {
        storeId = storeId
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(rakssid, "WalkingDeadGameData", "ShowStoreId", data)
end

function GamePacketSender:sendWalkingDeadUpdateGoods(goodsId, limit)
    local params = {
        goodsId = goodsId,
        limit = limit
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:broadCastServerCommonData("WalkingDeadGameData", "UpdateGoods", data)
end

function GamePacketSender:sendWalkingDeadSupplyData(rakssid, data)
    PacketSender:sendServerCommonData(rakssid, "WalkingDeadGameData", "SupplyData", data)
end

function GamePacketSender:sendWalkingDeadShowVipInfo(rakssid, vip, vipTime)
    local params = {
        vip = vip,
        vipTime = vipTime
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(rakssid, "WalkingDeadGameData", "ShowVipInfo", data)
end

function GamePacketSender:sendWalkingDeadDeathSettlement(rakssid, data)
    PacketSender:sendServerCommonData(rakssid, "WalkingDeadGameData", "DeathSettlement", data)
end

function GamePacketSender:sendWalkingDeadDeathItemsDetail(rakssid, itemList)
    local data = json.encode(itemList)
    PacketSender:sendServerCommonData(rakssid, "WalkingDeadGameData", "DeathItemsDetail", data)
end

function GamePacketSender:sendWalkingDeadDesignationDetail(rakssid, designationData, designationDataType)

    local params = {
        designationData = json.encode(designationData),
        designationDataType = json.encode(designationDataType)
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(rakssid, "WalkingDeadGameData", "DesignationDetail", data)
end

function GamePacketSender:sendWalkingDeadShowStoreByTraveller(rakssid, goodList)
    local data = json.encode(goodList)
    PacketSender:sendServerCommonData(rakssid, "WalkingDeadGameData", "ShowStoreByTraveller", data)
end

return GamePacketSender