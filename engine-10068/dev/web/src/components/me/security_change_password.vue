<template>
  <div>
    <div class="bm-security-password-title">
      <el-breadcrumb separator-class="el-icon-arrow-right">
        <!-- <el-breadcrumb-item :to="{ path: '/security' }">账号安全</el-breadcrumb-item> -->
        <el-breadcrumb-item>{{$t('security_change_password.account_security')}}</el-breadcrumb-item>
        <el-breadcrumb-item>{{$t('security_change_password.change_password')}}</el-breadcrumb-item>
      </el-breadcrumb>
    </div>
    <div class="bm-security-change-password">
      <el-form ref="api_data" status-icon :model="api_data" label-width="130px">
        <el-form-item :label="$t('security_change_password.old_password')" :rules="form_rule" prop="oldPassword">
          <el-input  @keyup.enter.native="confirm_change_password" type="password" :placeholder="$t('security_change_password.old_password')" clearable v-model="api_data.oldPassword"></el-input>
        </el-form-item>
        <el-form-item :label="$t('security_change_password.new_password')" :rules="form_rule2" prop="newPassword">
          <el-input  @keyup.enter.native="confirm_change_password" type="password" :placeholder="$t('security_change_password.new_password')" clearable v-model="api_data.newPassword"></el-input>
        </el-form-item>
        <el-form-item :label="$t('security_change_password.renew_password')" :rules="form_renew_password_rule" prop="renew_password">
          <el-input  @keyup.enter.native="confirm_change_password" type="password" :placeholder="$t('security_change_password.renew_password')" clearable v-model="api_data.renew_password"></el-input>
        </el-form-item>
          <el-button class="bm-confirm" @click="confirm_change_password">{{$t('security_change_password.change')}}</el-button>
      </el-form>
    </div>
  </div>
</template>

<script>
export default {
  name: "change_password",
  data() {
    var validatePass = (rule, value, callback) => {
      if (value === "") {
        callback(new Error(this.$t("security_change_password.renew_password")));
      } else if (value !== this.api_data.newPassword) {
        callback(new Error(this.$t("security_change_password.rules_password_not_same")));
      } else {
        callback();
      }
    };
    return {
      api_data: {
        oldPassword: "",
        newPassword: "",
        renew_password: ""
      },
      //验证规则
      form_rule: [
        { required: false,  message: this.$t("security_change_password.old_password"), trigger: "blur" },
        { max: 20, min: 6, message:this.$t("security_change_password.min6length") , trigger: ["blur", "change"] }
      ],
      form_rule2: [
        {  message:this.$t("security_change_password.old_password"), trigger: "blur" },
        { max: 20, min: 6, message: this.$t("security_change_password.min6length"), trigger: ["blur", "change"] }
      ],
      form_renew_password_rule: [{ validator: validatePass, trigger: "blur" }]
    };
  },
  created() {},
  methods: {
    //修改密码按钮
    confirm_change_password() {
      this.$refs["api_data"].validate(valid => {
        if (valid) {
          this.$http({
            method: "post",
            url:this.$api.API_BM +"api/v1/user/password/modify",
            data: this.api_data,
            headers: {
              language: "zh-CN",
            },
          })
            .then(result => {
              if (result.data.code == 1) {
                this.$message({
                  showClose: true,
                  message:this.$t("security_change_password.change_success") ,
                  type: "success"
                });
                this.$router.push({path: '/security'})
              } else if (result.data.code == 108) {
                this.$message({
                  showClose: true,
                  message:this.$t("security_change_password.old_password_error")  ,
                  type: "error"
                });
              } else {
                this.$message({
                  showClose: true,
                  message: result.data.message,
                  type: "error"
                });
              }
            })
            .catch(err => {
            });
        } else {
          return false;
        }
      });
    },
    //倒计时
    count_down_event() {
      var time = this.count_down;
      var timer = setInterval(res => {
        if (this.count_down == 0) {
          this.count_down = time;
          clearInterval(timer);
          this.send_status = false;
        } else {
          this.send_status = true;
          this.count_down -= 1;
        }
      }, 1000);
    }
  }
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style  lang="scss" scoped type="text/css">
.bm-security-password-title {
  text-align: left;
  padding-bottom: 20px;
  a {
    color: #409eff;
  }
}
.bm-security-change-password {
  width: 700px;
  // margin:0 auto;
  background-color: rgba(255, 255, 255, 0.8);
  padding: 20px;
}
.bm-confirm{
 width:200px;
}
</style>
