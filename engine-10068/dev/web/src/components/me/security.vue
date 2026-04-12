<template>
  <div class="hello">
    <div class="bm-security-form-title">
      <el-breadcrumb separator-class="el-icon-arrow-right">
        <el-breadcrumb-item>{{$t('security.account_security')}}</el-breadcrumb-item>
      </el-breadcrumb>
    </div>
    <div class="bm-security-form">
      <el-form  label-width="100px" class="demo-dynamic">
        <el-form-item class="bm-form-list" :label="$t('security.account')">
          <span>{{user_data.account}}</span>
        </el-form-item>
        <el-form-item class="bm-form-list" :label="$t('security.password')">
          <span>**********</span><router-link to="/security/change_password"><el-button type="text" class="fr bm-change-button-margin" >{{$t('security.click_change')}}</el-button></router-link>
        </el-form-item>
        <el-form-item class="bm-form-list" :label="$t('security.email')" v-show="user_data.email!==''">
          <span>{{user_data.email}}</span><el-button disabled type="text" class="fr bm-change-button-margin">{{$t('security.binded')}}</el-button>
        </el-form-item>
        <el-form-item class="bm-form-list" :label="$t('security.email')" v-show="user_data.email===''">
          <span>{{user_data.email}}</span><el-button @click="dialog_form_visible = true" type="text" class="fr bm-change-button-margin">{{$t('security.not_bind')}}</el-button>
        </el-form-item>
      </el-form>
    </div>

    <!-- 邮箱登录验证 -->
      <el-dialog  width="450px" :title="$t('security.bind_email')"  :visible.sync="dialog_form_visible">
        <el-form :model="change_form" ref="change_form">
          <el-form-item  prop="email" :rules="[
            {  required: true, message: $t('security.please_enter_email'), trigger: 'blur' },
            { type: 'email', message: $t('security.please_enter_correct_email'), trigger: ['blur', 'change'] }
          ]">
            <el-input v-model="change_form.email" @keyup.enter.native="bind_email_veriyfy" :placeholder="$t('security.please_enter_email')" auto-complete="off"></el-input>
          </el-form-item>
          </el-form>

          
          <el-form :model="change_form" ref="change_form1">
          <el-form-item  prop="verifyCode"  :rules="[
            { required: true,  message:$t('security.please_enter_auth_code') , trigger: 'blur' },
             { min: 1, max: 5, message:$t('security.length_min3_max5'), trigger: 'blur' }
          ]">
            <div style=" display: flex;">
              <el-input v-model="change_form.verifyCode"  @keyup.enter.native="bind_email_button"  :placeholder="$t('security.please_enter_auth_code')" auto-complete="off"></el-input><el-button @click="bind_email_veriyfy" v-show="!send_status" :loading="verify_code_status">{{verify_code_status?$t('security.sending'):$t('security.send_auth_code')}}</el-button><el-button v-show="send_status" disabled>{{count_down}}秒可重试</el-button>
            </div>
          </el-form-item>
          <el-button type="primary" class="bm-confirm" @click="bind_email_button">绑 定</el-button>
        </el-form>
    </el-dialog>
  </div>
</template>

<script>
export default {
  name: "security",
  data() {
    return {
      user_data: JSON.parse(unescape(this.$getCookie("userData"))),
      change_form: {
        email: "",
        verifyCode: ""
      },
      //验证码倒计时状态 , 倒计时 , 弹框状态 , 校验码按钮
      send_status: false,
      count_down: 60,
      dialog_form_visible: false,
      verify_code_status: false
    };
  },
  created() {
  },
  methods: {
    //发送邮箱验证码
    bind_email_veriyfy() {
      this.$refs["change_form"].validate(valid => {
        if (valid) {
          this.verify_code_status = true;
          this.$http({
            method: "post", //http://mods.sandboxol.com/user/api/v1/emails/%7Bemail%7D?email=1387884@163.com
            url:
              this.$api.API_BM +
              "api/v1/emails/%7Bemail%7D?email=" +
              this.change_form.email,
            data: {}
          })
            .then(result => {
              console.log(result.data);
              if (result.data.code == 1) {
                this.verify_code_status = false;
                this.count_down_event();
                this.send_status = true;
                this.$message({
                  showClose: true,
                  message:this.$t('security.auth_code_sended') ,
                  type: "success"
                });
              } else if (result.data.code == 111) {
                this.$message({
                  showClose: true,
                  message:this.$t('security.email_error') ,
                  type: "error"
                });
              } else if (result.data.code == 112) {
                this.$message({
                  showClose: true,
                  message:this.$t('security.email_invalid') ,
                  type: "error"
                });
              } else if (result.data.code == 113) {
                this.$message({
                  showClose: true,
                  message: this.$t('security.email_binded_user') ,
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
              console.log("err", err);
            });
        } else {
          console.log("error submit!!");
          return false;
        }
      });
    },
    //校验码输入,绑定操作
    bind_email_button() {
      this.$refs["change_form1"].validate(valid => {
        if (valid) {
          this.$http({
            method: "post", //http://mods.sandboxol.com/user/api/v1/emails/%7Bemail%7D?email=1387884@163.com
            url: this.$api.API_BM + "/api/v1/users/bind/email",
            data: this.change_form
          })
            .then(result => {
              console.log(result.data);
              if (result.data.code == 1) {
                this.$message({
                  showClose: true,
                  message:this.$t('security.email_bind_success') ,
                  type: "success"
                });
                //清空输入框操作
                this.change_form.email = "";
                this.change_form.verifyCode = "";
              } else if (result.data.code == 113) {
                this.$message({
                  showClose: true,
                  message:this.$t('security.email_binded_user') ,
                  type: "error"
                });
              } else if (result.data.code == 114) {
                this.$message({
                  showClose: true,
                  message: this.$t('security.user_binded_email') ,
                  type: "error"
                });
              } else if (result.data.code == 107) {
                this.$message({
                  showClose: true,
                  message:this.$t('security.auth_code_error'),
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
.bm-security-form-title {
  text-align: left;
  padding-bottom: 20px;
  color: #000;
}
.bm-security-form {
  width: 500px;
  // margin:0 auto;
  background-color: rgba(255, 255, 255, 0.8);
  padding: 20px;
  .bm-form-list {
    text-align: left;
  }
  .bm-change-button-margin {
    margin-right: 70px;
    text-decoration: underline;
    a {
      color: #409eff;
    }
  }
}
.bm-confirm {
  width: 100%;
}
</style>
