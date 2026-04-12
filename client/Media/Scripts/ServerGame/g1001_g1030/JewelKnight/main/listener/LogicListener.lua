LogicListener = {}

function LogicListener:init()
    EntityItemSpawnEvent.registerCallBack(self.onEntityItemSpawn)
end

function LogicListener.onEntityItemSpawn(itemId, itemMeta, behavior)
    if behavior == "break.block" then
        return false
    end

    for i, v in pairs(ItemConfig.noDrop) do
        if itemId == v.id then
            return false
        end
    end

    return true
end

return LogicListener