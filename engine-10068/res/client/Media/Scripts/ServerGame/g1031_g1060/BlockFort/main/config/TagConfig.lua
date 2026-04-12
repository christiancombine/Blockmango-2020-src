TagConfig = {}
TagConfig.tag = {}

function TagConfig:init(config)
    for _, v in pairs(config) do
        local data = {}
        data.order = tonumber(v.order)
        data.type = tonumber(v.type)
        data.tag = tonumber(v.tag)
        data.name = (v.name == "@@@" and {""} or {v.name})[1]
        data.icon = (v.icon == "@@@" and {""} or {v.icon})[1]
        self.tag[data.order] = data
    end
end

function TagConfig:getTagInfoByTypeAndTag(type, tag)
    type = tonumber(type)
    tag = tonumber(tag)
    for _, v in pairs(self.tag) do
        if v.type == type and v.tag == tag then
            return v
        end
    end
    print(" tag.csv can not find Info by type =", type,"and tag =", tag)
    return nil
end