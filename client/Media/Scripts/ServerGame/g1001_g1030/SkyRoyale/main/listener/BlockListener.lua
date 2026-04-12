--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

require "Match"

BlockListener = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
end

function BlockListener.onBlockBreak(rakssid, blockId, blockPos)
    if GameMatch.hasStartGame == false then
        return false
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if EngineWorld:isBlockChange(blockPos) then
        player:addItem(blockId, 1, 0)
        return true
    end
    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    if GameMatch.hasStartGame == false then
        return false
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        return true
    end

    return false
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

return BlockListener
--endregion
