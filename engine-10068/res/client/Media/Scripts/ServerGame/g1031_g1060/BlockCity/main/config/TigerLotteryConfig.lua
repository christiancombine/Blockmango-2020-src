TigerLotteryConfig = {}
TigerLotteryConfig.tigerLottery = {}

function TigerLotteryConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.actorName = v.actorName
        data.actorBody = v.actorBody
        data.actorBodyId = v.actorBodyId
        data.modleName = v.modleName
        data.modleBody = v.modleBody
        data.modleBodyId = v.modleBodyId
        data.tigerLotteryName = v.tigerLotteryName
        data.pos = VectorUtil.newVector3(v.x, v.y, v.z)
        data.yaw = tonumber(v.yaw)
        data.minPos = VectorUtil.newVector3i(v.x1, v.y1, v.z1)
        data.maxPos = VectorUtil.newVector3i(v.x2, v.y2, v.z2)
        data.refreshCD = tonumber(v.refreshCD)
        data.tipNor = v.tipNor
        data.tipPre = v.tipPre
        if data.tipNor == "@@@" then
            data.tipNor = ""
        end
        if data.tipPre == "@@@" then
            data.tipPre = ""
        end
        data.imageNor = v.imageNor
        data.imagePre = v.imagePre
        if data.imageNor == "@@@" then
            data.imageNor = ""
        end
        if data.imagePre == "@@@" then
            data.imagePre = ""
        end
        data.pools = StringUtil.split(v.pools, "#")
        data.countdownCD = v.countdownCD
        data.cubeMoney = v.cube
        self.tigerLottery[data.id] = data
    end
end
 
function TigerLotteryConfig:GetTigerLotteryConfig()
    return self.tigerLottery
end

return TigerLotteryConfig