# blockman_pc
- pc网页blockman
## 环境
``` bash
# node  v8.11.3
# npm   v5.6.0
# vue-cli   2.9.6
# 详细看package.json
```
## 打包 启动
``` bash
# 初次安装依赖
npm install
# 开发环境 浏览器打开localhost:8080
npm run dev
# 打包混淆
npm run build
# 单元测试(没有开发此功能)
npm run unit
```
- 增加多页配置:
    多页面需要配置一下几步,基于方便,我已将需要配置的地方留了对应模板例子,'example'
    根据以下几步操作, 
    1. 页面  在./html创建一个 '名字.html' 文件
    2. 入口  在./src/entry创建  '名字.js' 文件
    3. 修改目录名字  ./entry/名字.js
    ```JavaScript
        import App from '../名字/app/App'
        import router from '../名字/routes/router'
    ```
    4. 路由&组件    在./src创建  '名字' 文件夹 
        ```
            example/
                    app/
                        App.vue
                    routes/
                        router.js
        ```
        内容必须跟以上结构一样
    
- 过滤不打包页面
    - 在./build.js 
    ```JavaScript
        process.env.filterSystem=["example",...]  //这里追加
    ```
## 技术栈
- 使用axios并配置拦截器
    参考链接: https://github.com/970024540/vue-cli-MultiPage
- 使用多页面应用
    参考链接: https://github.com/970024540/vue-cli-MultiPage
- 使用elementUi
    文档链接: http://element-cn.eleme.io/#/zh-CN/component/quickstart
- 使用sass编译
    参考文档: https://www.cnblogs.com/crazycode2/p/6535105.html
- 使用three构造3d
    参考文档: https://github.com/mrdoob/three.js
- 使用dat.GUI调试3d界面
    参考文档: http://www.hangge.com/blog/cache/detail_1785.html
    
# 测试号
```
    uid : dhdkdjvhdjdj
    password : 未知
    uid : sherwin2
    password : 12345689
    uid : sherwin3
    password : 12345689
    uid : sherwin3
    password : 12345689
```
# 多国语言支持版本
- 区分多国语言代码
- https://blog.csdn.net/hanchaohao2012/article/details/50370984
- 获取ip
- http://ip-api.com/json/    


| 语 言     |   国家     |  语言代码 | 简码 |
| :--------:|:---------:|:---------:|:----:|
| 简体中文  |  中国      |  zh-cn    |  CN  |
| 英语      |  美国      |  en-us    |  US  |
| 韩文      |  韩国      |  ko-kr    |  KR  |
| 日语      |  日本      |  ja-jp    |  JP  |
| 印尼语    |  印度尼西亚 |  en-id    |  ID  |
| 德语      |  德国      |  de-de    |  DE  |
| 法语      |  法国      |  fr-fr    |  FR  |
| 意大利语  |  意大利     |  it-it    |  IT  |
| 葡萄牙语  |  葡萄牙     |  pt-br    |  PT  |
| 西班牙语  |  西班牙     |  es-es    |  ES  |
| 俄语      |  俄罗斯    |  ru-ru     |  RU  |
| 土耳其语  |  土耳其     |  tr-tr    |  TR  |
| 越南语    |  越南       | vi-vi     |  VN  |
| 英语      |  泰国       |  en-th    | TH  |


- 装扮模块的对应表

| type_id   |  name     | 
| :--------:|:---------:|
|   1       |   服饰    |
|   2       |   发饰    |
|   3       |   装饰    |
|   4       |   表情    |
|   5       |   动作    |
|   6       |   肤色    |

# api 的 issue
- 装扮装换格式/存放资源地方