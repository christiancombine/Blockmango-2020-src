GamePacketSender = {}


--发送职业奖池的数据
function GamePacketSender:sendJobsUpdateData(rakssid, data)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end
    PacketSender:sendLuaCommonData(rakssid, "jobListUpdate", data)
end

function GamePacketSender:sendHideJobButton(rakssid, isHide)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end
    local params = {
        isHide = isHide
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "hideJobButton", data)
end

--发送更新职业选择界面的数据(职业id,次数,)
function GamePacketSender:sendUpdateJobChoiceUIData(rakssid, data)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end
    PacketSender:sendLuaCommonData(rakssid, "UpdateJobs", data)
end


--从奖池随机出来的职业id传给客户端抽取界面
function GamePacketSender:sendSynJobsIdData(rakssid, id)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end
    local params = {
        id = id,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "synJobId", data)
end


--发送当前选择的职业id
function GamePacketSender:sendCurJobIdData(rakssid, id)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end
    local params = {
        id = id,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "CurJobId", data)
end

function GamePacketSender:sendLuckySkyEffectLimit(rakssid, time)
    local params = {
        time = time,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(rakssid, "PlayerInfoData", "LuckSkySetEffectLimit", data)
end

function GamePacketSender:sendLuckySkyBlockElasticity(rakssid, id, elasticity)
    local params = {
        id = id,
        elasticity = elasticity
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(rakssid, "BlockData", "BlockElasticity", data)
end

--多语言和时间
function GamePacketSender:sendLuckySkyCountDown(rakssid, text, time)
    local params = {
        ContDownTimeText = text,
        ContDownTime = time,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "CountDownData", data)
end

function GamePacketSender:sendLuckySkyMountHurt(entityId)
    local params = {
        entityId = entityId
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonDataToTrackingPlayers(entityId, "MountHurt", data)
end

function GamePacketSender:sendHideOrShowToolBar(rakssid, isShow)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end
    local params = {
        isShow = isShow
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "ToolBarData", data)
end

function GamePacketSender:sendLuckySkyPlayerInfo(rakssid, info)
    local data = DataBuilder.new():fromTable(info):getData()
    PacketSender:sendLuaCommonData(rakssid, "PlayerInfoData", data)
end

function GamePacketSender:sendLuckySkyMountInfo(rakssid, time)
    local params = { Time = time }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "MountInfoData", data)
end

function GamePacketSender:sendLuckySkyPlayerMotion(rakssid, motion)
    local data = DataBuilder.new():addVector3Param("Motion", motion):getData()
    PacketSender:sendLuaCommonData(rakssid, "PlayerMotionData", data)
end

function GamePacketSender:sendLuckySkyHoldHotPotato(rakssid, index)
    local params = {
        Index = index
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "HoldHotPotato", data)
end

function GamePacketSender:sendGeneralDeathTip(rakssid, name, tip, color)
    print("tip" .. tostring(tip))
    local params = {
        color = color,
        name = name,
        tipTxt = tip
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendServerCommonData(rakssid, "SkyWarLuckyBlockData", "LuckyBlockGeneralDeathTip", data)
end

function GamePacketSender:sendLuckySkyRopeDate(rakssid, time)
    local params = {
        Time = time,
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "SendRopeData", data)
end

function GamePacketSender:sendLuckySkyGameInfo(rakssid, info)
    local data = DataBuilder.new():fromTable(info):getData()
    PacketSender:sendLuaCommonData(rakssid, "GameInfoData", data)
end

function GamePacketSender:sendSetLuckySkyView(rakssid)
    PacketSender:sendLuaCommonData(rakssid, "SetView")
end

function GamePacketSender:sendDeadSummaryData(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "DeadSummaryData", data)
end

function GamePacketSender:sendGameSettlement(rakssid, data)
    PacketSender:sendLuaCommonData(rakssid, "GameSettlementData", data)
end

function GamePacketSender:sendUsePropData(rakssid, id, num)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end
    local params = {
        propId = id,
        num = num
    }
    local data = DataBuilder.new():fromTable(params):getData()
    PacketSender:sendLuaCommonData(rakssid, "sendUsePropData", data)
end

return GamePacketSender