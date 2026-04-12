require "config.XTipsConfig"

XGameTips = {}
-- 1036002  推车提示
-- 1036003  触发buff提示
function XGameTips:sendTips(tipsId, ...)
    local tipsMsg = self:jointTips(...)
    local tipsTime, tipsLocation = XTipsConfig:getTipsInfoByTipid(tipsId)
    if tipsTime and tipsLocation then
        if tipsLocation == XTipsLocation.top then
            MsgSender.sendTopTips(tipsTime, tipsId, tipsMsg)
            return true
        elseif tipsLocation == XTipsLocation.center then
            MsgSender.sendCenterTips(tipsTime, tipsId, tipsMsg)
            return true
        elseif tipsLocation == XTipsLocation.buttom then
            MsgSender.sendBottomTips(tipsTime, tipsId, tipsMsg)
            return true
        elseif tipsLocation == XTipsLocation.other then
            MsgSender.sendOtherTips(tipsTime, tipsId, tipsMsg)
        end
    end
    return false
end

function XGameTips:jointTips(...)
    local tab = { ... }
    return table.concat(tab,'\t')
end

