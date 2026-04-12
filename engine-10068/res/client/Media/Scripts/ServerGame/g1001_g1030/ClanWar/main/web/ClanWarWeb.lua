--region *.lua

ClanWarWeb = {}
ClanWarWeb.ver = nil
ClanWarWeb.RankAddr = nil
ClanWarWeb.RewardAddr = nil
ClanWarWeb.PropAddr = nil

ClanWarWeb.userAttrPath = "/minigame/i/api/{version}/user-details/{userId}"
ClanWarWeb.consumeAPI = "/props/api/{version}/consume/{userid}";
ClanWarWeb.rewardAPI = "/clans/i/api/{version}/clans/war/rewards";

function ClanWarWeb:init(version, rankAddr, rewardAddr, propAddr, gameId)
    self.ver = version
    self.RankAddr = rankAddr
    self.RewardAddr = rewardAddr
    self.PropAddr = propAddr
    self.gameId = gameId
end

function ClanWarWeb:GetReward(dataList, callback)
    local path = self.RewardAddr .. self.rewardAPI;
    path = string.gsub(path, "{version}", self.ver)
    local data = {}
    local uuid = require "base.util.uuid"
    uuid.randomseed(os.time())
    data.requestId = uuid()
    data.userdataList = dataList
    WebService.asyncPost(path, {}, data, function(data, code)
        callback(data)
    end)
end

--endregion
