AreaManager = {}

local AreaList = {}

function AreaManager:initArea(configs)
    for _, config in pairs(configs) do
        local Area
        if config.type == AreaConfig.Type.Tower then
            Area = TowerArea.new(config)
        elseif config.type == AreaConfig.Type.Team then
            Area = TeamArea.new(config)
        elseif config.type == AreaConfig.Type.Skin then
            Area = SkinArea.new(config)
        elseif config.type == AreaConfig.Type.Level then
            Area = LevelArea.new(config)
        end
        table.insert(AreaList, Area)
    end
end

function AreaManager:actorClicked(entityId, player)
    for _, area in pairs(AreaList) do
        if area.entityId == entityId then
            area:onClick(player)
            break
        end
    end
end

function AreaManager:onPlayerMove(player, x, y, z)
    for _, area in pairs(AreaList) do
        area:checkArea(player, x, y, z)
    end
end

function AreaManager:getAreaByEntityId(entityId)
    for _, area in pairs(AreaList) do
        if area.entityId == entityId then
            return area
        end
    end
end

function AreaManager:checkPlayerLevel(player)
    --目前只有客户端才会调用
    for _, area in pairs(AreaList) do
        area:checkPlayerLevel(player)
    end
end

function AreaManager:addPlayerToTeamArea(player)
    for _, area in pairs(AreaList) do
        if area.config.type == AreaConfig.Type.Team then
            table.insert(area.needSyncList, player)
        end
    end
end

function AreaManager:getTeamAreaById(areaId)
    for _, area in pairs(AreaList) do
        if area.config.type == AreaConfig.Type.Team and area.config.Id == areaId then
           return area
        end
    end
end