--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

require "Match"

BlockListener = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    return false
end

return BlockListener
--endregion
