local handler = class("GamePacketHandler", EventObservable)

Trigger = {}
Trigger.isFirstOpen = true

function handler:init()
    self:registerEvent(S2CPlayerMotionData, self.onPlayerMotionData)
    self:registerEvent(S2CLSOpenLottery, self.onOpenLottery)
    self:registerEvent(S2CMoveLimitCheck, self.onMoveLimitCheck)

    self:registerDataCallBack("KeyNum", function(keys)
        self.onKeyNum(keys)
    end)

    self:registerDataCallBack("LotteryResult", function(data)
        self.onLotteryResult(data)
    end, DataBuilderProcessor)

    self:registerDataCallBack("LotteryRefreshResult", function(data)
        self.onLotteryRefreshResult(data)
    end, DataBuilderProcessor)

    self:registerDataCallBack("playSound", function(data)
        local type = data:getNumberParam("type")
        SoundUtil.playSound(type)
    end, DataBuilderProcessor)

    self:registerDataCallBack("playSound3d", function(data)
        local type = data:getNumberParam("type")
        local x = data:getNumberParam("x")
        local y = data:getNumberParam("y")
        local z = data:getNumberParam("z")
        SoundUtil.playSound3D(type, x, y, z)
    end, DataBuilderProcessor)

    self:registerDataCallBack("ResourceUpgrade", function(pointId)
        self.onResourceUpgrade(pointId)
    end)
end

function handler.onKeyNum(keys)
    PlayerWallet:setMoneyCount(Define.Money.KEY, tonumber(keys))
end

function handler.onPlayerMotionData()
    local player = Game:getPlayer()
    local dest = player.fallDistance
    local totle = 0
    local motion = 0
    while (true) do
        motion = ((motion / 0.98) + 0.08) / 0.98
        totle = totle + motion
        if totle >= dest then
            break
        end
    end

    player.fallDistance = 0
    player.motion = VectorUtil.newVector3(0, motion, 0)
end

function handler.onLotteryResult(data)
    local rewardId = data:getNumberParam("rewardId")
    local price = data:getNumberParam("price")
    local seq = data:getNumberParam("seq")
    GUILottery:onGetReward(rewardId, price, seq)
end

function handler.onLotteryRefreshResult(data)
    local result = data:getParam("reward")
    local refreshPrice = data:getNumberParam("refreshPrice")
    local lotteryPrice = data:getNumberParam("lotteryPrice")
    GUILottery:onGetRefreshResult(result, refreshPrice, lotteryPrice)
end

function handler.onOpenLottery()
    if Trigger.isFirstOpen then
        GamePacketSender:sendBedWarLotteryRefresh()
        Trigger.isFirstOpen = false
    end
    GUIMain:onLotteryBtn()
end

function handler.onMoveLimitCheck(data)
    local builder = DataBuilder.new():fromData(data)
    ClientHelper.putIntPrefs("RunLimitCheck", builder:getNumberParam("runLimitCheck"))
    ClientHelper.putIntPrefs("SprintLimitCheck", builder:getNumberParam("sprintLimitCheck"))
end

function handler.onResourceUpgrade(pointId)
    if ResourcePointConfig == nil then
        LuaTimer:schedule(function()
            handler.onResourceUpgrade(pointId)
        end, 100)
        return
    end
    ResourcePointConfig:UpgradeResource(pointId)
end

handler:init()