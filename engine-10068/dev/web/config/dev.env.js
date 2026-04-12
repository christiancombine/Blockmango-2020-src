'use strict'
const merge = require('webpack-merge')
const prodEnv = require('./prod.env')

module.exports = merge(prodEnv, {
  NODE_ENV: '"development"', //开发环境
  API_BM: "/user/",//因为配置了代理，所以本地只需要配应用名就行了
  API_BM_GAME: "/game/",//因为配置了代理，所以本地只需要配应用名就行了
  API_BM_APPGAME: "/game/",//因为配置了代理，所以本地只需要配应用名就行了
  API_BM_q: "/v1/",//因为配置了代理，所以本地只需要配应用名就行了
  API_BM_decoration: "/decoration/"//因为配置了代理，所以本地只需要配应用名就行了
})
