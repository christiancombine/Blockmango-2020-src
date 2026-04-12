--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

require "config.BlockConfig"
require "Match"

BlockListener = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    local canBreak = BlockConfig:canBreakBlock(blockId, player:getHeldItemId())
    if canBreak and blockId == BlockID.SAND then
        player:onBreakSand()
    end
    return canBreak
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    return BlockConfig:canPlaceBlock(itemId)
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

return BlockListener
--endregion
