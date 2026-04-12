GamePacketSender = {}

function GamePacketSender:SendLiftingSimulatorShopRegion(rakssid, status)
    local params = {
        ShopStatus = status
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "ShopRegion", data)
end

function GamePacketSender:sendExchangeReward(rakssid, propsType, leftTime)
    local params = {
        propsType = propsType,
        leftTime = leftTime,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "ExchangeReward", data)
    print("GamePacketSender:sendExchangeReward", propsType, leftTime)
end

function GamePacketSender:SendLSPlayerMuscleMassChange(rakssid, maxMuscleMass, muscleMass)
    local params = {
        maxMp = maxMuscleMass,
        curMp = muscleMass,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "MuscleMass", data)
end

function GamePacketSender:SendLSPlayerHealthChange(rakssid, health, maxHealth)
    local params = {
        curHp = health,
        maxHp = maxHealth,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "Health", data)
end

function GamePacketSender:SendLSFitnessEquipChange(rakssid, curId, maxId)
    local params = {
        curId = curId,
        maxId = maxId,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "FitnessEquip", data)
end

function GamePacketSender:SendLSGeneChange(rakssid, geneId, maxMuscleMass)
    local params = {
        geneId = geneId,
        maxMuscleMass = maxMuscleMass,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "Gene", data)
end

function GamePacketSender:SendPlayerGoldChange(rakssid, gold)
    local params = {
        curGold = tostring(gold)
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "Gold", data)
    --print("GamePacketSender:SendPlayerGoldChange " .. tostring(rakssid) .. " " .. tostring(gold))
end

function GamePacketSender:SendPlayerIsAllowWorkout(rakssid, isAllow)
    local params = {
        isAllowWorkout = isAllow
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "isAllowWorkout", data)
end

function GamePacketSender:SendPlayerIsAllowAttack(rakssid, isAllow)
    local params = {
        isAllowAttack = isAllow
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "isAllowAttack", data)
end

function GamePacketSender:SendLiftingSimulatorMeRank(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "meRank", data)
end

function GamePacketSender:SendLiftingSimulatorAllOtherRank(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "allOtherRank", data)
end

function GamePacketSender:SendStayAreaInfo(rakssid, area, isEnter)
    local params = {
        areaType = area,
        isEnter = isEnter,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "areaType", data)
end

function GamePacketSender:SendAutoAreaInfo(userId, isEnter)
    local params = {
        userId = tostring(userId),
        isEnter = isEnter,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:broadCastLuaCommonData("AutoAreaInfo", data)
end

function GamePacketSender:SendShopFitnessEquip(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "ShopFitnessEquip", data)
end

function GamePacketSender:SendShopGene(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "ShopGene", data)
end

function GamePacketSender:SendShopAdvanced(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "ShopAdvanced", data)
end

function GamePacketSender:SendShopSkill(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "ShopSkill", data)
end

function GamePacketSender:SendJoinAdvanced(rakssid)
    PacketSender:sendLuaCommonData(rakssid, "JoinAdvanced", "")
end

function GamePacketSender:SendQuitAdvanced(rakssid)
    PacketSender:sendLuaCommonData(rakssid, "QuitAdvanced", "")
end

function GamePacketSender:SendStartAdvanced(rakssid, countdown, Id)
    local params = { countdown = countdown }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "StartAdvanced", data)

    Id = Id or 0
    ReportUtil.reportStartAdvanced(PlayerManager:getPlayerByRakssid(rakssid), Id)
end

function GamePacketSender:SendAdvancedResult(rakssid, isWin, Id)
    local params = { isWin = isWin }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "AdvancedResult", data)

    Id = Id or 0
    ReportUtil.reportAdvancedResult(PlayerManager:getPlayerByRakssid(rakssid), isWin, Id)
end

function GamePacketSender:SendEquipSkill(rakssid, id, position, isUnload)
    local params = {
        id = id,
        position = position,
        isUnload = isUnload
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "equipmentOrUnloadSkill", data)

end

function GamePacketSender:SendEquipSkin(rakssid, id, status)
    local params = {
        id = id,
        status = status,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "equipmentOrUnloadSkin", data)
end

function GamePacketSender:SendMemberUpArrow(rakssid, isVisible)
    local params = { isVisible = isVisible, }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "MemberUpArrow", data)
end

function GamePacketSender:SendCaptainUpArrow(rakssid, isVisible)
    local params = { isVisible = isVisible, }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "CaptainUpArrow", data)
end

function GamePacketSender:SendAllTeamInfo(rakssid, teamIds)
    PacketSender:sendLuaCommonData(rakssid, "AllTeamInfo", json.encode(teamIds))
end

function GamePacketSender:SendCreateTeam(rakssid, userId)
    PacketSender:sendLuaCommonData(rakssid, "CreateTeam", tostring(userId))
end

function GamePacketSender:SendDisbandTeam(rakssid, userId)
    PacketSender:sendLuaCommonData(rakssid, "DisbandTeam", tostring(userId))
end

function GamePacketSender:SendPlayerTeam(rakssid, userId)
    PacketSender:sendLuaCommonData(rakssid, "PlayerTeam", tostring(userId))
end

function GamePacketSender:SendBossHPInfo(rakssid, curHp, maxHp, quantity)
    local params = { curHp = curHp, maxHp = maxHp, quantity = quantity }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "BossHPInfo", data)
end

function GamePacketSender:SendBossCurHp(rakssid, curHp, entityId)
    local params = {
        curHp = curHp or 0,
        entityId = entityId or 0
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "BossCurHp", data)
end

function GamePacketSender:sendAutoWorkoutBack(rakssid, isVisible)
    local params = { isVisible = isVisible, }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "AutoWorkoutBack", data)
end

--function GamePacketSender:sendFloatNumber(rakssid, value)
--    local params = { damage = value, }
--    local data = DataBuilder.new():fromTable(params):getData()
--    PacketSender:broadCastLuaCommonData("ShowFloatNumber", data)
--end

function GamePacketSender:sendShopItemSelectStatus(rakssid, selectTable, selectItemId)
    local params = {
        selectTable = selectTable,
        selectItemId = selectItemId
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "ShopItemSelectStatus", data)
end

function GamePacketSender:sendAutoWorkAdInfo(rakssid, type, id, duration, waitTime, count)
    local params = { type = type, id = id, duration = duration, waitTime = waitTime, count = count }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "AutoWorkAd", data)
end


function GamePacketSender:sendEditionVisibleInfo(rakssid, isVisible)
    local params = { isVisible = isVisible }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "EditionVisibleInfo", data)
end

function GamePacketSender:sendNewGuideProgressInfo(rakssid, index)
    local params = { index = index }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "NewGuideProgressInfo", data)
end

function GamePacketSender:SendTokenShopFitnessEquip(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "TokenShopFitnessEquip", data)
end

function GamePacketSender:SendTokenShopGene(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "TokenShopGene", data)
end

function GamePacketSender:SendTokenShopSkill(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "TokenShopSkill", data)
end

function GamePacketSender:SendPlayerTokenNumChange(rakssid, token)
    local params = {
        token = tostring(token)
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "TokenNum", data)
end

function GamePacketSender:SendAddWallText(rakssid, text, index)
    PacketSender:sendLuaCommonData(rakssid, "AddWallText", text .. "," .. index)
end

function GamePacketSender:SendEntityRankCountdown(rakssid, time)
    local params = {
        time = time,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "EntityRankCountdown", data)
end

return GamePacketSender