---
--- Created by Jimmy.
--- DateTime: 2018/4/27 0027 16:29
---

ArmsConfig = {}
ArmsConfig.arms = {}

function ArmsConfig:init(arms)
    self:initArms(arms)
end

function ArmsConfig:initArms(arms)
    for id, arm in pairs(arms) do
        local item = {}
        item.id = id
        item.level = tonumber(arm.level)
        item.itemId = tonumber(arm.itemId)
        item.name = arm.name
        item.image = arm.image
        item.type = tonumber(arm.type)
        item.attack = tonumber(arm.attack)
        item.range = tonumber(arm.range)
        item.speed = tonumber(arm.speed)
        item.maxBullet = tonumber(arm.maxBullet)
        item.knockback = tonumber(arm.knockback)
        item.sniperDistance = tonumber(arm.sniperDistance)
        item.bulletPerShoot = tonumber(arm.bulletPerShoot)
        item.bulletOffset = tonumber(arm.bulletOffset)
        item.attackX = tonumber(arm.attackX)
        item.price = tonumber(arm.price)
        item.desc = "prop.attack.value=" .. item.attack .. "&" .. "prop.range.value=" .. item.range .. "&" .. "prop.speed.value=" .. item.speed
        self.arms[item.level] = item
    end
    self:prepareArms()
end

function ArmsConfig:prepareArms()
    for level, arm in pairs(self.arms) do
        HostApi.setGunSetting(ArmsConfig.newGunSetting(arm))
    end
end

function ArmsConfig.newGunSetting(item)
    local setting = GunPluginSetting.new()
    setting.gunId = item.itemId
    setting.gunType = item.type
    setting.maxBulletNum = item.maxBullet
    setting.damage = item.attack
    setting.knockback = item.knockback
    setting.bulletId = 441
    setting.shootRange = item.range
    setting.cdTime = item.speed
    setting.sniperDistance = item.sniperDistance
    setting.bulletPerShoot = item.bulletPerShoot
    setting.bulletOffset = item.bulletOffset
    return setting
end

function ArmsConfig:getArms()
    return self.arms
end

function ArmsConfig:getArmById(id)
    for lv, arm in pairs(self.arms) do
        if arm.id == id then
            return arm
        end
    end
end

function ArmsConfig:getArmByLv(level)
    return self.arms[level]
end

return ArmsConfig