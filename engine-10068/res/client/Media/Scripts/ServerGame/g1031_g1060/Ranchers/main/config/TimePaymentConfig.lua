TimePaymentConfig = {}
TimePaymentConfig.payment = {}

function TimePaymentConfig:initPayment(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.time = tonumber(v.time)
        data.payment = tonumber(v.payment)
        self.payment[data.id] = data
    end
end

function TimePaymentConfig:getPaymentByTime(time)
    for i, v in pairs(self.payment) do
        if v.time > time then
            return self.payment[v.id].payment
        end
    end

    return self.payment[#self.payment].payment
end

function TimePaymentConfig:initRanchTimePrice()
    local price = {}
    for i, v in pairs(self.payment) do
        local data = RanchTimePrice.new()
        data.id = v.id
        data.price = v.payment
        data.timeLeft = v.time
        table.insert(price, data)
    end

    return price
end

return TimePaymentConfig