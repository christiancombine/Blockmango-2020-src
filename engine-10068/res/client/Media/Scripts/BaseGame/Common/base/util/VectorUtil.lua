---
--- Created by Jimmy.
--- DateTime: 2017/10/19 0019 10:38
---
local function float2int(value)
    if value == nil then
        return 0
    end
    if value > 0 then
        return math.floor(value)
    end
    local newValue = math.floor(value)
    if newValue == value then
        return value
    end
    return math.floor(newValue) + 1
end

VectorUtil = {}

function VectorUtil.newVector3i(x, y, z)
    local vec = {}
    vec.x = float2int(tonumber(x))
    vec.y = float2int(tonumber(y))
    vec.z = float2int(tonumber(z))
    return vec
end

function VectorUtil.newVector3(x, y, z)
    local vec3 = {}
    vec3.x = tonumber(x)
    vec3.y = tonumber(y)
    vec3.z = tonumber(z)
    return vec3
end

function VectorUtil.newVector4(x, y, z, w)
    local vec4 = {}
    vec4.x = tonumber(x)
    vec4.y = tonumber(y)
    vec4.z = tonumber(z)
    vec4.w = tonumber(w)
    return vec4
end

function VectorUtil.vector4ToVector3(vec4)
    local vec3 = {}
    vec3.x = vec4.x
    vec3.y = vec4.y
    vec3.z = vec4.z
    return vec3
end

function VectorUtil.toVector3(vec3i)
    local vec3 = {}
    vec3.x = vec3i.x
    vec3.y = vec3i.y
    vec3.z = vec3i.z
    return vec3
end

function VectorUtil.toVector3i(vec3)
    local vec3i = {}
    vec3i.x = float2int(vec3.x)
    vec3i.y = float2int(vec3.y)
    vec3i.z = float2int(vec3.z)
    return vec3i
end

function VectorUtil.hashVector3(vec3)
    return vec3.x .. ":" .. vec3.y .. ":" .. vec3.z
end

function VectorUtil.toBlockVector3(x, y, z)
    if x < 0 then
        x = x - 1
    end
    if y < 0 then
        y = y - 1
    end
    if z < 0 then
        z = z - 1
    end
    return VectorUtil.newVector3i(x, y, z)
end

function VectorUtil.equal(vec3_1, vec3_2)
    if vec3_1 and vec3_2 then
        return vec3_1.x == vec3_2.x and vec3_1.y == vec3_2.y and vec3_1.z == vec3_2.z
    else
        return false
    end
end

function VectorUtil.add3(vec3_1, vec3_2)
    return VectorUtil.newVector3(vec3_1.x + vec3_2.x, vec3_1.y + vec3_2.y, vec3_1.z + vec3_2.z)
end

function VectorUtil.add3i(vec3_1, vec3_2)
    return VectorUtil.newVector3i(vec3_1.x + vec3_2.x, vec3_1.y + vec3_2.y, vec3_1.z + vec3_2.z)
end

function VectorUtil.sub3(vec3_1, vec3_2)
    return VectorUtil.newVector3(vec3_1.x - vec3_2.x, vec3_1.y - vec3_2.y, vec3_1.z - vec3_2.z)
end

function VectorUtil.sub3i(vec3_1, vec3_2)
    return VectorUtil.newVector3i(vec3_1.x - vec3_2.x, vec3_1.y - vec3_2.y, vec3_1.z - vec3_2.z)
end

function VectorUtil.getPosX(vec3i, size)
    size = size or 1
    return VectorUtil.newVector3i(vec3i.x + size, vec3i.y, vec3i.z)
end

function VectorUtil.getNegX(vec3i, size)
    size = size or 1
    return VectorUtil.newVector3i(vec3i.x - size, vec3i.y, vec3i.z)
end

function VectorUtil.getPosY(vec3i, size)
    size = size or 1
    return VectorUtil.newVector3i(vec3i.x, vec3i.y + size, vec3i.z)
end

function VectorUtil.getNegY(vec3i, size)
    size = size or 1
    return VectorUtil.newVector3i(vec3i.x, vec3i.y - size, vec3i.z)
end

function VectorUtil.getPosZ(vec3i, size)
    size = size or 1
    return VectorUtil.newVector3i(vec3i.x, vec3i.y, vec3i.z + size)
end

function VectorUtil.getNegZ(vec3i, size)
    size = size or 1
    return VectorUtil.newVector3i(vec3i.x, vec3i.y, vec3i.z - size)
end

function VectorUtil.distance(pos1, pos2)
    local x = pos1.x - pos2.x
    local y = pos1.y - pos2.y
    local z = pos1.z - pos2.z
    return math.sqrt(x * x + y * y + z * z)
end

VectorUtil.ZERO = VectorUtil.newVector3(0, 0, 0)
VectorUtil.ONE = VectorUtil.newVector3(1, 1, 1)
VectorUtil.UNIT_X = VectorUtil.newVector3(1, 0, 0)
VectorUtil.UNIT_Y = VectorUtil.newVector3(0, 1, 0)
VectorUtil.UNIT_Z = VectorUtil.newVector3(0, 0, 1)
VectorUtil.NEG_UNIT_X = VectorUtil.newVector3(-1, 0, 0)
VectorUtil.NEG_UNIT_Y = VectorUtil.newVector3(0, -1, 0)
VectorUtil.NEG_UNIT_Z = VectorUtil.newVector3(0, 0, -1)
VectorUtil.INVALID = VectorUtil.newVector3(3.402823466e+38, 3.402823466e+38, 3.402823466e+38)

return VectorUtil