PaymentConfig = {}
PaymentConfig.timePayment = {}
PaymentConfig.fruitPayment = {}
PaymentConfig.ticketPayment = {}

local TYPE_PAYMENT = {
    TIME = 1,
    FRUIT = 2,
    TICKET = 3
}

function PaymentConfig:initPayment(config)
    for _, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.num = tonumber(v.num)
        data.payment = tonumber(v.payment)

        if tonumber(v.type) == TYPE_PAYMENT.TIME then
            table.insert(self.timePayment, data)
        end

        if tonumber(v.type) == TYPE_PAYMENT.FRUIT then
            table.insert(self.fruitPayment, data)
        end

        if tonumber(v.type) == TYPE_PAYMENT.TICKET then
            table.insert(self.ticketPayment, data)
        end
    end

    table.sort(self.timePayment, function (a, b)
        return a.id < b.id
    end)

    table.sort(self.fruitPayment, function (a, b)
        return a.id < b.id
    end)

    table.sort(self.ticketPayment, function (a, b)
        return a.id < b.id
    end)
end

function PaymentConfig:getPaymentByTime(num)
    for i, v in pairs(self.timePayment) do
        if v.num > num then
            return self.timePayment[i].payment
        end
    end

    return self.timePayment[#self.timePayment].payment
end

function PaymentConfig:getPaymentByFruit(num)
    for i, v in pairs(self.fruitPayment) do
        if v.num > num then
            return self.fruitPayment[i].payment
        end
    end

    return self.fruitPayment[#self.fruitPayment].payment
end

function PaymentConfig:getPaymentByTicket(num)
    for i, v in pairs(self.ticketPayment) do
        if v.num > num then
            return self.ticketPayment[i].payment
        end
    end

    return self.ticketPayment[#self.ticketPayment].payment
end

function PaymentConfig:initBirdSimulatorTimePayment()
    local price = {}
    for i, v in pairs(self.timePayment) do
        local data = BirdTimePrice.new()
        data.id = i
        data.price = v.payment
        data.timeLeft = v.num * 1000
        table.insert(price, data)
    end

    return price
end

return PaymentConfig