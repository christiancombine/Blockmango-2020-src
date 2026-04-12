RanchersWeb = {}

RanchersWeb.askForHelpApi = "/gameaide/api/v1/inner/farm/asking-for-help"
RanchersWeb.finishHelpApi = "/gameaide/api/v1/inner/farm/finish-help"
RanchersWeb.finishOrderApi = "/gameaide/api/v1/inner/farm/finish-order"
RanchersWeb.updateOrderApi = "/gameaide/api/v1/inner/farm/update-order"
RanchersWeb.helpListApi = "/gameaide/api/v1/inner/farm/help-list"
RanchersWeb.helpListByIdApi = "/gameaide/api/v1/inner/farm/help-list/{id}"
RanchersWeb.sendInvitationApi = "/gameaide/api/v1/inner/farm/invite/send-invitation"
RanchersWeb.landApi = "/gameaide/api/v1/inner/farm/lands"
RanchersWeb.landByIdApi = "/gameaide/api/v1/inner/farm/lands/{userId}"
RanchersWeb.myHelpListApi = "/gameaide/api/v1/inner/farm/my-help-list"
RanchersWeb.giftSendApi = "/gameaide/api/v1/inner/game/gift/send"
RanchersWeb.clanProsperityApi = "/gameaide/api/v1/inner/farm/ranking-list/clan-prosperity"
RanchersWeb.myClanProsperityApi = "/gameaide/api/v1/inner/farm/ranking-list/my-clan"

function RanchersWeb:init()
    self.RewardAddr = WebService.ServerHttpHost
end

function RanchersWeb:askForHelp(userId, userName, uniqueId, boxAmount, fullBoxNum, orderId, boxId, count, itemId, itemLevel, rewards, isHot)
    local path = self.RewardAddr .. self.askForHelpApi
    local data = {
        boxAmount = boxAmount,
        fullBoxNum = fullBoxNum,
        uniqueId = tostring(uniqueId),
        boxId = boxId,
        count = count,
        gameType = "g1031",
        itemId = itemId,
        lv = itemLevel,
        orderId = tostring(orderId),
        userId = tostring(userId),
        userName = tostring(userName),
        rewards = rewards,
        isHot = isHot,
    }
    WebService.asyncPost(path, {}, data)
end

function RanchersWeb:finishHelp(userId, helperId, helperName, helperSex, uniqueId, id, callback)
    local path = self.RewardAddr .. self.finishHelpApi
    local data = {
        id = tostring(id), -- http服务器通过help-list 或 my-help-list 返回的 订单的需求栏id
        uniqueId = tostring(uniqueId),
        helperId = tostring(helperId),
        helperName = helperName,
        helperSex = helperSex
    }
    WebService.asyncPost(path, {}, data, function(data, code)
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            callback(nil)
            return
        end
        if code == 1 then
            callback(data)
        elseif code == 13 then
            callback(nil)
            HostApi.sendRanchOrderHelpResult(player.rakssid, "ranch_tip_somebody_already_finish_help")
            RanchersWeb:myHelpList(userId)
        else
            HostApi.log("[error] RanchersWeb:onFinishHelp: code = " .. code)
            callback(nil)
            HostApi.sendRanchOrderHelpResult(player.rakssid, "ranch_tip_something_error")
            RanchersWeb:myHelpList(userId)
            HostApi.log("[error] RanchersWeb:onFinishHelp:[" .. tostring(userId) .. "] filed to finish help code[" .. code .. "] ")
        end
    end)
end

function RanchersWeb:finishOrder(userId, uniqueId)
    local path = self.RewardAddr .. self.finishOrderApi
    local params = {
        { "uniqueId", tostring(uniqueId) }
    }
    WebService.asyncPost(path, params, "{}")
end

function RanchersWeb:updateOrder(userId, uniqueId, fullBoxNum)
    local path = self.RewardAddr .. self.updateOrderApi
    local data = {
        uniqueId = tostring(uniqueId),
        fullBoxNum = fullBoxNum,
    }
    WebService.asyncPost(path, {}, data)
end

function RanchersWeb:helpList(userIds, userId)
    local path = self.RewardAddr .. self.helpListApi
    local params = {
        { "userId", tostring(userId) },
        { "userIds", table.concat(userIds, ",") }
    }
    WebService.asyncGet(path, params)
end

function RanchersWeb:helpListById(userId, id, callback)
    local path = self.RewardAddr .. self.helpListByIdApi
    path = string.gsub(path, "{id}", tostring(id))
    WebService.asyncGet(path, {}, function(data, code)
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            callback(nil)
            return
        end
        if data ~= nil and data.id ~= nil then
            local item = {}
            item.id = data.id
            item.userId = data.userId
            item.uniqueId = data.uniqueId
            item.orderId = data.orderId
            item.boxId = data.boxId
            item.itemId = data.itemId
            item.itemCount = data.itemCount
            item.isHot = data.isHot
            item.helperId = data.helperId
            item.helperName = data.helperName
            item.helperSex = data.helperSex
            item.boxAmount = data.boxAmount
            item.fullBoxNum = data.fullBoxNum
            callback(item)
            return
        end
        callback(nil)
        HostApi.sendCommonTip(player.rakssid, "ranch_tip_something_error")
    end)
end

function RanchersWeb:myHelpList(userId, retryTimes)
    retryTimes = retryTimes or 3
    local path = self.RewardAddr .. self.myHelpListApi
    local params = {
        { "userId", tostring(userId) }
    }
    WebService.asyncGet(path, params, function(data, code)
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            return
        end
        if code ~= 1 then
            if retryTimes > 0 then
                self:myHelpList(userId, retryTimes - 1)
            end
            return
        end
        if data == nil then
            return
        end
        local items = {}
        for i, v in pairs(data) do
            if v.orderId ~= nil then
                local item = {}
                item.uniqueId = v.uniqueId
                item.askForHelpId = v.id
                item.userId = v.userId
                item.orderId = tonumber(v.orderId)
                item.boxId = v.boxId
                item.helperId = v.helperId
                item.helperName = v.helperName
                item.helperSex = v.helperSex
                item.itemId = tonumber(v.itemId)
                item.count = v.itemCount
                item.isHot = v.isHot
                item.boxAmount = v.boxAmount
                item.fullBoxNum = v.fullBoxNum
                items[i] = item
            end
        end

        if player.task ~= nil then
            player.task:updateTasksListFromHttp(items)
            local ranchTask = player.task:initRanchTask()
            player:getRanch():setOrders(ranchTask)
        end
    end)
end

function RanchersWeb:receiveLand(userId)
    local path = self.RewardAddr .. self.landApi
    local data = {
        userId = tostring(userId)
    }
    WebService.asyncPost(path, {}, data)
end

function RanchersWeb:checkHasLand(userId, callback, retryTimes)
    retryTimes = retryTimes or 3
    local path = self.RewardAddr .. self.landByIdApi
    path = string.gsub(path, "{userId}", tostring(userId))
    WebService.asyncGet(path, {}, function(data, code)
        if code == 1 then
            if data ~= nil and data.id ~= nil then
                callback(data)
            else
                callback(nil)
            end
        elseif code == 11 then
            callback(nil)
            HostApi.log("RanchersWeb:onCheckHasLand: code = " .. code)
        else
            HostApi.log("[Error] RanchersWeb:onCheckHasLand: code = " .. code)
            if retryTimes > 0 then
                self:checkHasLand(userId, callback, retryTimes - 1)
            else
                callback(nil)
            end
        end
    end)
end

function RanchersWeb:checkLandsInfo(userIds, callback)
    local path = self.RewardAddr .. self.landApi
    local params = {
        { "ids", table.concat(userIds, ",") }
    }
    WebService.asyncGet(path, params, function(data, code)
        if not data then
            return
        end
        callback(data)
    end)

end

function RanchersWeb:updateLandInfo(userId, exp, gift, level, prosperity)
    local path = self.RewardAddr .. self.landByIdApi
    path = string.gsub(path, "{userId}", tostring(userId))
    local data = {
        exp = exp,
        gift = gift,
        level = level,
        prosperity = prosperity,
    }
    WebService.asyncPost(path, {}, data, function(data, code)
        if data == nil then
            HostApi.log("=== LuaLog: [error] RanchersWeb:onUpdateLandInfo:[" .. tostring(userId) .. "] filed to update land info ... ")
        end
    end)
end

function RanchersWeb:giftSend(fromUser, fromTitle, fromContent, fromMailType, toUser, toTitle, toContent, mailType, gameType, callback)
    local path = self.RewardAddr .. self.giftSendApi

    local fromContent_json = json.encode(fromContent)
    local toContent_json = json.encode(toContent)

    local data = {
        fromUser = tostring(fromUser),
        fromTitle = fromTitle,
        fromContent = fromContent_json,
        fromMailType = fromMailType,
        toUser = tostring(toUser),
        title = toTitle,
        content = toContent_json,
        mailType = mailType,
        gameType = gameType
    }
    WebService.asyncPost(path, {}, data, function(data, code)
        if code ~= 1 then
            HostApi.log("=== [error]RanchersWeb:onGiftSend : code = " .. code)
        end
        if not callback then
            return
        end
        callback(data, code)
    end)
end

function RanchersWeb:sendInvitation(userId, targetUserId, playerName, callback)
    local path = self.RewardAddr .. self.sendInvitationApi
    local data = {
        gameType = "g1031",
        targetId = tostring(targetUserId),
        userId = tostring(userId),
        userName = tostring(playerName),
    }
    WebService.asyncPost(path, {}, data, function(data, code)
        callback(code)
    end)
end

function RanchersWeb:getClanProsperity(callback)
    local path = self.RewardAddr .. self.clanProsperityApi
    WebService.asyncGet(path, {}, function(data, code)
        if data ~= nil then
            local items = {}
            for i, v in pairs(data) do
                local item = {}
                item.clanId = 0
                item.rank = 0
                item.maxCount = 0
                item.userCount = 0
                item.prosperity = 0
                item.name = ""
                item.headPic = ""

                if v.clanId ~= nil then
                    item.clanId = v.clanId
                end

                if v.rank ~= nil then
                    item.rank = v.rank
                end

                if v.maxCount ~= nil then
                    item.maxCount = v.maxCount
                end

                if v.userCount ~= nil then
                    item.userCount = v.userCount
                end

                if v.prosperity ~= nil then
                    item.prosperity = v.prosperity
                end

                if v.headPic ~= nil then
                    item.headPic = v.headPic
                end

                if v.name ~= nil then
                    item.name = v.name
                end
                table.insert(items, item)
            end
            callback(items)
            return
        end
        callback(nil)
    end)
end

function RanchersWeb:getMyClanProsperity(userId, callback)
    local path = self.RewardAddr .. self.myClanProsperityApi
    local params = {
        { "userId", tostring(userId) }
    }
    WebService.asyncGet(path, params, callback)
end
