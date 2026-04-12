---
--- Created by Jimmy.
--- DateTime: 2018/8/3 0003 14:42
---
TipNpc = class()
TipNpc.TYPE_TIP = 4
TipNpc.TYPE_MULI_TIP = 5

function TipNpc:ctor(config)
    self.name = config.name
    self.actor = config.actor
    self.position = VectorUtil.newVector3(config.x, config.y, config.z)
    self.yaw = tonumber(config.yaw)
    self.type = tonumber(config.type)
    self.title = config.title
    self.content = config.content
    self:onCreate()
end

function TipNpc:onCreate()
    EngineWorld:addSessionNpc(self.position, self.yaw, self.type, self.actor, function(entity)
        entity:setNameLang(self.name or "")
        if self.type == TipNpc.TYPE_TIP then
            entity:setSessionContent(self.content or "")
        elseif TipNpc.TYPE_MULI_TIP then
            local data = {}
            data.title = self.title
            data.tips = StringUtil.split(self.content, "#")
            entity:setSessionContent(json.encode(data) or "")
        end
    end)
end

return TipNpc

