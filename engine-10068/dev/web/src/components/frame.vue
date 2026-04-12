<template>
  <div>
      <el-container>
        
        <!-- 顶部栏显示部分 -->
        <el-header class="bm-header" >
            

          <!-- 已登录 -->
          <div class="user"  v-show="accessToken">
            <img class="bm-user-img noselect cursor" @click="window_page_location('me.html')" :src="user.picUrl||'http://p1.music.126.net/AIvo3OXdclsfak8-dwsezg==/109951163318209149.jpg?param=180y180'" alt=""><span class="bm-sex">{{user.sex==0?'♂':'♀'}}</span>   <span @click="window_page_location('me.html')" class="cursor">{{user.nickName}}</span>

            
            <span v-on:mouseenter="test_dropdown">
                <el-tooltip  placement="top" effect="light">
                  <div slot="content" >
                    <i class="el-icon-edit bm-treasure-box">{{user.golds}}</i>
                    <br/>
                    <i class="el-icon-edit">{{user.diamonds}}</i> <el-button type="text" class=""  @click="$router.push({path: '/login'})">{{$t("frame.recharge")}}</el-button>
                  </div>
                  <el-button  type="text" class="bm-cion-vip cursor">{{$t("frame.treasure")}}</el-button>
                </el-tooltip>
            </span>

            <span class="bm-logout noselect cursor"  @click="exit_out">{{$t("frame.sign_out")}}</span>
          </div>

          <!-- 未登录 -->
          <div class="user" v-show="!accessToken">
            <div class="visitor">
              <p>{{visitor_user_data.nickName}}</p>
            </div>
            <el-button type="text"  @click="login_frame_show=true">{{$t("frame.login")}}</el-button>
          </div>
        </el-header>
         
          
        <el-container>
            <!-- 左边栏显示部分 -->
            <bm-aside :asideMenu="asideMenu"></bm-aside>

            <!-- main显示部分 -->
            <el-main class="bm-main-box">
              <router-view @open_login_frame="open_login_frame"/>
            </el-main>

        </el-container>


      </el-container>
      <!-- 登录弹窗 -->
      <div id="login" >
        <el-dialog  width="450px" :title="$t('frame.sign_in')"  :visible.sync="login_frame_show">
          <div class="bm-box-content-login bm-box-content">
          <el-form :model="login_data" status-icon :rules="login_data_rules" ref="login_data" >
              <el-form-item  prop="uid">
                <el-input @keyup.enter.native="login_button" class="bm-form-input bm-form-input-account-number" v-model="login_data.uid" prefix-icon="el-icon-search" clearable :placeholder="$t('frame.placeholder_account')"></el-input>
              </el-form-item>
              <el-form-item  prop="password">
                <el-input @keyup.enter.native="login_button" class="bm-form-input bm-form-input-password" type="password" v-model="login_data.password" prefix-icon="el-icon-search" clearable :placeholder="$t('frame.placeholder_password')"></el-input>
              </el-form-item>
              <el-button sise="medium"  type="primary"  class="bm-form-button-login" @click="login_button" >{{$t('frame.sign_in_button')}}</el-button>
              <div class="bm-forget-and-sign-in">
                <el-button type="text" @click="forget_password_frame_show=true,login_frame_show=false">{{$t('frame.Forgot_password')}}</el-button>
                <el-button type="text" @click="signin_frame_show=true,login_frame_show=false">{{$t('frame.sign_up')}}</el-button>
              </div>
          </el-form>
        </div>
        </el-dialog>
      </div>

      <!-- 注册弹窗 -->
      <div id="sign-in" >
        <el-dialog  width="450px"  :title="$t('frame.sign_up')"  :visible.sync="signin_frame_show">
          <div class="bm-box-content-signin bm-box-content">
            <el-form :model="register" status-icon :rules="register_rules" ref="register" >
                <el-form-item  prop="uid">
                  <el-input class="bm-form-input bm-form-input-account-number" @keyup.enter.native="signin_button" v-model="register.uid" prefix-icon="el-icon-search" clearable :placeholder="$t('frame.placeholder_account')"></el-input>
                </el-form-item>
                <el-form-item  prop="password">
                  <el-input class="bm-form-input bm-form-input-password" type="password" @keyup.enter.native="signin_button" v-model="register.password" prefix-icon="el-icon-search" clearable :placeholder="$t('frame.placeholder_password')"></el-input>
                </el-form-item>
                <el-form-item  prop="checkPass">
                  <el-input class="bm-form-input bm-form-input-password" type="password" @keyup.enter.native="signin_button" v-model="register.checkPass" prefix-icon="el-icon-search" clearable :placeholder="$t('frame.placeholder_re_password')"></el-input>
                </el-form-item>
                <el-button  type="primary"  sise="medium" class="bm-form-button-login" @click="signin_button">{{$t('frame.sign_up_button')}}</el-button>
                <div class="bm-forget-and-sign-in">
                  <el-button type="text" @click="$router.push({path: '/'})"></el-button>
                  <el-button type="text" @click="signin_frame_show=false,login_frame_show=true">{{$t('frame.have_account_sign_in')}}</el-button>
                </div>
            </el-form>
          </div>
        </el-dialog>
      </div>

      <!-- 找回密码弹窗 -->
      <div id="forget-password" >
        <el-dialog :close-on-click-modal="false"  width="450px"  :title="$t('frame.retrieve_password')"  :visible.sync="forget_password_frame_show">
          <div class="bm-box-content">
            <h1>{{$t('frame.retrieve_password')}}</h1>
            <el-form :model="forget_password_api_data" ref="forget_password_api_data" >
              <el-form-item prop="email" :rules="[
                  { required: true, message: $t('frame.please_enter_email'), trigger: 'blur' },
                  { type: 'email', message: $t('frame.email_tips_rule'), trigger: ['blur', 'change'] }
                ]">
                <el-input  max="30"  clearable v-model="forget_password_api_data.email" :placeholder="$t('frame.please_enter_email')" ></el-input>
              </el-form-item>
              <el-form-item>
                <el-button class="bm-next-button" :loading="!send_status_lock" v-show="!send_status" @click="change_email_next">{{$t('frame.next')}}</el-button>
                <el-button class="bm-next-button" v-show="send_status" disabled >{{count_down+$t('frame.Resend_after_seconds')}}</el-button>
              </el-form-item>
              <p v-show="send_status">{{$t('frame.reset_password')}}{{forget_password_api_data.email}}</p>
            </el-form>
          </div>
        </el-dialog>
      </div>
  </div>
</template>

<script>

export default {
  name: "frame",
  props: ["asideMenu"],
  data() {
    /*
      * 登录相关验证
      */
    //账号规则
    var check_uid = (rule, value, callback) => {
      if (!value) {
        return callback(new Error(this.$t("frame.account_cannot_empty")));
      } else if (value.length < 4) {
        return callback(new Error(this.$t("frame.account_min_four_length")));
      } else {
        callback();
      }
    };
    //密码规则
    var validate_pass = (rule, value, callback) => {
      if (value === "") {
        callback(new Error(this.$t("frame.placeholder_password")));
      } else if (value.length < 6) {
        return callback(new Error(this.$t("frame.account_max_six_length")));
      } else {
        callback();
      }
    };

    /*
      * 注册相关验证
      */
    //二次密码规则
    var validate_pass2 = (rule, value, callback) => {
      if (value === "") {
        callback(new Error(this.$t("frame.placeholder_re_password")));
      } else if (value !== this.register.password) {
        callback(new Error(this.$t("frame.placeholder_again_password_agreed")));
      } else {
        callback();
      }
    };
    return {
      /*
        * 登录相关
        */
      login_frame_show: false,
      login_data: {
        appType: "web",
        uid: "",
        platform: "",
        password: ""
      },
      login_data_rules: {
        uid: [{ validator: check_uid, trigger: "blur" }],
        password: [{ validator: validate_pass, trigger: "blur" }]
      },

      /*
        * 注册相关
        */
      signin_frame_show: false,
      register: {
        appType: "web",
        uid: "",
        password: "",
        checkPass: ""
      },
      register_rules: {
        uid: [{ validator: check_uid, trigger: "blur" }],
        password: [{ validator: validate_pass, trigger: "blur" }],
        checkPass: [{ validator: validate_pass2, trigger: "blur" }]
      },

      /*
        * 找回密码相关
        */
      forget_password_frame_show: false,
      forget_password_api_data: {
        email: ""
      },
      send_status: false,
      send_status_lock: true,
      count_down: 60,

      /*
        * 用户信息相关
        */
      user: {
        nickName: "",
        picUrl: "",
        golds: 0,
        sex: null
      },
      visitor_user_data: {
        nickName: this.$getCookie("nickName") || ""
      },
      visitor_login_api: {
        appType: "web",
        // deviceId: "string",
        imei: this.$getCookie("visitor_mac") || this.$mac()
        // os: "string",
        // uuid: "string"
      },
      accessToken: this.$getCookie("accessToken")
    };
  },
  created() {
    this.get_user_info();
    if (
      !this.accessToken &&
      (!this.$getCookie("visitor_accessToken") &&
        !this.$getCookie("visitor_mac") &&
        !this.$getCookie("visitor_id"))
    ) {
      this.visitor_login();
    }
  },
  methods: {
   
    /*
      * 游客登录/退出
      */
    //获取用户信息
    get_user_info() {
      if (this.$getCookie("accessToken") && this.$getCookie("userId")) {
        //必须要有令牌才能请求个人数据
        this.$http({
          method: "post",
          url: this.$api.API_BM + "api/v1/user/details/info",
          data: {}
        })
          .then(result => {
            if (result.data.code == 1) {
              this.user = result.data.data;
              this.$setCookie("userData", JSON.stringify(result.data.data));
            } else {
              this.$message({
                showClose: true,
                message: result.data.message,
                type: "error"
              });
            }
          })
          .catch(err => {
            if (JSON.stringify(err).indexOf("401") >= 0) {
              this.$delCookie("userId");
              this.$delCookie("accessToken");
              this.$delCookie("userData");
              this.accessToken = this.$getCookie("accessToken");
              this.visitor_login();
            }
          });
      }
    },

    //退出登录
    exit_out() {
      this.$http({
        method: "put",
        url: this.$api.API_BM + "api/v1/user/login-out",
        data: {}
      })
        .then(result => {
          console.log(result.data);
          if (result.data.code == 1) {
            this.$delCookie("userId");
            this.$delCookie("accessToken");
            this.$delCookie("userData");
            this.$router.push({ path: "/" });
            this.$router.go(0);
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
    },

    //游客自动登录
    visitor_login() {
      this.$http({
        method: "post",
        url: this.$api.API_BM + "/api/v1/visitor",
        data: this.visitor_login_api
      })
        .then(result => {
          if (result.data.code == 1) {
            this.visitor_user_data = result.data.data;
            this.$setCookie("visitor_id", result.data.data.id);
            this.$setCookie(
              "visitor_accessToken",
              result.data.data.accessToken
            );
            this.$setCookie("visitor_mac", this.visitor_login_api.imei);
            this.$setCookie("nickName", this.visitor_user_data.nickName);
          } else {
            this.$message({
              showClose: true,
              message: result.data.message,
              type: "error"
            });
          }
        })
        .catch(err => {});
    },

    /*
      * 用户登录相关
      */
    //登录按钮
    login_button() {
      this.$refs["login_data"].validate(valid => {
        console.log(valid);
        if (valid) {
          this.$http({
            method: "post",
            url: this.$api.API_BM + "api/v1/login",
            // url: "http://120.92.158.119/user/game/api/v1/login",
            data: this.login_data
          })
            .then(result => {
              console.log(result.data);
              if (result.data.code == 1) {
                this.$setCookie("accessToken", result.data.data.accessToken);
                this.$setCookie("userId", result.data.data.userId);
                this.$router.push({ path: "/" });
                this.$router.go(0);
              } else if (result.data.code == 102) {
                this.$message({
                  showClose: true,
                  message: this.$t("frame.account_password_not_exist"),
                  type: "error"
                });
              } else if (result.data.code == 108) {
                this.$message({
                  showClose: true,
                  message: this.$t("frame.account_password_not_exist"),
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
            .catch(err => {});
        } else {
          return false;
        }
      });
    },
    //子主件触发登录弹框事件处理
    open_login_frame(...allData) {
      this.login_frame_show = true;
    },

    /*
      * 注册相关
      */
    //注册按钮
    signin_button() {
      this.$refs["register"].validate(valid => {
        if (valid) {
          this.$http({
            method: "post",
            url: this.$api.API_BM + "api/v1/register",
            data: this.register
          })
            .then(result => {
              if (result.data.code == 1) {
                //注册成功==1
                this.$setCookie(
                  "temporary_accessToken",
                  result.data.data.accessToken
                );
                this.$setCookie("temporary_userId", result.data.data.userId);
                //to do ....
                this.$router.push({ path: "/user_data_add" });
              } else if (result.data.code == 101) {
                this.$message({
                  showClose: true,
                  message: this.$t("frame.account_already_exists"),
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
            .catch(err => {});
        } else {
          return false;
        }
      });
    },

    /*
      * 找回密码相关
      */
    //下一步按钮(发送邮箱验证码)
    change_email_next() {
      this.$refs["forget_password_api_data"].validate(valid => {
        if (this.send_status_lock) {
          this.send_status_lock = false;
        } else {
          return;
        }
        if (valid) {
          this.$http({
            method: "post",
            url:
              this.$api.API_BM +
              "api/v1/emails/password/reset?email=" +
              this.forget_password_api_data.email,
            data: {}
          })
            .then(result => {
              console.log(result.data);
              if (result.data.code == 1) {
                this.count_down_event();
                this.send_status = false;
                this.send_status_lock = true;
              } else if (result.data.code == 116) {
                this.$message({
                  showClose: true,
                  message: this.$t("frame.email_not_bind_user"),
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
            .catch(err => {});
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
    },

    //财富
    test_dropdown(e) {
      if (e) {
        this.get_user_info();
      }
    },

    //多页面跳转
    window_page_location(href){
      window.location.href=href
    }
  }
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style  lang="scss"  type="text/css">
.bm-header {
  background-color: #01925b;
  color: #fff;
  line-height: 50px;
  height: 50px !important;
  text-align: right;
  font-size: 12px;
  position: fixed;
  width: 100%;
  z-index: 5;
  .bm-user-img {
    width: 30px;
    height: 30px;
    border-radius: 100px;
    margin-right: 10px;
  }
  .bm-cion-vip {
    padding: 3px;
    background-color: rgb(160, 160, 160);
    border-radius: 3px;
    color: #fff;
    font-size: 12px;
  }
  .bm-treasure-box {
    i {
      font-size: 17px !important;
    }
  }
  .bm-cion-number {
    width: 140px;
  }
  .bm-cion {
    position: relative;
    margin-left: 45px;
    &::after {
      content: "";
      width: 20px;
      height: 20px;
      position: absolute;
      left: -20px;
      top: 50%;
      transform: translateY(-50%);
      background-image: url("https://s2.music.126.net/store/web/img/jifen.png?1154b396a57d6f4f8dfc22e5983e5970");
      background-size: contain;
    }
  }
  .bm-logout {
    text-decoration: underline;
    padding: 0 10px;
  }
  .visitor {
    display: inline-block;
    padding-right: 20px;
  }
}
.bm-left-aside {
  position: fixed;
  top: 50px;
  left: 0;
  height: 100%;
  text-align: left;
}
.bm-main-box {
  background-color: #e3c99e;
  margin-left: 200px;
  margin-top: 50px;
}
.el-aside {
  color: #333;
}
.bm-box-content-login {
  height: 250px;
}
.bm-box-content-signin {
  height: 300px;
}
.bm-box-content {
  background-color: #fff;
  padding: 20px 40px;
  .bm-form-button-login {
    width: 100%;
  }
  .bm-forget-and-sign-in {
    display: flex;
    justify-content: space-between;
  }
  h1 {
    padding: 15px 0;
    text-align: left;
  }
  .bm-next-button {
    margin: 30px 0;
    width: 100%;
  }
}
</style>
