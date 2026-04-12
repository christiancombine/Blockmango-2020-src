SceneManager = {}
SceneManager.userManorInfo = {}
SceneManager.userHouseInfo = {}
SceneManager.userFieldInfo = {}
SceneManager.userWareHouseInfo = {}
SceneManager.userBuildingInfo = {}
SceneManager.userTaskInfo = {}
SceneManager.userAchievementInfo = {}
SceneManager.userAccelerateItemsInfo = {}
SceneManager.callOnTarget = {}
SceneManager.errorTarget = {}

function SceneManager:initUserManorInfo(userId, manor)
    self.userManorInfo[tostring(userId)] = manor
end

function SceneManager:getUserManorInfoByUid(userId)
    if self.userManorInfo[tostring(userId)] ~= nil then
        return self.userManorInfo[tostring(userId)]
    end

    return nil
end

function SceneManager:deleteUserManorInfoByUid(userId)
    if self.userManorInfo[tostring(userId)] ~= nil then
        self.userManorInfo[tostring(userId)] = nil
    end
end

function SceneManager:initUserHouseInfo(userId, house)
    self.userHouseInfo[tostring(userId)] = house
end

function SceneManager:getUserHouseInfoByUid(userId)
    if self.userHouseInfo[tostring(userId)] ~= nil then
        return self.userHouseInfo[tostring(userId)]
    end

    return nil
end

function SceneManager:deleteUserHouseInfoByUid(userId)
    if self.userHouseInfo[tostring(userId)] ~= nil then
        self.userHouseInfo[tostring(userId)] = nil
    end
end

function SceneManager:initUserFieldInfo(userId, field)
    self.userFieldInfo[tostring(userId)] = field
end

function SceneManager:getUserFieldInfoByUid(userId)
    if self.userFieldInfo[tostring(userId)] ~= nil then
        return self.userFieldInfo[tostring(userId)]
    end

    return nil
end

function SceneManager:deleteUserFieldInfoByUid(userId)
    if self.userFieldInfo[tostring(userId)] ~= nil then
        self.userFieldInfo[tostring(userId)] = nil
    end
end

function SceneManager:initUserWareHouseInfo(userId, wareHouse)
    self.userWareHouseInfo[tostring(userId)] = wareHouse
end

function SceneManager:getUserWareHouseInfoByUid(userId)
    if self.userWareHouseInfo[tostring(userId)] ~= nil then
        return self.userWareHouseInfo[tostring(userId)]
    end

    return nil
end

function SceneManager:deleteUserWareHouseInfoByUid(userId)
    if self.userWareHouseInfo[tostring(userId)] ~= nil then
        self.userWareHouseInfo[tostring(userId)] = nil
    end
end

function SceneManager:initUserBuildingInfo(userId, building)
    self.userBuildingInfo[tostring(userId)] = building
end

function SceneManager:getUserBuildingInfoByUid(userId)
    if self.userBuildingInfo[tostring(userId)] ~= nil then
        return self.userBuildingInfo[tostring(userId)]
    end

    return nil
end

function SceneManager:deleteUserBuildingInfoByUid(userId)
    if self.userBuildingInfo[tostring(userId)] ~= nil then
        self.userBuildingInfo[tostring(userId)] = nil
    end
end

function SceneManager:initUserTaskInfo(userId, taskInfo)
    self.userTaskInfo[tostring(userId)] = taskInfo
end

function SceneManager:getUserTaskInfoByUid(userId)
    if self.userTaskInfo[tostring(userId)] ~= nil then
        return self.userTaskInfo[tostring(userId)]
    end

    return nil
end

function SceneManager:deleteUserTaskInfoByUid(userId)
    if self.userTaskInfo[tostring(userId)] ~= nil then
        self.userTaskInfo[tostring(userId)] = nil
    end
end

function SceneManager:initUserAchievementInfo(userId, achievementInfo)
    self.userAchievementInfo[tostring(userId)] = achievementInfo
end

function SceneManager:getUserAchievementInfoByUid(userId)
    if self.userAchievementInfo[tostring(userId)] ~= nil then
        return self.userAchievementInfo[tostring(userId)]
    end

    return nil
end

function SceneManager:deleteUserAchievementInfoByUid(userId)
    if self.userAchievementInfo[tostring(userId)] ~= nil then
        self.userAchievementInfo[tostring(userId)] = nil
    end
end

function SceneManager:initUserAccelerateItemsInfo(userId, accelerateItemsInfo)
    self.userAccelerateItemsInfo[tostring(userId)] = accelerateItemsInfo
end

function SceneManager:getUserAccelerateItemsInfoByUid(userId)
    if self.userAccelerateItemsInfo[tostring(userId)] ~= nil then
        return self.userAccelerateItemsInfo[tostring(userId)]
    end

    return nil
end

function SceneManager:deleteUserAccelerateItemsInfoByUid(userId)
    self.userAccelerateItemsInfo[tostring(userId)] = nil
end

function SceneManager:initCallOnTarget(userId, targetUserId)
    self.callOnTarget[tostring(userId)] = {}
    self.callOnTarget[tostring(userId)].userId = targetUserId
    self.callOnTarget[tostring(userId)].time = 0
end

function SceneManager:getTargetByUid(userId)
    if self.callOnTarget[tostring(userId)] ~= nil then
        return self.callOnTarget[tostring(userId)].userId
    end

    return nil
end

function SceneManager:getCallOnTarget()
    return self.callOnTarget
end

function SceneManager:deleteCallOnTargetByUid(userId)
    if self.callOnTarget[tostring(userId)] ~= nil then
        self.callOnTarget[tostring(userId)] = nil
    end
end

function SceneManager:initErrorTarget(userId)
    self.errorTarget[tostring(userId)] = {}
    self.errorTarget[tostring(userId)].userId = userId
    self.errorTarget[tostring(userId)].time = 0
end

function SceneManager:deleteErrorTargetByUid(userId)
    if self.errorTarget[tostring(userId)] ~= nil then
        self.errorTarget[tostring(userId)] = nil
    end
end

function SceneManager:initDataFromDB(userId, data, tag)
    if tag == DataBaseManager.TAG_PLAYER then
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            return false
        end
        if not data.__first then
            player:initDataFromDB(data)
        else
            player:addMoney(GameConfig.playerInitMoney)

            for i, v in pairs(PlayerInitItemsConfig.inventory) do
                player:addItem(v.id, v.num, 0)
            end

        end
        player:setPlayerRanchInfo()
        DataBaseManager:getDataFromDB(userId, DataBaseManager.TAG_ACCELERATE_ITEMS)
        DataBaseManager:getDataFromDB(userId, DataBaseManager.TAG_TASK)
        DataBaseManager:getDataFromDB(userId, DataBaseManager.TAG_ACHIEVEMENT)
    end

    if tag == DataBaseManager.TAG_MANOR then
        if self.userManorInfo[tostring(userId)] == nil then
            return false
        end
        if not data.__first then
            self.userManorInfo[tostring(userId)]:initDataFromDB(data)
        end
        DataBaseManager:getDataFromDB(userId, DataBaseManager.TAG_HOUSE)
        DataBaseManager:getDataFromDB(userId, DataBaseManager.TAG_FIELD)
        DataBaseManager:getDataFromDB(userId, DataBaseManager.TAG_ROADS)
        DataBaseManager:getDataFromDB(userId, DataBaseManager.TAG_WAREHOUSE)
        DataBaseManager:getDataFromDB(userId, DataBaseManager.TAG_BUILDING)
    end

    if tag == DataBaseManager.TAG_HOUSE then
        if self.userHouseInfo[tostring(userId)] == nil then
            return false
        end
        if not data.__first then
            self.userHouseInfo[tostring(userId)]:initDataFromDB(data)
        end

        self.userHouseInfo[tostring(userId)]:setHouseToWorld()
    end

    if tag == DataBaseManager.TAG_BUILDING then
        if self.userBuildingInfo[tostring(userId)] == nil then
            return false
        end

        if not data.__first then
            self.userBuildingInfo[tostring(userId)]:initDataFromDB(data, userId)
        else
            self.userBuildingInfo[tostring(userId)]:initFirstAnimalBuilding(userId)
        end
    end

    if tag == DataBaseManager.TAG_WAREHOUSE then
        if self.userWareHouseInfo[tostring(userId)] == nil then
            return false
        end

        if not data.__first then
            self.userWareHouseInfo[tostring(userId)]:initDataFromDB(data)
        else
            self.userWareHouseInfo[tostring(userId)]:addPlayerInitItems()
        end

        self.userWareHouseInfo[tostring(userId)]:setWareHousePos(userId)
    end

    if tag == DataBaseManager.TAG_FIELD then
        if self.userFieldInfo[tostring(userId)] == nil then
            return false
        end

        if not data.__first then
            self.userFieldInfo[tostring(userId)]:initDataFromDB(data, userId)
        else
            self.userFieldInfo[tostring(userId)]:initFieldPos()
        end
    end

    if tag == DataBaseManager.TAG_TASK then
        if self.userTaskInfo[tostring(userId)] == nil then
            return false
        end

        if not data.__first then
            self.userTaskInfo[tostring(userId)]:initDataFromDB(data)
        end

        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            return false
        end

        RanchersWeb:myHelpList(player.userId)
    end

    --if tag == DataBaseManager.TAG_AREA then

    --end

    if tag == DataBaseManager.TAG_ACHIEVEMENT then
        if self.userAchievementInfo[tostring(userId)] == nil then
            return false
        end

        if not data.__first then
            self.userAchievementInfo[tostring(userId)]:initDataFromDB(data)
        end
        DataBaseManager:getDataFromDB(userId, DataBaseManager.TAG_RANCHER_EXPLORE_INFO)
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            return false
        end

        local ranchAchievements = player.achievement:initRanchAchievements()
        player:getRanch():setAchievements(ranchAchievements)
    end

    if tag == DataBaseManager.TAG_RANCHER_EXPLORE_INFO then
        if self.userAchievementInfo[tostring(userId)] == nil then
            return false
        end

        if not data.__first then
            -- HostApi.log("DataBaseManager.TAG_RANCHER_EXPLORE_INFO: " .. data)
            if self.userAchievementInfo[tostring(userId)]:initRanchExploreFromDB(data) then
                -- 保存探险数据，等后续对数据处理之后，再修改数据内容
                if data.money then
                    -- HostApi.log("src_data.money" .. tonumber(src_data.money))
                    local cover_data = {}
                    cover_data.money = tonumber(data.money)
                    self.userAchievementInfo[tostring(userId)].exploreAchievementData = cover_data
                    self.userAchievementInfo[tostring(userId)].isHasExploreData = true
                end
            end 
        end
    end

    if tag == DataBaseManager.TAG_ACCELERATE_ITEMS then
        if self.userAccelerateItemsInfo[tostring(userId)] == nil then
            return false
        end
        if not data.__first then
            self.userAccelerateItemsInfo[tostring(userId)]:initDataFromDB(data)
        else
            self.userAccelerateItemsInfo[tostring(userId)]:initItems()
        end

        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            return false
        end

        local accelerateItemsInfo = player.accelerateItems:initRanchShortcutFree()
        player:getRanch():setShortcutFreeTimes(accelerateItemsInfo)
    end

    if tag == DataBaseManager.TAG_ROADS then
        if self.userFieldInfo[tostring(userId)] == nil then
            return false
        end

        if not data.__first then
            self.userFieldInfo[tostring(userId)]:initRoadsDataFromDB(data, userId)
        else
            self.userFieldInfo[tostring(userId)]:initRoadsPos()
        end
    end


end

function SceneManager:onTick(ticks)
    for i, v in pairs(self.callOnTarget) do
        local player = PlayerManager:getPlayerByUserId(tonumber(i))
        if player == nil then
            v.time = v.time + 1
        else
            v.time = 0
        end

        if v.time > 30 then
            self:deleteCallOnTargetByUid(tonumber(i))
        end
    end

    for i, v in pairs(self.errorTarget) do
        local player = PlayerManager:getPlayerByUserId(tonumber(i))
        if player == nil then
            HostApi.log(" errorTarget : userId[".. tostring(i) .."]  --  time[".. v.time .."] ")
            v.time = v.time + 1
        else
            v.time = 0
        end
        if v.time > 30 then
            self:deleteErrorTargetByUid(tonumber(i))
        end
    end
end

return SceneManager