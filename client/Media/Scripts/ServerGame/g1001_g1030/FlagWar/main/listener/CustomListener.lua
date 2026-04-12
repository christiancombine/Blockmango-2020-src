---
--- Created by Jimmy.
--- DateTime: 2018/6/8 0008 11:51
---
CustomListener = {}

function CustomListener:init()
    CustomTipMsgEvent.registerCallBack(self.onCustomTipMsg)
end

function CustomListener.onCustomTipMsg(rakssid, extra, isRight)
    if isRight == false then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local extras = StringUtil.split(extra, "=")
    if extras[1] == "Occupation" and extras[2] ~= nil then
        local npc = OccupationConfig:getOccupationNpc(tonumber(extras[2]))
        if npc then
            npc:onPlayerSelected(player)
        end
    end
end

return CustomListener
--endregion
