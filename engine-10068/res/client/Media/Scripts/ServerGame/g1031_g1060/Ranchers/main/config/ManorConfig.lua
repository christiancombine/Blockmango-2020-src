require "config.Area"

ManorConfig = {}
ManorConfig.manor = {}
ManorConfig.serviceCenter = {}
ManorConfig.serviceCenter.gate = {}
ManorConfig.fieldInitPos = {}
ManorConfig.roadsInitPos = {}
ManorConfig.initFieldNum = 0

function ManorConfig:initManor(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.manorStartPosX = tonumber(v.manorStartPosX)
        data.manorStartPosY = tonumber(v.manorStartPosY)
        data.manorStartPosZ = tonumber(v.manorStartPosZ)
        data.manorEndPosX = tonumber(v.manorEndPosX)
        data.manorEndPosY = tonumber(v.manorEndPosY)
        data.manorEndPosZ = tonumber(v.manorEndPosZ)

        data.gateStartPosX = tonumber(v.gateStartPosX)
        data.gateStartPosY = tonumber(v.gateStartPosY)
        data.gateStartPosZ = tonumber(v.gateStartPosZ)
        data.gateEndPosX = tonumber(v.gateEndPosX)
        data.gateEndPosY = tonumber(v.gateEndPosY)
        data.gateEndPosZ = tonumber(v.gateEndPosZ)

        data.initPosX = tonumber(v.initPosX)
        data.initPosY = tonumber(v.initPosY)
        data.initPosZ = tonumber(v.initPosZ)

        data.effectPosX = tonumber(v.effectPosX)
        data.effectPosY = tonumber(v.effectPosY)
        data.effectPosZ = tonumber(v.effectPosZ)

        self.manor[data.id] = data
    end
end

function ManorConfig:initServiceCenter(config)
    self:initGate(config)
end

function ManorConfig:initGate(config)
    for i, v in pairs(config.gate) do
        local gate = {}
        gate.pos = Area.new(v.pos)
        gate.initPos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        gate.yaw = tonumber(v.initPos[4])
        self.serviceCenter.gate[i] = gate
    end
end

function ManorConfig:initFieldPos(config)
    for i, v in pairs(config) do
        local data = {}
        data.pos = VectorUtil.newVector3(v.pos[1], v.pos[2], v.pos[3])
        self.fieldInitPos[i] = data
        self.initFieldNum = self.initFieldNum + 1
    end
end

function ManorConfig:initRoadsPos(config)
    for i, v in pairs(config) do
        local data = {}
        data.pos = VectorUtil.newVector3(v.pos[1], v.pos[2], v.pos[3])
        self.roadsInitPos[i] = data
    end
end

function ManorConfig:prepareGate()
    for i, v in pairs(self.serviceCenter.gate) do
        EngineWorld:addActorNpc(v.initPos, v.yaw, "ranchers_door.actor", function(entity)
            entity:setCanCollided(false)
        end)
    end

    for i, v in pairs(self.manor) do
        local pos = VectorUtil.newVector3(v.effectPosX, v.effectPosY, v.effectPosZ)
        EngineWorld:addActorNpc(pos, 0, "ranchers_door.actor", function(entity)
            entity:setCanCollided(false)
        end)
    end
end

function ManorConfig:getInitPosByManorIndex(index)
    if self.manor[index] == nil then
        HostApi.log("===LuaLog: [error] ManorConfig:getInitPosByManorIndex: manor[" .. index .. "] = nil")
        return nil
    end

    local manor = self.manor[index]
    return VectorUtil.newVector3(manor.initPosX, manor.initPosY, manor.initPosZ)
end

function ManorConfig:getStartPosByManorIndex(index)
    if self.manor[index] == nil then
        HostApi.log("===LuaLog: [error] ManorConfig:getManorStartPosByManorIndex: manor[" .. index .. "] = nil")
        return nil
    end

    local manor = self.manor[index]
    return VectorUtil.newVector3i(manor.manorStartPosX, manor.manorStartPosY, manor.manorStartPosZ)
end

function ManorConfig:getEndPosByManorIndex(index)
    if self.manor[index] == nil then
        HostApi.log("===LuaLog: [error] ManorConfig:getEndPosByManorIndex: manor[" .. index .. "] = nil")
        return nil
    end
    local manor = self.manor[index]
    return VectorUtil.newVector3i(manor.manorEndPosX, manor.manorEndPosY, manor.manorEndPosZ)
end

function ManorConfig:getGatePosByManorIndex(index)
    local gatePos = {}
    if self.manor[index] == nil then
        gatePos[1] = {}
        gatePos[1][1] = 0
        gatePos[1][2] = 0
        gatePos[1][3] = 0
        gatePos[2] = {}
        gatePos[2][1] = 0
        gatePos[2][2] = 0
        gatePos[2][3] = 0
        HostApi.log("===LuaLog: [error] ManorConfig:getGatePosByManorIndex: manor[" .. index .. "] = nil")
        return Area.new(gatePos)
    end

    local manor = self.manor[index]
    gatePos[1] = {}
    gatePos[1][1] = manor.gateStartPosX
    gatePos[1][2] = manor.gateStartPosY
    gatePos[1][3] = manor.gateStartPosZ
    gatePos[2] = {}
    gatePos[2][1] = manor.gateEndPosX
    gatePos[2][2] = manor.gateEndPosY
    gatePos[2][3] = manor.gateEndPosZ
    return Area.new(gatePos)
end

function ManorConfig:serviceCenterGateTeleport(player, x, y, z)
    for i, v in pairs(ManorConfig.serviceCenter.gate) do
        if v.pos:inArea(VectorUtil.newVector3(x, y, z)) == true then
            if player.isInServiceGate == false then
                player.isInServiceGate = true
                if player.manorIndex == 0 then
                    local userId = player.userId
                    RanchersWeb:checkHasLand(player.userId, function (data)
                        local callBackPlayer = PlayerManager:getPlayerByUserId(userId)
                        if callBackPlayer == nil then
                            return false
                        end
                        if data ~= nil then
                            HostApi.log(" === sendCallOnManorResetClient == ")
                            HostApi.sendCallOnManorResetClient(callBackPlayer.rakssid, callBackPlayer.userId)
                        else
                            HostApi.sendCommonTip(callBackPlayer.rakssid, "ranch_tip_need_get_manor")
                        end
                    end)
                    return true
                else
                    local manor = self.manor[player.manorIndex]
                    if manor ~= nil then
                        MsgSender.sendCenterTipsToTarget(player.rakssid, 5, Messages:welcomeManor(player.manor.name))
                        player:teleportPos(VectorUtil.newVector3i(manor.initPosX, manor.initPosY, manor.initPosZ))
                    end
                    return true
                end
            end
        else
            player.isInServiceGate = false
        end
    end
end

function ManorConfig:manorGateTeleport(player, x, y, z)
    local manorIndex = 0
    local manorName = "Rancher"
    if player.manorIndex ~= 0 then
        -- master
        manorIndex = player.manorIndex
        if player.targetManorIndex ~= 0 then
            manorIndex = player.targetManorIndex
            manorName = player.name
        end
    else
        -- visitor
        local targetUserId = SceneManager:getTargetByUid(player.userId)
        if targetUserId ~= nil then
            local manorInfo = SceneManager:getUserManorInfoByUid(targetUserId)
            if manorInfo ~= nil then
                manorIndex = manorInfo.manorIndex
                manorName = manorInfo.name
            end
        end
    end

    if manorIndex ~= nil and manorIndex ~= 0 then
        if ManorConfig:getGatePosByManorIndex(manorIndex):inArea(VectorUtil.newVector3(x, y, z)) == true then
            if player.isInManorGate == false then
                player.isInManorGate = true
                if player.isMaster then
                    if player.manorIndex == manorIndex then
                        -- go to the service center init pos if player is master
                        player:teleportPos(GameConfig.initPos)
                    else
                        -- player is master, and his manor is in the same server of target user's
                        local vec3 = ManorConfig:getInitPosByManorIndex(player.manorIndex)
                        if vec3 ~= nil then
                            player:teleportPos(vec3)
                            player.targetManorIndex = player.manorIndex
                            SceneManager:initCallOnTarget(player.userId, player.userId)
                            MsgSender.sendCenterTipsToTarget(player.rakssid, 5, Messages:welcomeManor(manorName))
                        else
                            player:teleportPos(GameConfig.initPos)
                        end
                    end
                else
                    HostApi.log(" === sendCallOnManorResetClient == ")
                    HostApi.sendCallOnManorResetClient(player.rakssid, player.userId)
                end

                return true
            end
        else
            player.isInManorGate = false
        end
    end
end

function ManorConfig:getVec3iByOffset(manorIndex, offset)
    local startPos = self:getStartPosByManorIndex(manorIndex)
    if startPos == nil then
        return nil
    end

    local vec3 = {}
    vec3.x = startPos.x + offset.x
    vec3.y = startPos.y + offset.y
    vec3.z = startPos.z + offset.z

    return VectorUtil.newVector3i(vec3.x, vec3.y, vec3.z)
end

function ManorConfig:getVec3ByOffset(manorIndex, offset)
    local startPos = self:getStartPosByManorIndex(manorIndex)
    if startPos == nil then
        return nil
    end

    local vec3 = {}
    vec3.x = startPos.x + offset.x
    vec3.y = startPos.y + offset.y
    vec3.z = startPos.z + offset.z

    return VectorUtil.newVector3(vec3.x, vec3.y, vec3.z)
end

function ManorConfig:getOffset3iByVec3(manorIndex, vec)
    local startPos = self:getStartPosByManorIndex(manorIndex)
    if startPos == nil then
        return nil
    end

    local offset = {}
    offset.x = vec.x - startPos.x
    offset.y = vec.y - startPos.y
    offset.z = vec.z - startPos.z

    return VectorUtil.newVector3i(offset.x, offset.y, offset.z)
end

function ManorConfig:getOffset3ByVec3(manorIndex, vec)
    local startPos = self:getStartPosByManorIndex(manorIndex)
    if startPos == nil then
        return nil
    end

    local offset = {}
    offset.x = vec.x - startPos.x
    offset.y = vec.y - startPos.y
    offset.z = vec.z - startPos.z

    return VectorUtil.newVector3(offset.x, offset.y, offset.z)
end

return ManorConfig