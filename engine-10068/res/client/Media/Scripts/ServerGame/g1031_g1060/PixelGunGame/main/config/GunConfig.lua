GunConfig = {}

GunConfig.Type = {
    Rifle = 1,
    Pistol = 2,
    CloseIn = 3,
    Snipe = 4,
    Bazooka = 5
}

GunConfig.Quality = {
    General = 1,
    Rare = 2,
    Epic = 3,
    Legend = 4,
    Surplus = 5
}

local GunConfigs = {}
local GunPool = {}
local SunOfWeight = {}

function GunConfig:init(guns)
    self:initGuns(guns)
end

function GunConfig:initGuns(guns)
    for id, gun in pairs(guns) do
        local item = {
            gunId = tonumber(id),
            itemId = tonumber(gun.itemId),
            type = tonumber(gun.type),
            name = gun.name,
            image = gun.image,
            spareBullet = tonumber(gun.spareBullet),
            bulletId = tonumber(gun.bulletId),
            bulletNum = tonumber(gun.maxBulletNum),
            hurtBlock = tonumber(gun.hurtBlock),
            pickupBulletNum = tonumber(gun.pickupBulletNum),
            attack = tonumber(gun.attack),
            range = tonumber(gun.range),
            speed = tonumber(gun.speed),
            knockback = tonumber(gun.knockback),
            recoil = tonumber(gun.recoil),
            sniperDistance = tonumber(gun.sniperDistance),
            bulletPerShoot = tonumber(gun.bulletPerShoot),
            bulletOffset = tonumber(gun.bulletOffset),
            gunHeadShot = tonumber(gun.gunHeadShot),
            mouseSensitivity = tonumber(gun.mouseSensitivity),
            bulletEffect = gun.bulletEffect or "",
            flameEffect = gun.flameEffect or "",
            isPiercing = tonumber(gun.isPiercing),
            explosionRange = tonumber(gun.explosionRange),
            gravityVelocity = tonumber(gun.gravityVelocity),
            bounceTimes = tonumber(gun.bounceTimes),
            isAutoShoot = tonumber(gun.isAutoShoot),
            quality = tonumber(gun.quality),
            weight = tonumber(gun.weight) or 0,
            gunType = tonumber(gun.gunType),
            bulletSpeed = tonumber(gun.bulletSpeed) or -1,
            actor = gun.actor,
            propertyIds = gun.propertyIds,
            headName = gun.headName or "",
            shootSpeed = tonumber(gun.shootSpeed) or 0
        }
        GunConfigs[gun.itemId] = item

        if GunPool[item.gunType] == nil then
            GunPool[item.gunType] = {}
        end
        table.insert(GunPool[item.gunType], item)
    end
    self:prepareGuns()
    self:setSumOfWeight()
end

function GunConfig:prepareGuns()
    for i, gun in pairs(GunConfigs) do
        HostApi.setBulletClipSetting(gun.bulletId, 64)
        HostApi.setGunSetting(GunConfig.newGunSetting(gun))
    end
end

function GunConfig.newGunSetting(item)
    local setting = GunPluginSetting.new()
    setting.gunId = item.itemId
    setting.gunType = item.type
    setting.maxBulletNum = item.bulletNum
    setting.damage = item.attack
    setting.knockback = item.knockback
    setting.bulletId = item.bulletId
    setting.shootRange = item.range
    setting.cdTime = item.speed
    setting.sniperDistance = item.sniperDistance
    setting.mouseSensitivity = item.mouseSensitivity
    setting.bulletPerShoot = item.bulletPerShoot
    setting.bulletOffset = item.bulletOffset
    setting.headshotDamage = item.gunHeadShot
    setting.recoil = item.recoil
    setting.bulletSpeed = item.bulletSpeed
    if #item.bulletEffect > 7 then
        setting.bulletEffect = item.bulletEffect
    end
    if #item.flameEffect > 7 then
        setting.flameEffect = item.flameEffect
    end
    setting.isPiercing = item.isPiercing
    setting.explosionRange = item.explosionRange
    setting.gravityVelocity = item.gravityVelocity
    setting.bounceTimes = item.bounceTimes
    setting.isAutoShoot = item.isAutoShoot
    return setting
end

function GunConfig:getGunByItemId(id)
    return GunConfigs[tostring(id)]
end

function GunConfig:getRandomGun(type)
    local rs = math.random(0, SunOfWeight[type])
    for gunId, gun in pairs(GunPool[type]) do
        rs = rs - gun.weight
        if rs < 0 then
            return gun
        end
    end
end

function GunConfig:setSumOfWeight()
    for type, pool in pairs(GunPool) do
        SunOfWeight[type] = self:getSumOfWeight(pool)
    end
end

function GunConfig:getSumOfWeight(pool)
    local sunOfWeight = 0
    for _, gun in pairs(pool) do
        sunOfWeight = sunOfWeight + gun.weight
    end
    return sunOfWeight
end

