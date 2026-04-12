import Vue from 'vue'
import Router from 'vue-router'
import frame from '@/components/frame' //整个外层框架

//登录注册相关
import index from '@/components/me/index'
import security from '@/components/me/security'
import change_password from '@/components/me/security_change_password'
import language from '@/components/me/language'


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
          path: 'language',
          name: 'language',
          component: language
        },
      ]
    },

  ]
})
