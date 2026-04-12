GamePacketSender = {}

function GamePacketSender:sendTowerDefenseHallPlayerTowerList(rakssid, towerList)
    local data = {}
    for _, tower in pairs(towerList) do
        table.insert(data, tower)
    end
    PacketSender:sendLuaCommonData(rakssid, "TowerList", json.encode(data))
end

function GamePacketSender:sendTowerDefenseBroadcastInfo(rakssid, text)
    local params = {
        text = text
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "Broadcast", data)
end

function GamePacketSender:sendTowerDefenseBuySuccess(rakssid, itemId)
    local params = {
        itemId = itemId
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "BuySuccess", data)
end

function GamePacketSender:sendTowerDefenseGetNewTower(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "GetNewTower", json.encode(data))
end

function GamePacketSender:sendLevelInfo(player)
    local params = {
        exp = player.cur_exp,
        level = player.level
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(player.rakssid, "LevelInfo", data)
end

function GamePacketSender:sendAddExp(player, exp)
    local params = {
        exp = exp
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(player.rakssid, "AddExp", data)
end

function GamePacketSender:sendSkinInfo(player)
    local params = {
        skinList = player.skinList
    }
    PacketSender:sendLuaCommonData(player.rakssid, "SkinInfo", json.encode(params))
end

function GamePacketSender:sendEquipSkin(player)
    local params = {
        equipSkinId = player.equipSkinId
    }
    PacketSender:sendLuaCommonData(player.rakssid, "EquipSkin", json.encode(params))
end

function GamePacketSender:sendExchangeRate(player)
    local params = {
        ExchangeRate = GameConfig.ExchangeRate
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(player.rakssid, "ExchangeRate", data)
end

function GamePacketSender:sendRankKey(player, key)
    PacketSender:sendLuaCommonData(player.rakssid, "UpdateRankKey", key)
end

function GamePacketSender:sendUpgradeInfo(player, rewards, beforeLv, nowLv)
    local params = {
        rewards = json.encode(rewards),
        beforeLv = beforeLv,
        nowLv = nowLv
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(player.rakssid, "UpgradeInfo", data)
end

function GamePacketSender:sendTeamAreaCountdown(player, areaId, time)
    local params = {
        areaId = areaId,
        time = time
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(player.rakssid, "Countdown", data)
end

function GamePacketSender:sendTeamAreaPlayerNum(player, areaId, playerNum)
    local params = {
        areaId = areaId,
        playerNum = playerNum
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(player.rakssid, "PlayerNum", data)
end