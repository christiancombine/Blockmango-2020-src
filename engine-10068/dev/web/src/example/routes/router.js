import Vue from 'vue'
import Router from 'vue-router'
import frame from '@/components/frame' //整个外层框架

//登录注册相关
import user_data_add from '@/components/user_data_add'
import change_password from '@/components/me/security_change_password'

import index from '@/components/index'
import game_details from '@/components/game_details'
import security from '@/components/me/security'
import avatar from '@/components/avatar'

Vue.use(Router)

export default new Router({
  routes: [{
      path: '/',
      // name: 'frame',
      // redirect: '/',
      component: frame,
      children: [{
          path: '/',
          name: 'index',
          component: index
        },
        {
          path: 'security',
          name: 'security',
          component: security,
        },
        {
          path: 'security/change_password',
          name: 'security/change_password',
          component: change_password
        },
        {
          path: 'user_data_add',
          name: 'user_data_add',
          component: user_data_add
        },
        {
          path: 'game_details',
          name: 'game_details',
          component: game_details
        },
        {
          path: 'avatar',
          name: 'avatar',
          component: avatar
        },
      ]
    },

  ]
})
