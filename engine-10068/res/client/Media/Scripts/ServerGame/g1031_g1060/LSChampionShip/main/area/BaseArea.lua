BaseArea = class()

function BaseArea:ctor(areaName, areaType, areaRange, areaLanguage)
    self.Name = areaName
    self.Type = areaType
    self.Regions = areaRange
    self.Language = areaLanguage
    self.PlayerList = {}
    self:onCreateArea()
end

function BaseArea:onCreateArea()
    --在区域中心显示文字
    --显示区域范围
end

function BaseArea:inArea(Pos, areaExpand)
    areaExpand = areaExpand or 0
    for _, Region in pairs(self.Regions) do
        if Region[1].x - areaExpand < Pos.x and Region[2].x + areaExpand > Pos.x
                and Region[1].z - areaExpand < Pos.z and Region[2].z + areaExpand > Pos.z then
            return true
        end
    end
    return false
end

function BaseArea:removeLeftPlayer(player, Pos)
    local isPlayerIn = self:inArea(Pos)
    if not isPlayerIn and self:isPlayerInArea(player) then
        return self:removePlayerFromArea(player)
    end
    return false
end

function BaseArea:checkBeforeInArea(player, Pos)
    if player:isGrab() == false then
        return
    end

    local isPlayerIn = self:inArea(Pos, GameConfig.areaExpand)
    if isPlayerIn then
        self:onBeforePlayerIn(player)
    end
end

function BaseArea:addJoinedPlayer(player, Pos)
    self:checkBeforeInArea(player, Pos)

    local isPlayerIn = self:inArea(Pos)
    if isPlayerIn and self:isPlayerInArea(player) then
        self:onPlayerStay(player)
        return true
    elseif isPlayerIn then
        self:addPlayerToArea(player)
        return true
    end
    return false
end

function BaseArea:addPlayerToArea(player)
    table.insert(self.PlayerList, player)
    player.curArea = self.Type
    self:onPlayerIn(player)
end

function BaseArea:removePlayerFromArea(player)
    for index, v in pairs(self.PlayerList) do
        if player.rakssid == v.rakssid then
            self:onPlayerOut(v)
            table.remove(self.PlayerList, index)
            return true
        end
    end
    return false
end

function BaseArea:isPlayerInArea(player)
    for _, v in pairs(self.PlayerList) do
        if player.rakssid == v.rakssid then
            return true
        end
    end
    return false
end

function BaseArea:onBeforePlayerIn(player)
    player:unbindAllEntity()
end

function BaseArea:onPlayerIn(player)

end

function BaseArea:onPlayerOut(player)

end

function BaseArea:onPlayerStay(player)

end

return BaseArea