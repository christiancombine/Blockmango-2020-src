PreInstallConfig = {}
PreInstallConfig.data = {}

function PreInstallConfig:init(config)
    for _, v in pairs(config) do
        local data = {}
        data.order = tonumber(v.order)
        data.id = tonumber(v.id)
        data.rPos = VectorUtil.newVector3(v.x, v.y, v.z)
        data.rYaw = tonumber(v.yaw)
        self.data[data.order] = data
    end
end

function PreInstallConfig:preparePreInstall(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end
    for _, v in pairs(self.data) do
        local itemInfo = StoreConfig:getItemInfoById(v.id)
        if not itemInfo then
            return false
        end
        if itemInfo.type == BuildingType.Fort or itemInfo.type == BuildingType.Wall then
            local rEndPos = VectorUtil.newVector3(v.rPos.x + itemInfo.length - 1, v.rPos.y + itemInfo.height, v.rPos.z + itemInfo.width - 1)
            local aStartPos = ManorConfig:getPosByOffset(player.manorIndex, v.rPos, true)
            local aEndPos = ManorConfig:getPosByOffset(player.manorIndex, rEndPos, true)
            local minPos = ManorConfig:sortPos(aStartPos, aEndPos, true)
            local yaw = ManorConfig:getYawByOffsetYaw(player.manorIndex, v.rYaw)
            SceneManager:createOrDestroyHouse(itemInfo.fileName, true, minPos, yaw, 0)

            local minOffset, maxOffset = ManorConfig:sortPos(v.rPos, rEndPos, true)
            player.schematic:addSchematic(itemInfo.id, minOffset, maxOffset, v.rYaw)

        elseif itemInfo.type == BuildingType.Turret then
            local orientation = ManorConfig:getOrientationByIndex(player.manorIndex)
            local offset = v.rPos
            local actorId = v.id

            -- 解决放置装饰时不同朝向产生的偏移
            if orientation == 90 then
                offset.x = offset.x + 1
            end
            if orientation == 270 then
                offset.z = offset.z + 1
            end

            local newPos = ManorConfig:getPosByOffset(player.manorIndex, offset, false)
            local yaw = ManorConfig:getYawByOffsetYaw(player.manorIndex, 0)

            local entityId = EngineWorld:getWorld():addDecoration(newPos, yaw, player.userId, actorId)
            player.decoration:addDecoration(entityId, actorId, offset, 0)
        end
    end
end