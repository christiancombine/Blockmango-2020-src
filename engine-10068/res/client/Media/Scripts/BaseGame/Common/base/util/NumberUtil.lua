---
--- Created by Jimmy.
--- DateTime: 2018/3/8 0008 10:42
---
NumberUtil = {}

function NumberUtil.getIntPart(num)
    if num <= 0 then
        return math.ceil(num);
    end
    if math.ceil(num) == num then
        num = math.ceil(num);
    else
        num = math.ceil(num) - 1;
    end
    return num;
end

function NumberUtil.int64ToNumber(num)
    return tonumber(tostring(num))
end

return NumberUtil