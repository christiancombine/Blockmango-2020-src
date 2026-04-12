// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import Vue from 'vue'
import App from '../index/app/App'
import router from '../index/routes/router'

import axios from '../../config/http'

import "babel-polyfill"//解决ie浏览器promise未定义情况

import ElementUI from 'element-ui'
import 'element-ui/lib/theme-chalk/index.css';
import '../../static/util/normal.css';

import {getCookie,setCookie,delCookie} from '../../static/util/cookie';
import {encodeSearchParams,mac} from '../../static/util/utils';

//自定义主件
import bmAside from "../components/aside_left";
Vue.component('bm-aside', bmAside)



//多国语言使用
import VueI18n from 'vue-i18n'
//语言导入
import LangEn_us from '../../static/language/en-us'
import LangZh_cn from '../../static/language/zh-cn'



Vue.use(VueI18n)
const i18n = new VueI18n({
  //定义默认语言
  locale: 'US', 
  messages:{
    'CN': LangZh_cn, //中文
    "US": LangEn_us,
  }
})


Vue.use(ElementUI);

Vue.prototype.$http = axios
Vue.prototype.$api = process.env

Vue.prototype.$getCookie = getCookie
Vue.prototype.$setCookie = setCookie
Vue.prototype.$delCookie = delCookie
Vue.prototype.$encodeSearchParams = encodeSearchParams
Vue.prototype.$mac = mac

Vue.config.productionTip = false



/* eslint-disable no-new */
new Vue({
  el: '#app',
  router,
  i18n,
  components: { App },
  template: '<App/>'
})
