local manager = nil

MapManager = class("MapManager", BaseMapManager)

function MapManager:Instance()
    return manager
end

function MapManager:clearMonster()
    local monsters = LuaEntityManager:getMonstersByManager(self)
    for _, monster in pairs(monsters) do
        monster:onDie()
    end
end

Listener.registerCallBack(MapManagerCreateEvent, function(data)
    local data = json.decode(data)
    manager = MapManager.new(data.mapConfig, data.processId)
    for _, pathId in pairs(data.pathIds) do
        manager:addPath(pathId)
    end

    LuaTimer:schedule(function()
        BgmUtil:startBgm(tonumber(data.mapConfig.bgmId))
    end, 10)

    manager:getPath(data.curId):addPlayer(GamePlayer:Instance())
end)

Listener.registerCallBack(MapManagerDestroyEvent, function()
    manager:onDestroy()
end)

Listener.registerCallBack(UsePropsEvent, function(data)
    local builder = DataBuilder.new():fromData(data)
    manager.propsTrigger:useProps(builder:getParam("propsId"), builder:getParam("userId"))
end)