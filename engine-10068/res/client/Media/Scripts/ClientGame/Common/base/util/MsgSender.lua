--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

MsgSender = {}

function MsgSender.sendMsg(msg, args)
    if msg == nil then
        HostApi.log("Msg.sendMsg  is nil msg")
        return
    end
    if type(msg) == "number" then
        HostApi.sendMsgByType(0, 5, msg, args or "")
    else
        HostApi.sendMsg(0, 5, msg)
    end
end

function MsgSender.sendTopTips(time, msg, args)
    if msg == nil then
        HostApi.log("Msg.sendTopTips  is nil msg")
        return
    end

    if type(msg) == "number" then
        HostApi.sendMsgByType(1, time, msg, args or "")
    else
        HostApi.sendMsg(1, time, msg)
    end
end

function MsgSender.sendBottomTips(time, msg, args)
    if msg == nil then
        HostApi.log("Msg.sendBottomTips  is nil msg")
        return
    end

    if type(msg) == "number" then
        HostApi.sendMsgByType(2, time, msg, args or "")
    else
        HostApi.sendMsg(2, time, msg)
    end
end

function MsgSender.sendCenterTips(time, msg, args)
    if msg == nil then
        HostApi.log("Msg.sendCenterTips  is nil msg")
        return
    end

    if type(msg) == "number" then
        HostApi.sendMsgByType(3, time, msg, args or "")
    else
        HostApi.sendMsg(3, time, msg)
    end
end

function MsgSender.sendOtherTips(time, msg, args)
    if msg == nil then
        HostApi.log("Msg.sendOtherTips is nil msg")
        return
    end

    if type(msg) == "number" then
        HostApi.sendMsgByType(4, time, msg, args or "")
    else
        HostApi.sendMsg(4, time, msg)
    end
end

return MsgSender

--endregion
