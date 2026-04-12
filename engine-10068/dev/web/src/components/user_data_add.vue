<template>
  <div class="hello">
     <div class="bm-box-content">
       <div>
         <h1>{{$t('user_data_add.enter_nickName')}}</h1>
          <el-input type="text" max="20" v-model="register.nickName" auto-complete="off"></el-input>
       </div>
          <h1>{{$t('user_data_add.sex')}}</h1>
       <div class="bm-gender-select">
          <el-button class="bm-button-sex" :class="register.sex==1?'active':''" @click="register.sex=1">{{$t('user_data_add.man')}}</el-button>
          <el-button class="bm-button-sex" :class="register.sex==2?'active':''" @click="register.sex=2">{{$t('user_data_add.woman')}}</el-button>
       </div>
       <div>
          <el-button class="bm-complete" :disabled="register.nickName.length==''" @click="complete_button">{{$t('user_data_add.complete')}}</el-button>
       </div>
     </div>
  </div>
</template>

<script>
export default {
  name: "sign_in",
  data() {
    return {
      register: {
        // diamonds: 0,
        // golds: 0,
        // isFirstLogin: false,
        nickName: "",
        sex: 1
        // userId: 0,
        // vip: 0
      }
    };
  },
  created() {},
  methods: {
    //完成按钮
    complete_button() {
      this.$http({
        method: "post",
        url: this.$api.API_BM + "api/v1/user/register",
        data: this.register,
        headers: {
          "Access-Token": this.$getCookie("temporary_accessToken"),
          userId: this.$getCookie("temporary_userId")
        }
      })
        .then(result => {
          if (result.data.code == 1) {
            this.$setCookie(
              "accessToken",
              this.$getCookie("temporary_accessToken")
            );
            this.$setCookie("userId", this.$getCookie("temporary_userId"));

            //删除临时数据--临时数据产生只有未填写完资料才出现
            this.$delCookie("temporary_accessToken");
            this.$delCookie("temporary_userId");

            this.$message({
              showClose: true,
              message: this.$t("user_data_add.sign_up_success"),
              type: "success"
            });
            this.router_link("/");
            this.$router.go(0);
          } else if (result.data.code == 102) {
            this.$message({
              showClose: true,
              message: this.$t("user_data_add.account_password_not_exist"),
              type: "error"
            });
          } else if (result.data.code == 1003) {
            this.$message({
              showClose: true,
              message: this.$t("user_data_add.nickname_already_exists"),
              type: "error"
            });
          } else if (result.data.code == 1001) {
            this.$message({
              showClose: true,
              message: this.$t("user_data_add.profile_exist"),
              type: "error"
            });
          } else {
            this.$message({
              showClose: true,
              message:  result.data.message,
              type: "error"
            });
          }
        })
        .catch(err => {
          console.log("err", err);
        });
    },
    //路由跳转
    router_link(href, query) {
      this.$router.push({
        path: href,
        query: query || {}
      });
    }
  }
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style  lang="scss" scoped  type="text/css">
.bm-box-content {
  width: 500px;
  height: 300px;
  background-color: #fff;
  padding: 20px 40px;
  h1 {
    padding: 15px 0;
    text-align: left;
  }
  .bm-gender-select {
    display: flex;
    justify-content: space-between;
    .active {
      border-color: #01925b;
    }
    .bm-button-sex {
      width: 160px;
      margin: 0 20px;
    }
  }
  .bm-complete {
    width: 100%;
    margin-top: 20px;
  }
}
</style>
