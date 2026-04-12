
ChestListener = {}

function ChestListener:init()
    ChestOpenEvent.registerCallBack(function(vec3, inv, rakssid)
        self:onChestOpen(vec3, inv, rakssid)
    end)
    ChestOpenSpHandleEvent.registerCallBack(self.onChestOpenSpHandle)
end

function ChestListener:onChestOpen(vec3, inv, rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    HostApi.log("onChestOpen " .. vec3.x .. " " .. vec3.y .. " " .. vec3.z)
    --ChestConfig:fillOpenChest(vec3, inv)
end

function ChestListener.onChestOpenSpHandle(vec3, entityId)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if player == nil then
        return false
    end
    return true
end

return ChestListener