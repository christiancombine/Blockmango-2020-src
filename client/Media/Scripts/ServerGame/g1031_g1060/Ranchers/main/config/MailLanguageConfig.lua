MailLanguageConfig = {}
MailLanguageConfig.lang = {}

function MailLanguageConfig:init(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.type = tonumber(v.type)
        data.title = v.title
        data.name = v.name
        self.lang[data.id] = data
    end
end


function MailLanguageConfig:getLangByType(type)
    for i, v in pairs(self.lang) do
        if v.type == type then
            return v
        end
    end

    return nil
end

return MailLanguageConfig