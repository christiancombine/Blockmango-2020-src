GameParty = {}
GameParty.party = {}
GameParty.DataType = {
    newest = 0,
    playerNum = 1,
    LoveNum = 2,
    frient = 3, 
}

GameParty.party[GameParty.DataType.newest + 1] = {}
GameParty.party[GameParty.DataType.playerNum + 1] = {}
GameParty.party[GameParty.DataType.LoveNum + 1] = {}
GameParty.party[GameParty.DataType.frient + 1] = {}

function GameParty:getAllPartyList()
    SkyBlockWeb:getAllPartyList(GameParty.DataType.newest)
    SkyBlockWeb:getAllPartyList(GameParty.DataType.playerNum)
    SkyBlockWeb:getAllPartyList(GameParty.DataType.LoveNum)
end


function GameParty:getFriendPartyList(userId)
    SkyBlockWeb:getFriendPartyList(userId)
end

function GameParty:getPartyInfo(userId)
    SkyBlockWeb:getPartyInfo(userId)
end

function GameParty:createParty(data)
    SkyBlockWeb:createParty(data)
end

function GameParty:applyParty(userId, partyName, partyDesc)
    SkyBlockWeb:applyParty(userId, partyName, partyDesc)
end

function GameParty:giveLoveInfo(userId, targetUserId)
    SkyBlockWeb:giveLove(userId, targetUserId)
end

function GameParty:getFreeCreateNum(userId)
    SkyBlockWeb:getFreeCreateNum(userId)
end

function GameParty:sendSkyBlockPartyData(info, listType)
    print("sendSkyBlockPartyData")
    info.listType = listType + 1
    self.party[listType + 1] = info
    local data = {}
    table.insert(data, info)
    GamePacketSender:sendSkyBlockPartyData(0, json.encode(data))
end

function GameParty:sendSkyBlockFrientPartyList(userId, info)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    print("sendSkyBlockFrientPartyList userId == "..tostring(userId))
    local cur_data = {}
    cur_data.data = info
    cur_data.listType = GameParty.DataType.frient + 1
    self.party[GameParty.DataType.frient + 1] = cur_data

    local data = {}
    table.insert(data, cur_data)
    GamePacketSender:sendSkyBlockPartyData(player.rakssid, json.encode(data))
end

function GameParty:sendAllPartyData(rakssid)
    print("sendAllPartyData")
    local data = self.party
    GamePacketSender:sendSkyBlockPartyData(rakssid, json.encode(data))
end

function GameParty:recivePartyinfo(data, userId)
    print("recivePartyinfo userId == " .. tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    GamePacketSender:sendPartyData(player.rakssid, json.encode(data))
end

function GameParty:sendPlayerPartyData(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    if DbUtil.CanSavePlayerData(player, DbUtil.TAG_PLAYER) and DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
        local data = {}
        data.giveLoveLastNum = player.giveLoveLastNum
        data.level = player.level
        data.loveNum = player.loveNum
        data.loveNumType = player.loveNumType
        data.freeCreatePartyTimes = player.freeCreatePartyTimes
        GamePacketSender:sendPlayerPartyData(player.rakssid, json.encode(data))
    end
end

function GameParty:getLoveNum(userId, targetId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    if targetId then
        SkyBlockWeb:getLoveNum(userId, targetId)
        return
    end

    if not GameMatch:isMaster(userId) then
        local targetUserId = GameMatch:getTargetByUid(userId)
        if targetUserId then
            SkyBlockWeb:getLoveNum(userId, targetUserId)
        end
    else
        SkyBlockWeb:getLoveNum(userId, userId)
    end
end

function GameParty:partyAddPlayer(userId)
    if not GameMatch:isMaster(userId) then
        local targetUserId = GameMatch:getTargetByUid(userId)
        if targetUserId then
            print("partyAddPlayer ".." userId == "..tostring(userId).."  targetUserId == "..tostring(targetUserId))
            SkyBlockWeb:partyAddPlayer(userId, targetUserId)
        end
    end
end

function GameParty:partySubPlayer(userId)
    if not GameMatch:isMaster(userId) then
        local targetUserId = GameMatch:getTargetByUid(userId)
        if targetUserId then
            print("partySubPlayer ".." userId == "..tostring(userId).."  targetUserId == "..tostring(targetUserId))
            SkyBlockWeb:partyReducePlayer(userId, targetUserId)
        end
    end
end

function GameParty:sendUpDateLoveNum(userId, targetUserId, loveNum)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local data = {}
    data.targetUserId = tostring(targetUserId)
    data.loveNum = loveNum

    GamePacketSender:sendUpDateLoveNum(player.rakssid, json.encode(data))
end



return GameParty