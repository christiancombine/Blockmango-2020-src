GamePacketSender = {}

function GamePacketSender:sendTowerDefenseRoundInfo(rakssid, round, allRound, prepareTime, needSyn)
    local params = {
        round = round,
        allRound = allRound,
        prepareTime = prepareTime,
        needSyn = needSyn
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "RoundInfo", data)
end

function GamePacketSender:sendTowerDefenseHomeHp(rakssid, initHp, hp)
    local params = {
        initHp = initHp,
        homeHP = hp
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "HomeHp", data)
end

function GamePacketSender:sendTowerDefenseSettlement(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "Settlement", json.encode(data))
end

function GamePacketSender:sendTowerCanBuildList(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "BuildList", json.encode(data))
end

function GamePacketSender:sendTowerPaySkillList(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "PaySkillList", json.encode(data))
end

function GamePacketSender:sendTowerDefenseRoundPassTip(rakssid, oldRound, newRound, reward, isWarning)
    local params = {
        oldRound = oldRound,
        newRound = newRound,
        reward = reward,
        isWarning = isWarning
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "RoundPassTip", data)
end

function GamePacketSender:sendTowerDefenseWaitTip(rakssid, type, countdown, currentTeammateNum, allTeammateNum)
    local params = {
        countdown = countdown or 0,
        currentTeammateNum = currentTeammateNum or 0,
        allTeammateNum = allTeammateNum or 0,
        type = type or Define.WaitMessageType.Close
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "WaitTip", data)
end

function GamePacketSender:sendTowerDefensePlayerList(rakssid, userId, name)
    local params = {
        userId = tostring(userId),
        name = name,
    }
    PacketSender:sendLuaCommonData(rakssid, "PlayerList", json.encode(params))
end

function GamePacketSender:sendTowerDefenseBroadcastInfo(rakssid, text)
    local params = {
        text = text
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "Broadcast", data)
end

function GamePacketSender:sendTowerDefenseUsePropInfo(rakssid, playerName, propStr)
    local params = {
        playerName = playerName,
        propStr = propStr,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "UseProp", data)
end

function GamePacketSender:sendTowerDefenseReward(rakssid, round, time, reward, boss, loginReward, allRound, checkPoint)
    local params = {
        round = round,
        time = time,
        reward = reward,
        boss = boss,
        loginReward = loginReward,
        allRound = allRound,
        checkPoint = checkPoint
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "Reward", data)
end

function GamePacketSender:sendTowerDefensePlayAgainStatus(rakssid, agreedNum, totalNum)
    local params = {
        agreedNum = agreedNum,
        totalNum = totalNum,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "PlayAgainStatus", data)
end

function GamePacketSender:sendTowerDefensePlayAgain(rakssid)
    PacketSender:sendLuaCommonData(rakssid, "PlayAgain")
end

function GamePacketSender:sendTowerDefenseProcessId(rakssid, processId)
    local params = {
        processId = processId
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "ProcessId", data)
end

function GamePacketSender:sendTowerDefenseCanPlaceBlockId(rakssid, blockId)
    local params = {
        blockId = blockId
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "CanPlaceBlockId", data)
end

function GamePacketSender:sendTowerDefensePlayerTowerList(rakssid, towerList)
    local data = {}
    for _, tower in pairs(towerList) do
        table.insert(data, tower)
    end
    PacketSender:sendLuaCommonData(rakssid, "TowerList", json.encode(data))
end

function GamePacketSender:sendTowerDefenseGetNewTowerBook(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "GetNewTowerBook", json.encode(data))
end

function GamePacketSender:sendBuySkillSuccess(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "BuySkillSuccess", json.encode(data))
end

function GamePacketSender:sendUseProps(rakssid, propsId, userId)
    local params = {
        propsId = propsId,
        userId = userId
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "UseProps", data)
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

function GamePacketSender:sendShowScreenEffect(rakssid, propsId, isShow)
    local params = {
        propsId = propsId,
        isShow = isShow
    }
    PacketSender:sendLuaCommonData(rakssid, "ShowScreenEffect", json.encode(params))
end

function GamePacketSender:sendExchangeRate(player)
    local params = {
        ExchangeRate = GameConfig.ExchangeRate
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(player.rakssid, "ExchangeRate", data)
end

function GamePacketSender:sendGameGoldExchangeRate(player, exchangeLimit)
    local params = {
        GGoldExchangeRate = GameConfig.GameGoldExchangeRate,
        exchangeLimit = exchangeLimit,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(player.rakssid, "GGExchangeRate", data)
end

function GamePacketSender:sendPayItemList(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "PayItemList", json.encode(data))
end

function GamePacketSender:sendBuyPayItemSuccess(rakssid, itemId, num)
    local params = {
        itemId = itemId,
        num = num,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "BuyPayItemSuccess", data)
end

function GamePacketSender:sendEquipSkin(player)
    local params = {
        equipSkinId = player.equipSkinId
    }
    PacketSender:sendLuaCommonData(player.rakssid, "EquipSkin", json.encode(params))
end

function GamePacketSender:sendShortCutBarSetting(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "ShortCutBarSetting", json.encode(data))
end