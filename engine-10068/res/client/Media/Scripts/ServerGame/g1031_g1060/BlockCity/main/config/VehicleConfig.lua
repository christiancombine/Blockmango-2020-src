VehicleConfig = {}
VehicleConfig.vehicles = {}
VehicleConfig.vehicleById = {}
VehicleConfig.RaceVehicleInfo = {
    StartLine = {},
    RaceVehicleType = 0,
    EndLine = {},
}

VehicleConfig.checkDis = {} --用于赛车前检测在起跑位向前多少个位置存在entity

checkWin = setmetatable(
        { --0-270 为yaw
            [0] = true,
            [90] = false,
            [180] = false,
            [270] = true,
        }, {
            __mul = function(checkWin, tab)--tab = {true or false, yaw}
                if tab[1] and checkWin[tab[2]] then
                    return true
                elseif tab[1] and not checkWin[tab[2]] or not tab[1] and checkWin[tab[2]] then
                    return false
                elseif not tab[1] and not checkWin[tab[2]] then
                    return true
                else
                    return false
                end
            end
        })

--车的种类(6: 警车 7:红色法拉利  3:黑车  9:越野 )
VehicleType = {
    POLICE = 6,
    SPORTSCAR = 7,
    BLACKCAR = 3,
    OFFROADCAR = 9,
}

RaceFailType = {
    notFreeLines = 1,     --没有空闲跑道
    targetWaitAns = 2,    --对方在等待竞赛
    iWaitAns = 3,         --我要等待被邀请玩家的回复
    iHaveNotAns = 4,      --我要回复邀请者
}

VehicleConfig.RaceVehicle = {}

function VehicleConfig:init(config)
    for _, v in pairs(config) do
        local vehicle = {}
        vehicle.type = tonumber(v.type)
        vehicle.pos = VectorUtil.newVector3(v.x, v.y, v.z)
        vehicle.yaw = tonumber(v.yaw)
        table.insert(self.vehicles, vehicle)
    end
end

function VehicleConfig:prepareVehicle()
    for i, vehicle in pairs(self.vehicles) do
        vehicle.entityId = EngineWorld:addVehicleNpc(vehicle.pos, vehicle.type, vehicle.yaw)
        self.vehicleById[vehicle.entityId] = vehicle
    end
end

function VehicleConfig:checkAndResetPosition()
    for _, vehicle in pairs(self.vehicles) do
        if EngineWorld:getWorld():hasPlayerByVehicle(vehicle.entityId) == false then
            EngineWorld:getWorld():resetVehiclePositionAndYaw(vehicle.entityId, vehicle.pos, vehicle.yaw)
        end
    end
end

function VehicleConfig:getVehicleTypeByEntityId(entityId)
    entityId = tonumber(entityId)
    for _, v in pairs(self.vehicles) do
        if v.entityId == entityId then
            return v.type
        end
    end

    return 0
end

function VehicleConfig:initRaceVehicleInfo(infos)

    local fd = tonumber(infos.forwardDis) --forward distance 向前检测多少个个位置是否存在entity
    for yaw = 0, 270, 90 do
        self.checkDis[yaw] = { -- x1 < x2 , z1 < z2
            x1 = (yaw == 90 and {-fd} or {-2})[1],
            x2 = (yaw == 270 and {fd} or {2})[1],
            z1 = (yaw == 180 and {-fd} or {-2})[1],
            z2 = (yaw == 0 and {fd} or {2})[1],
        }
    end

    local raceInfo = self.RaceVehicleInfo
    local ep = infos.EndLine.pos
    for i, val in pairs(infos.StartLine) do
        local v = val.posAndYaw
        local data = {}
        data.pos = VectorUtil.newVector3(v[1], v[2], v[3])
        data.yaw = v[4]
        raceInfo.StartLine[i] = data
    end

    for i=1, 3 do
        ep[i], ep[i+3] = ShopHouseConfig:sortDataASC(ep[i], ep[i+3])
        --pos=[x1, y1, z1, x2, y2, z2, 冲线朝向(0, 90, 180, 270)]
    end
    raceInfo.EndLine = {
        Start = VectorUtil.newVector3(ep[1], ep[2], ep[3]),
        End = VectorUtil.newVector3(ep[4], ep[5], ep[6]),
        Yaw = tonumber(ep[7]) -- 冲线朝向
    }

    raceInfo.RaceVehicleType = tonumber(infos.raceVehicleType)
end

function VehicleConfig:checkFreeRaceLines()
    local raceInfo = self.RaceVehicleInfo
    local RaceLine = {}
    for _, v in pairs(raceInfo.StartLine) do
        local cDis = self.checkDis[tonumber(v.yaw)]
        local x, y, z = v.pos.x, v.pos.y, v.pos.z
        local startPos = VectorUtil.newVector3(x+cDis["x1"], y-2, z+cDis["z1"])
        local endPos = VectorUtil.newVector3(x+cDis["x2"], y+2, z+cDis["z2"])

        local tab = EngineWorld:getWorld():hasEntitiesInPos(startPos, endPos)
        if not next(tab) then
            table.insert(RaceLine, v)

        else
            local len = 0
            local world = EngineWorld:getWorld()
            for _, entityId in pairs(tab) do
                if self.RaceVehicle[entityId] and world:hasPlayerByVehicle(entityId) == false then
                    len = len + 1
                    EngineWorld:removeEntity(entityId)
                    self.RaceVehicle[entityId] = nil

                elseif self.vehicleById[entityId] and world:hasPlayerByVehicle(entityId) == false then
                    len = len + 1
                    local vehicle = self.vehicleById[entityId]
                    world:resetVehiclePositionAndYaw(vehicle.entityId, vehicle.pos, vehicle.yaw)

                end
            end
            if len == #tab then
                table.insert(RaceLine, v)
            end
        end

        if #RaceLine == 2 then
            return RaceLine
        end
    end
    return nil
end

function VehicleConfig:prepareRaceVehicle(pos, yaw)
    local entityId = EngineWorld:addVehicleNpc(pos, self.RaceVehicleInfo.RaceVehicleType, yaw)
    self.RaceVehicle[entityId] = os.time()
    return entityId
end

function VehicleConfig:checkAndRemoveRaceVehicle()
    if not next(self.RaceVehicle) then
        return
    end

    local idx = {}
    for entityId, cTime in pairs(self.RaceVehicle) do
        if os.time() - cTime >= 60 and EngineWorld:getWorld():hasPlayerByVehicle(entityId) == false then
            EngineWorld:removeEntity(entityId)
            table.insert(idx, entityId)
        end
    end
    for _, entityId in pairs(idx) do
        self.RaceVehicle[entityId] = nil
    end
end

function VehicleConfig:checkPlayerFinishRace(pos)
    local eps = self.RaceVehicleInfo.EndLine.Start
    local epe = self.RaceVehicleInfo.EndLine.End
    return pos.x >= eps.x and pos.x <= epe.x and
            pos.y >= eps.y and pos.y <= epe.y and
            pos.z >= eps.z and pos.z <= epe.z
end

function VehicleConfig:sendToRaceOrNotPacket(rakssid, vEntityId, competitorUId, isAgree, competitorName)
    local packet = DataBuilder.new():addParam("vehicle_entityId", vEntityId):addParam("isAgree", isAgree):addParam("competitorUId", competitorUId):addParam("name", competitorName):getData()
    PacketSender:sendServerCommonData(rakssid, "syncBlockCityRaceToGetOnCarOrNot", "", packet)
end

function VehicleConfig:sendRaceFailMsgPacket(rakssid, showType, targetOrInvitorName)
    local packet = DataBuilder.new():addParam("showType", showType):addParam("name", targetOrInvitorName):getData()
    PacketSender:sendServerCommonData(rakssid, "showBlockCityRaceFailMas", "", packet)
end

return VehicleConfig