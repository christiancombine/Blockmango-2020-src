--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

require "Match"
require "messages.Messages"

BlockListener = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    BlockBreakWithMetaEvent.registerCallBack(self.onBlockBreakWithMeta)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockBreakWithGunEvent.registerCallBack(self.onBlockBreakWithGun)
end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    return false
end

function BlockListener.onBlockBreakWithMeta(rakssid, blockId, blockMeta, vec3, itemID)
    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    return false
end

function BlockListener.onBlockBreakWithGun(rakssid, blockId, blockPos, gunId)
    return false
end

return BlockListener
--endregion
