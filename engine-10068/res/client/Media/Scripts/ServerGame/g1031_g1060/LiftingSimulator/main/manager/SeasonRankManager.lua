SeasonRankManager = {}

SeasonRankManager.championActors = {}
SeasonRankManager.curSeasonRankData = {}
SeasonRankManager.lastUpdateTime = 0

function SeasonRankManager:init()
    self:getCurSeasonInfo()
    self:getLastSeasonInfo()
end

function SeasonRankManager:getLastSeasonInfo()
    for _, setting in pairs(GameChampionConfig.settings) do
        local actor = GameChampion.new(setting)
        local lv = tonumber(setting.lv)
        self:addActor(actor, lv)
    end

    LiftingSimulatorWeb:GetLastRankChampion(function (info, code)
        if code == 1 and info then
            LogUtil.log("SeasonRankManager:getLastSeasonInfo suc" .. inspect.inspect(info))

            for lv, actor in pairs(self.championActors) do
                for _, v in pairs(info) do
                    if (lv - 1) == v.segment then
                        local userId = v.userId
                        LogUtil.log("LiftingSimulatorWeb:GetDecoration " .. tostring(userId))
                        LiftingSimulatorWeb:GetDecoration(userId, function (innerInfo, innerCode)
                           if innerCode == 1 and innerInfo then
                               self:changeActorInfo(v.segment, innerInfo)
                               LogUtil.log("LiftingSimulatorWeb:GetDecoration suc " .. " " .. tostring(userId) .. " " .. inspect.inspect(innerInfo))
                           else
                               LogUtil.log("LiftingSimulatorWeb:GetDecoration error " .. tostring(userId))
                           end
                        end)
                    end
                end
            end
        else
            LogUtil.log("SeasonRankManager:getLastSeasonInfo error")
        end
    end)
end

function SeasonRankManager:getCurSeasonInfo()
    self.lastUpdateTime = os.time()

    for _, vPlayer in pairs(PlayerManager:getPlayers()) do
        EntityRankManager:deleteEntityRank(vPlayer.rakssid)
    end

    for _, setting in pairs(GameChampionConfig.settings) do

        local lv = tonumber(setting.lv)
        LiftingSimulatorWeb:GetRankByLv(lv - 1, 0, 10, function (info, code)
            if code == 1 and info then
                LogUtil.log("SeasonRankManager:getCurSeasonInfo suc lv: " .. lv)
                --print_lua_table(info)
                LogUtil.log(string.format("getCurSeasonInfo %d %s", lv, inspect.inspect(info)))
                self.curSeasonRankData[lv] = info
                EntityRankManager:updateEntityRank(lv)
            else
                LogUtil.log("SeasonRankManager:getCurSeasonInfo error lv: " .. lv)
            end
        end)
    end

    --local index = 1
    --for _, vPlayer in pairs(PlayerManager:getPlayers()) do
    --    EntityRankManager:delayCreateEntityRank(vPlayer.rakssid, index * GameMatch.tickInterval)
    --    index = index + 1
    --end
end

function SeasonRankManager:onTick(ticks)
    if os.time() - self.lastUpdateTime >= GameConfig.seasonRankDataCd then
        self:getCurSeasonInfo()
    end
    LiftingSimulatorWeb:tryUploadLv()
end

function SeasonRankManager:getCountdownTime()
    return GameConfig.seasonRankDataCd - (os.time() - self.lastUpdateTime)
end

function SeasonRankManager:addActor(actor, lv)
    self.championActors[lv] = actor
end

function SeasonRankManager:getActor(segment)
    for lv, actor in pairs(self.championActors) do
        if (lv - 1) == segment then
            return actor
        end
    end
    return nil
end

function SeasonRankManager:getActors()
    return self.championActors
end

function SeasonRankManager:onSeaonOver(content)
    self:resetChampionActor()

    for _, setting in pairs(GameChampionConfig.settings) do
        local actor = GameChampion.new(setting)
        local lv = tonumber(setting.lv)
        self:addActor(actor, lv)
    end

    local info = json.decode(content)
    LogUtil.log("SeasonRankManager:onSeaonOver " .. inspect.inspect(info))

    for segment, userId in pairs(info) do
        LiftingSimulatorWeb:GetDecoration(userId, function (innerInfo, innerCode)
           if innerCode == 1 and innerInfo then
               self:changeActorInfo(tonumber(segment), innerInfo)
               LogUtil.log("SeasonRankManager:onSeaonOver GetDecoration suc " .. " " .. tostring(userId) .. " " .. inspect.inspect(innerInfo))
           else
               LogUtil.log("SeasonRankManager:onSeaonOver GetDecoration error " .. tostring(userId))
           end
        end)
    end

    --重置实体排行榜
    SeasonRankManager:getCurSeasonInfo()
end

function SeasonRankManager:changeActorInfo(lv, info)
    local gameChampion = self:getActor(lv)
    if gameChampion then
        local entity = EngineWorld:getEntity(gameChampion.entityId)
        local entityActorNpc = DynamicCastEntity.dynamicCastActorNpc(entity)

        if entity and entityActorNpc then

            local actorName = "g1055_blackman.actor"
            local headName = string.format("%s=%s", gameChampion.topInfo, info.nickName)
            -- girl
            if info.sex == 2 then
                actorName = "girl.actor"
            -- boy
            else
                actorName = "boy.actor"
            end

            LogUtil.log("SeasonRankManager:changeActorInfo" .. inspect.inspect(gameChampion))

            entityActorNpc:setHeadName(headName)
            entityActorNpc:setActorName(actorName)
            entity:setScale(gameChampion.scale)
            entityActorNpc:setHeight(gameChampion.height)

            local builder = DataBuilder.new()
            local bodyMulti = ""

            local decorationMap = { "custom_face" }
            for _, bodyName in pairs(decorationMap) do
                if info.decorationMap[bodyName] == nil then
                    info.decorationMap[bodyName] = "1"
                end
            end

            for bodyName, bodyId in pairs(info.decorationMap) do
                if tostring(bodyName) ~= "exclusive_parts" then
                    bodyMulti = bodyMulti .. tostring(bodyName) .. "," .. tostring(bodyId) .."|"
                end
            end
            builder:addParam("bodyMulti", bodyMulti)
            entityActorNpc:addCustomBodyPart("1", builder:getData())

            local data = DataBuilder.new()
                                :addParam("entityId", gameChampion.entityId)
                                :addParam("actorName", tostring(actorName))
                                :addParam("headNameMultiLeft", gameChampion.topInfo)
                                :addParam("headNameMultiRight", info.nickName)
                                :addParam("scale", gameChampion.scale)
                                :addParam("height", gameChampion.height)
                                :getData()

            PacketSender:broadCastServerCommonData("SyncEntityActorNpcInfo", "", data)
        end
    end
end

function SeasonRankManager:resetChampionActor()
    for _, actor in pairs(self.championActors) do
        EngineWorld:removeEntity(actor.entityId)
    end
    self.championActors = {}
end

return SeasonRankManager