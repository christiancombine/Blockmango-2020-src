AreaManager = {}

AreaManager.AreaMapping = {
    [Define.Area.ShopArea] = ShopArea,
    [Define.Area.SellArea] = SellArea,
    [Define.Area.AutoArea] = AutoArea,
    [Define.Area.SafeArea] = SafeArea,
    [Define.Area.AdvancedArea] = AdvancedArea,
    [Define.Area.BackHallArea] = BackHallArea,
    [Define.Area.StartAdvancedArea] = StartAdvancedArea,
}

local Areas = {}
local AdvancedSceneAreas = {}


function AreaManager:createArea()
    local AreasInfo = FunctionalAreaConfig:getAllAreaInfo()
    for _, AreaInfo in pairs(AreasInfo) do
        if AreaInfo.AreaType == Define.Area.AdvancedArea then

        end
    end
    for _, AreaInfo in pairs(AreasInfo) do
        if AreaInfo.AreaType ~= Define.Area.AdvancedArea then
            local Area = self.AreaMapping[AreaInfo.AreaType]
            local area = Area.new(AreaInfo.AreaId, AreaInfo.AreaType, AreaInfo.AreaRange, AreaInfo.AreaLanguage)
            local scene = nil
            if not scene then
                table.insert(Areas, area)
            else
                local sceneAreas = AdvancedSceneAreas[scene.Id]
                if sceneAreas ~= nil then
                    table.insert(sceneAreas, area)
                end
            end
        end
    end
end

function AreaManager:checkPlayerPosition(player, Pos)
    if player:isAdvancing() then
        local sceneAreas = AdvancedSceneAreas[player.advancingId]
        if not sceneAreas then
            return
        end
        for _, area in pairs(sceneAreas) do
            area:removeLeftPlayer(player, Pos)
        end
        for _, area in pairs(sceneAreas) do
            area:addJoinedPlayer(player, Pos)
        end
    else
        for _, Area in pairs(Areas) do
            Area:removeLeftPlayer(player, Pos)
        end
        for _, Area in pairs(Areas) do
            Area:addJoinedPlayer(player, Pos)
        end
    end
end

function AreaManager:onPlayerLogout(player)
    for _, area in pairs(Areas) do
        area:removePlayerFromArea(player)
    end
    for _, areas in pairs(AdvancedSceneAreas) do
        for _, area in pairs(areas) do
            area:removePlayerFromArea(player)
        end
    end
end