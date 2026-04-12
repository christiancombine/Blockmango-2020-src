
InventoryConfig = class()

function InventoryConfig:ctor(config)
    self.fixedItems = {}
    self.foodNumRange = {}
    self.food = {}
    self.equipNumRange={}
    self.equip = {}
    self.weaponNumRange={}
    self.weapon = {}
    self.materialsNumRange={}
    self.materials = {}
    self.pos = {}
    self.hasResetChest = {}
    self:init(config)
end

function InventoryConfig:init(config)
    if config == nil then
        HostApi.log("InventoryConfig is nil")
        return
    end


    self.foodNumRange[1] = tonumber(config.foodNumRange[1])
    self.foodNumRange[2] = tonumber(config.foodNumRange[2])
    for i, v in pairs(config.food) do
        self.food[i] = {}
        self.food[i].id = tonumber(v.id)
        self.food[i].randomRange = {}
        self.food[i].randomRange[1] = tonumber(v.randomRange[1])
        self.food[i].randomRange[2] = tonumber(v.randomRange[2])
        self.food[i].probability = tonumber(v.probability)
    end

    self.equipNumRange[1] = tonumber(config.equipNumRange[1])
    self.equipNumRange[2] = tonumber(config.equipNumRange[2])
    for i, v in pairs(config.equip) do
        self.equip[i] = {}
        self.equip[i].id = tonumber(v.id)
        self.equip[i].randomRange = {}
        self.equip[i].randomRange[1] = tonumber(v.randomRange[1])
        self.equip[i].randomRange[2] = tonumber(v.randomRange[2])
        self.equip[i].probability = tonumber(v.probability)
    end

    self.materialsNumRange[1] = tonumber(config.materialsNumRange[1])
    self.materialsNumRange[2] = tonumber(config.materialsNumRange[2])
    for i, v in ipairs(config.materials) do
        self.materials[i] = {}
        self.materials[i].id = tonumber(v.id)
        self.materials[i].randomRange = {}
        self.materials[i].randomRange[1] = tonumber(v.randomRange[1])
        self.materials[i].randomRange[2] = tonumber(v.randomRange[2])
        self.materials[i].probability = tonumber(v.probability)
    end

    self.weaponNumRange[1] = tonumber(config.weaponNumRange[1])
    self.weaponNumRange[2] = tonumber(config.weaponNumRange[2])

    for i, v in ipairs(config.weapon) do
        self.weapon[i] = {}
        self.weapon[i].id = tonumber(v.id)
        self.weapon[i].randomRange = {}
        self.weapon[i].randomRange[1] = tonumber(v.randomRange[1])
        self.weapon[i].randomRange[2] = tonumber(v.randomRange[2])
        self.weapon[i].probability = tonumber(v.probability)
    end

    for i, v in ipairs(config.pos) do
        self.pos[i] = VectorUtil.newVector3i(v.x, v.y, v.z)
    end

end


function InventoryConfig:reset()
    self.hasResetChest = {}
end

return InventoryConfig