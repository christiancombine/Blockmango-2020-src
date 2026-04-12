SkyBlockWeb = {}

----PartyGet
SkyBlockWeb.friendPartyListApi = "/gameaide/api/v1/inner/friend/game/party/list/{gameId}/{userId}"
SkyBlockWeb.allPartyListApi = "/gameaide/api/v1/inner/game/party/list/{gameId}"
SkyBlockWeb.partyInfoApi = "/gameaide/api/v1/inner/game/party/info/{gameId}/{userId}"
SkyBlockWeb.loveNumApi = "/gameaide/api/v1/inner/game/party/like/{gameId}/{userId}"
SkyBlockWeb.getFreeCreateApi = "/gameaide/api/v1/inner/game/party/freeTime/{gameId}/{userId}"
-----PartyPost
SkyBlockWeb.createPartyApi = "/gameaide/api/v1/inner/game/party/create"
SkyBlockWeb.applyPartyApi = "/gameaide/api/v1/inner/game/party/update"
SkyBlockWeb.giveLoveApi = "/gameaide/api/v1/inner/game/party/like"
SkyBlockWeb.addPlayerApi = "/gameaide/api/v1/inner/game/party/add"
SkyBlockWeb.reducePlayerApi = "/gameaide/api/v1/inner/game/party/reduce"
----BuildGet
SkyBlockWeb.buildMemberApi = "/gameaide/api/v1/inner/game/build/member"
SkyBlockWeb.buildGroupApi = "/gameaide/api/v1/inner/my/game/build"

function SkyBlockWeb:init()
    self.RewardAddr = WebService.ServerHttpHost
end

function SkyBlockWeb:getFriendPartyList(userId)
    print("web getFriendPartyList userId == " .. tostring(userId))
    local path = self.RewardAddr .. self.friendPartyListApi
    path = string.gsub(path, "{gameId}", GameMatch.gameType)
    path = string.gsub(path, "{userId}", tostring(userId))
    local params = {
        { "userId", tostring(userId) },
        { "gameId", GameMatch.gameType },
    }
    WebService.asyncGet(path, params, function(data, code)
        local player = PlayerManager:getPlayerByUserId(userId)
        if not player then
            return
        end
        print("onFriendPartyList code = " .. tostring(code) .. " userId = " .. tostring(userId))
        if code == 1 and data then
            GameParty:sendSkyBlockFrientPartyList(userId, data)
        end
    end)
end

function SkyBlockWeb:getAllPartyList(listType)
    print("web getAllPartyList listType == " .. tostring(listType))
    local path = self.RewardAddr .. self.allPartyListApi
    path = string.gsub(path, "{gameId}", GameMatch.gameType)
    local params = {
        { "type", listType },
        { "gameId", GameMatch.gameType },
    }
    WebService.asyncGet(path, params, function(data, code)
        if code == 1 and data then
            GameParty:sendSkyBlockPartyData(data, listType)
        end
    end)
end

function SkyBlockWeb:getPartyInfo(userId)
    print("web getPartyInfo userId == " .. tostring(userId))
    local path = self.RewardAddr .. self.partyInfoApi
    path = string.gsub(path, "{gameId}", GameMatch.gameType)
    path = string.gsub(path, "{userId}", tostring(userId))

    local params = {
        { "userId", tostring(userId) },
        { "gameId", GameMatch.gameType },
    }
    WebService.asyncGet(path, params, function(data, code)
        print("getPartyInfo code = " .. tostring(code) .. " userId = " .. tostring(userId))
        if code == 1 and data then
            GameParty:recivePartyinfo(data, userId)
        end
    end)
end

function SkyBlockWeb:getLoveNum(userId, targetUserId)
    print("web getLoveNum userId == " .. tostring(userId) .. " targetUserId==" .. tostring(targetUserId))
    local path = self.RewardAddr .. self.loveNumApi
    path = string.gsub(path, "{gameId}", GameMatch.gameType)
    path = string.gsub(path, "{userId}", tostring(targetUserId))

    local params = {
        { "userId", tostring(targetUserId) },
        { "gameId", GameMatch.gameType },
    }
    WebService.asyncGet(path, params, function(data, code)
        print("getLoveNum code = " .. tostring(code) .. " userId = " .. tostring(userId) .. " targetUserId==" .. tostring(targetUserId))
        if code == 1 and data then
            local player = PlayerManager:getPlayerByUserId(userId)
            if not player then
                return
            end
            player:updateLoveNum(data, targetUserId)
        end
    end)
end

function SkyBlockWeb:getFreeCreateNum(userId)
    print("web getFreeCreate userId == " .. tostring(userId))
    local path = self.RewardAddr .. self.getFreeCreateApi
    path = string.gsub(path, "{gameId}", GameMatch.gameType)
    path = string.gsub(path, "{userId}", tostring(userId))

    local params = {
        { "userId", tostring(userId) },
        { "gameId", GameMatch.gameType },
    }
    WebService.asyncGet(path, params, function(data, code)
        local player = PlayerManager:getPlayerByUserId(userId)
        if not player then
            return
        end
        print("getFreeCreateNum code = " .. tostring(code) .. " userId = " .. tostring(userId))
        if code == 1 and data then
            player:updatefreeCreateNum(data)
        end
    end)
end

function SkyBlockWeb:createParty(data)
    print("web createParty userId == " .. tostring(data.userId) .. " currency == " .. tostring(data.currency) .. " quantity == " .. tostring(data.quantity)
            .. "  time == " .. tostring(data.time) .. " maxPlayerNum ==" .. tostring(data.maxPlayerNum))
    local path = self.RewardAddr .. self.createPartyApi
    local requestBody = {
        currency = data.currency,
        gameId = GameMatch.gameType,
        maxPlayerNum = data.maxPlayerNum,
        partyDesc = tostring(data.partyDesc),
        partyName = tostring(data.partyName),
        quantity = data.quantity,
        time = data.time,
        userId = tostring(data.userId),
    }
    local index = data.index
    WebService.asyncPost(path, {}, requestBody, function(data, code)
        print("createParty code = " .. tostring(code) .. " data = " .. json.encode(requestBody))
        local player = PlayerManager:getPlayerByUserId(requestBody.userId)
        if not player then
            return
        end
        if code == 1 then
            if player.enableIndie then
                GameAnalytics:design(player.userId, 0, { "g1048create_party_suc_indie", index })
            else
                GameAnalytics:design(player.userId, 0, { "g1048create_party_suc", index })
            end
            GameMatch:createPartySuccess(requestBody.userId)
        end
    end)
end

function SkyBlockWeb:applyParty(userId, partyName, partyDesc)
    print("web applyParty userId == " .. tostring(userId))
    local path = self.RewardAddr .. self.applyPartyApi
    local data = {
        partyDesc = tostring(partyDesc),
        partyName = tostring(partyName),
        userId = tostring(userId),
        gameId = GameMatch.gameType,
    }
    WebService.asyncPost(path, {}, data, function(data, code)
        local player = PlayerManager:getPlayerByUserId(userId)
        if not player then
            return
        end
        print("onApplyParty code = " .. tostring(code) .. " userId = " .. tostring(userId))
        GameParty:getPartyInfo(userId)
    end)
end

function SkyBlockWeb:giveLove(userId, targetUserId)
    print("web giveLove userId == " .. tostring(userId) .. " targetUserId == " .. tostring(targetUserId))
    local path = self.RewardAddr .. self.giveLoveApi
    local params = {
        { "targetUserId", tostring(targetUserId) },
        { "userId", tostring(userId) },
        { "gameId", GameMatch.gameType },
    }
    WebService.asyncPost(path, params, "{}", function(data, code)
        local player = PlayerManager:getPlayerByUserId(userId)
        if not player then
            return
        end
        print("onGiveLove code = " .. tostring(code) .. " userId = " .. tostring(userId) .. " targetUserId = " .. tostring(targetUserId))
        if code == 1 then
            GameParty:getLoveNum(userId, targetUserId)
            player:giveLoveSuccess(targetUserId)
        end
    end)
end

function SkyBlockWeb:partyAddPlayer(userId, targetUserId)
    print("web partyAddPlayer userId == " .. tostring(userId) .. " targetUserId ==" .. tostring(targetUserId))
    local path = self.RewardAddr .. self.addPlayerApi
    local params = {
        { "targetUserId", tostring(targetUserId) },
        { "userId", tostring(userId) },
        { "gameId", GameMatch.gameType },
    }
    WebService.asyncPost(path, params, "{}", function(data, code)
        print("onPartyAddPlayer code = " .. tostring(code) .. " userId = " .. tostring(userId))
    end)
end

function SkyBlockWeb:partyReducePlayer(userId, targetUserId)
    print("web partyReducePlayer userId == " .. tostring(userId) .. " targetUserId == " .. tostring(targetUserId))
    local path = self.RewardAddr .. self.reducePlayerApi
    local params = {
        { "targetUserId", tostring(targetUserId) },
        { "userId", tostring(userId) },
        { "gameId", GameMatch.gameType },
    }
    WebService.asyncPost(path, params, "{}", function(data, code)
        print("onPartyReducePlayer code = " .. tostring(code) .. " userId = " .. tostring(userId))
    end)
end

function SkyBlockWeb:getbuildMember(userId, targetUserId, isShowTips)
    print("web getbuildMember userId == " .. tostring(userId) .. " targetUserId == " .. tostring(targetUserId))
    local path = self.RewardAddr .. self.buildMemberApi

    local params = {
        { "userId", tostring(targetUserId) },
        { "gameId", GameMatch.gameType },
    }
    WebService.asyncGet(path, params, function(data, code)
        local player = PlayerManager:getPlayerByUserId(userId)
        if not player then
            return
        end
        print("onGetBuildMember code = " .. tostring(code) .. " extra = " .. tostring(userId) .. ":" .. tostring(isShowTips))
        if code == 1 and data then
            GameMultiBuild:onBuildGroupData(userId, isShowTips, data)
        end
    end)
end

function SkyBlockWeb:getbuildGroup(userId, isShowTips)
    print("web getbuildGroup userId == " .. tostring(userId))
    local path = self.RewardAddr .. self.buildGroupApi

    local params = {
        { "userId", tostring(userId) },
        { "gameId", GameMatch.gameType },
    }
    WebService.asyncGet(path, params, function(data, code)
        local player = PlayerManager:getPlayerByUserId(userId)
        if not player then
            return
        end
        print("onGetBuildGroup code = " .. tostring(code) .. " mark = " .. tostring(userId) .. ":" .. tostring(isShowTips))
        if code == 1 and data then
            GameMultiBuild:onBuildGroupUserId(userId, isShowTips, data)
        end
    end)
end

