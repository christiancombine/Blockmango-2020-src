import axios from 'axios'; //引入axios

import {
  getCookie
} from '../static/util/cookie'; //引入进度条插件 ，npm install --save nprogress

import NProgress from 'nprogress'; //引入进度条插件 ，npm install --save nprogress
import 'nprogress/nprogress.css'; // 引入进度条css文件，相对路径即可

//axios 基本配置
axios.defaults.timeout = 45000; //请求超时时间
axios.defaults.withCredentials = false; //运行跨域
//设置头部信息
// axios.defaults.headers['withCredentials'] ="true";

//添加一个请求拦截器
axios.interceptors.request.use(function (config) {
  //在请求发出之前进行一些操作
  NProgress.start(); //进度条开始

  
  //过滤部分接口不需要使用令牌
  if(config.url!="http://ip-api.com/json"){
    if ((getCookie("accessToken") && getCookie("userId"))||(getCookie("visitor_accessToken") && getCookie("visitor_id"))) { // 每次发送请求之前判断是否存在token，如果存在，则统一在http请求的header都加上token，不用每次请求都手动添加了
      config.headers['Access-Token'] = getCookie("accessToken")||getCookie("visitor_accessToken") ;
      config.headers['userId'] = getCookie("userId")||getCookie("visitor_id") ;
      //设置语言
      config.headers["language"]= getCookie("language")||"en_US"


    }
  }else{
    config.headers["Content-Type"]="text/plain; charset=utf-8"
  }




  return config;
}, function (error) {
  NProgress.done(); //进度条结束
  //Do something with request error
  return Promise.reject(error);
});

//添加一个响应拦截器
axios.interceptors.response.use(function (res) {
  //在这里对返回的数据进行处理
  NProgress.done();
  return res;
}, function (error) {
  NProgress.done();
  if (error.response) {
    switch (error.response.status) {
      case 401:
        console.log('接口返回401，请查看是否未登录。跳至登录页面')
    }
  }
  //Do something with response error
  return Promise.reject(error);
})

export default axios; //最后我们要对axios导出去使用
