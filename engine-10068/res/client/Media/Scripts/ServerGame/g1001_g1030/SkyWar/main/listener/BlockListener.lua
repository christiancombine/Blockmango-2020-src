--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

require "Match"

BlockListener = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    if blockId == BlockID.CHEST then
        return false
    end
    return GameMatch.allowBreak
end

return BlockListener
--endregion
