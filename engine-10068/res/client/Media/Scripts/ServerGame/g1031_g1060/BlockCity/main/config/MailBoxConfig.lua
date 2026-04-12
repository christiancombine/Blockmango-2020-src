MailBoxConfig = {}
MailBoxConfig.mailBox = {}

function MailBoxConfig:init(config)
    for _, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.x = tonumber(v.x)
        data.y = tonumber(v.y)
        data.z = tonumber(v.z)
        data.yaw = tonumber(v.yaw)
        data.x1 = tonumber(v.x1)
        data.y1 = tonumber(v.y1)
        data.z1 = tonumber(v.z1)
        data.x2 = tonumber(v.x2)
        data.y2 = tonumber(v.y2)
        data.z2 = tonumber(v.z2)
        data.actorName = v.actorName
        data.actorBody = v.actorBody
        data.actorBodyId = v.actorBodyId
        data.tipNor = v.tipNor
        data.tipPre = v.tipPre
        data.imageNor = v.imageNor
        data.imagePre = v.imagePre
        if data.imageNor == "@@@" then
            data.imageNor = ""
        end
        if data.imagePre == "@@@" then
            data.imagePre = ""
        end
        self.mailBox[data.id] = data
    end
end

function MailBoxConfig:getMailBoxById(id)
    if not self.mailBox[tonumber(id)] then
        return nil
    end

    return self.mailBox[tonumber(id)]
end