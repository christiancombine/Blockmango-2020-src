XTipsConfig = {}
XTipsConfig.configs = {}
XTipsLocation = 
{
    top = 1,
    other = 2,
    center = 3,
    buttom = 4
}
-- 1036001  击杀玩家提示
-- 1036002  玩家推动车的提示
function XTipsConfig:OnInit(configs)
    for _, config in pairs(configs) do
        local tab = {}
        tab.TipsId = tonumber(config.TipsId)
        tab.TipsTime = tonumber(config.TipsTime)
        tab.TipsLocation =  tonumber(config.TipsLocation)
        tab.TipsName = config.TipsName
        table.insert(self.configs, tab)
    end
end

function XTipsConfig:getTipsInfoByTipid(tipsId)
    for _, config in pairs(self.configs) do
        if config.TipsId == tipsId then
            return config.TipsTime, config.TipsLocation
        end
    end
    return nil
end
