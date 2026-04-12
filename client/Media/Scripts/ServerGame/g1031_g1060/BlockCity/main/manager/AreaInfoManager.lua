AreaInfoManager = {}

AreaType = {
    TigerLottery = 104701,
    Cannon = 104702,
    MailBox = 104703,
    Decoration = 104704,
    PublicChairs = 104705,
    Manager = 104706,
    ShopHouse = 104707,
}

function AreaInfoManager:push(id, type, titleNor, titlePre, backNor, backPre, isVisible, startPos, endPos)
    if self.areaInfoList == nil then
        self.areaInfoList = {}
    end
    local score = {}
    score.id = id
    score.type = type
    score.titleNor = titleNor
    score.titlePre = titlePre
    score.backNor = backNor
    score.backPre = backPre
    score.isVisible = isVisible
    score.startPos = VectorUtil.newVector3i(startPos.x, startPos.y, startPos.z)
    score.endPos = VectorUtil.newVector3i(endPos.x, endPos.y, endPos.z)
    self.areaInfoList[id] = score
end

function AreaInfoManager:removeDynamicAreaInfo()
    for _, v in pairs(self.areaInfoList) do
        --if v.type == AreaType.MailBox then
        --    self.areaInfoList[v.id] = nil
        --end

        if v.type == AreaType.Decoration then
            self.areaInfoList[v.id] = nil
        end
    end
end

function AreaInfoManager:SyncAreaInfo(rakssid)
    local areas = {}
    for _, v in pairs(self.areaInfoList) do
        local area = ActionArea.new()
        area.id = v.id
        area.type = v.type
        area.titleNor = v.titleNor
        area.titlePre = v.titlePre
        area.isVisible = v.isVisible
        area.backNor = v.backNor
        area.backPre = v.backPre
        area.startPos = v.startPos
        area.endPos = v.endPos
        table.insert(areas, area)
    end
    HostApi.setPlayerActionArea(rakssid, areas)
    self:removeDynamicAreaInfo()
end
