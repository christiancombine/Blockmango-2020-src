<template>
  <div id="app">
    <router-view :asideMenu="asideMenu"/>
  </div>
</template>

<script>
export default {
  name: "App",
  data() {
    return {
      asideMenu: []  //防止多层主件翻译不对情况
    };
  },
  created () {
     if (!this.$getCookie("language")) {
      this.get_user_ip();
    } else {
      this.$i18n.locale = this.$getCookie("language").slice(
        this.$getCookie("language").length - 2,
        this.$getCookie("language").length
      );
      this.asideMenu=[
        {
          path: "/",
          title:this.$t("frame.game")
        },
        {
          path: "/avatar",
          title: this.$t("frame.dressing")
        },
        {
          path: "/avatar",
          title: this.$t("frame.store")
        },
        {
          path: "/avatar",
          title: this.$t("frame.vip")
        },
        {
          path: "/avatar",
          title: this.$t("frame.recharge")
        },
        {
          path: "/avatar",
          title: this.$t("frame.tribal")
        },
        {
          path: "/avatar",
          title: this.$t("frame.friend")
        },
      ]
    }
  },
  methods: {
     /*
      * 判断客户ip国家
      */
    get_user_ip() {
      //必须要有令牌才能请求个人数据
      this.$http({
        method: "get",
        url: "http://ip-api.com/json",
        data: {}
      })
        .then(result => {
          if (result.data.status == "success") {
            //语言修改
            this.$i18n.locale = result.data.countryCode;
            this.$setCookie("language", this.$t("name"));
            this.$router.go(0)
          } else {
            this.$message({
              showClose: true,
              message: result.data.status,
              type: "error"
            });
          }
        })
        .catch(err => {});
    },
  }
};
</script>

<style  lang="scss"  type="text/css">
#app {
  font-family: "Avenir", Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
}
body {
  background-color: #e3c99e;
}
//禁止选中文本
.noselect {
  -webkit-touch-callout: none; /* iOS Safari */
  -webkit-user-select: none; /* Chrome/Safari/Opera */
  -khtml-user-select: none; /* Konqueror */
  -moz-user-select: none; /* Firefox */
  -ms-user-select: none; /* Internet Explorer/Edge */
  user-select: none; /* Non-prefixed version, currently
  not supported by any browser */
}
//小手
.cursor{cursor:pointer; }
.fr{
  float: right;
}
.fl{
  float: left;
}
</style>
