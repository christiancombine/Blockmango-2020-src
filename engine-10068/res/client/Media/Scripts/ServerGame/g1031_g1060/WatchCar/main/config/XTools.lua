
XTools = {}
function XTools:CastToVector(_x, _y, _z)
    local x = type(_x) ~= "number" and tonumber(_x) or _x
    local y = type(_y) ~= "number" and tonumber(_y) or _y
    local z = type(_z) ~= "number" and tonumber(_z) or _z
    return VectorUtil.newVector3(x, y, z)
end

