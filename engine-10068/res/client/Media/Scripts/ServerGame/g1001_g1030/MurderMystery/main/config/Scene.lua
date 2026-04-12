---
--- Created by Jimmy.
--- DateTime: 2017/11/14 0014 16:10
---
require "config.PropConfig"
require "config.StoreConfig"
require "config.ProduceConfig"

Scene = class()

function Scene:ctor(begin, config)
    self.begin = begin
    self.teleportPos = {}
    self.role = {}
    self.props = {}
    self.moneyPos = {}
    self.storeConfig = {}
    self.produceConfig = {}
    self:init(config)
end

function Scene:init(config)
    self:initTeleportPos(config.teleportPos)
    self:initRoles(config.role)
    self:initProps(config.props)
    self:initMoneyPos(config.moneyPos)
    self:initMerchantConfig(config.stores)
    self:initProduceConfig(config.produce)
end

function Scene:initTeleportPos(config)
    for i, v in pairs(config) do
        self.teleportPos[i] = {}
        self.teleportPos[i].use = false
        self.teleportPos[i].x = tonumber(v.x)
        self.teleportPos[i].y = tonumber(v.y)
        self.teleportPos[i].z = tonumber(v.z)
    end
end

function Scene:initMoneyPos(config)
    for i, v in pairs(config) do
        self.moneyPos[i] = {}
        self.moneyPos[i].id = v.id
        self.moneyPos[i].tick = tonumber(v.tick)
        self.moneyPos[i].life = tonumber(v.life)
        self.moneyPos[i].pos = VectorUtil.newVector3(v.pos[1], v.pos[2], v.pos[3])
    end
end

function Scene:initRoles(config)
    self.role.murderer = self:initRole(config.murderer)
    self.role.sheriff = self:initRole(config.sheriff)
    self.role.civilian = self:initRole(config.civilian)
end

function Scene:initRole(config)
    local role = {}
    role.prop = {}
    for i, v in pairs(config.prop) do
        local key = tonumber(v.id)
        role.prop[key] = {}
        role.prop[key].id = tonumber(v.id)
        role.prop[key].num = tonumber(v.num)
        role.prop[key].fm = {}
        role.prop[key].fm.id = tonumber(v.fm[1])
        role.prop[key].fm.lv = tonumber(v.fm[2])
    end
    role.hp = config.hp
    return role
end

function Scene:initProps(config)
    self.props = PropConfig.new(config)
end

function Scene:getRole(role)
    if role == GamePlayer.ROLE_CIVILIAN then
        return self.role.civilian
    end
    if role == GamePlayer.ROLE_SHERIFF then
        return self.role.sheriff
    end
    if role == GamePlayer.ROLE_MURDERER then
        return self.role.murderer
    end
    return self.role.civilian
end

function Scene:initMerchantConfig(stores)
    self.storeConfig = MerchantConfig.new(self.begin, stores)
end

function Scene:initProduceConfig(produce)
    self.produceConfig = ProduceConfig.new(produce)
end

function Scene:getArmsHurt(id)
    return self.props:getArmsHurt(id)
end

function Scene:getValidTeleportPos()
    for i, v in pairs(self.teleportPos) do
        if (v.use == false) then
            v.use = true
            return v
        end
    end
    return self.teleportPos[1]
end

function Scene:getStore()
    return self.storeConfig
end

function Scene:reset()
    for i, v in pairs(self.teleportPos) do
        v.use = false
    end
    self.produceConfig:reset()
end

return Scene