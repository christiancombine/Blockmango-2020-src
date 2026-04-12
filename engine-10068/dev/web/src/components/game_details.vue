<template>
  <div>
    <div class="bm-content-box">
      <div class="bm-content-box-top">
        <div class="bm-game-detils-img">
          <img :src="game_data.bannerPic[0]" alt="">
        </div>
        <div class="bm-game-details-info">
          <div class="bm-details-title">
            <h1>{{game_data.gameTitle}}</h1>
            <div class="bm-game-tag">
              <span v-for="(itm,index) in game_data.gameTypes" class="bm-game-tag-item" :key="index">{{itm}}</span>
            </div>
            <div class="bm-game-status">
              <div>{{online_number+$t('game_details.online_number')}}</div>
              <div><i class="el-icon-share"></i>{{game_data.praiseNumber}}</div>
            </div>
          </div>
          <div class="bm-details-play">
            <el-button class="bm-play-game-button" :data-disabled="!game_data.visitorEnter" @click="start_game_event">{{$t('game_details.play')}}</el-button>
          </div>
        </div>
      </div>
      <div class="bm-content-box-bottom">
        <h3>{{$t('game_details.description')}}</h3>
        <div class="bm-game-introduce">
          {{game_data.gameDetail}}
        </div>
      </div>

      <!-- 启动游戏&*下载客户端frame -->
      <div>
        <iframe id="bm-start-game-iframe" v-show="false" :src="start_game_src"></iframe>
      </div>

      <!-- 提示下载框 -->
      <div>
        <el-dialog :title="$t('game_details.tip')" :visible.sync="dialog_tips_download_show" width="30%" center>
          <span>{{$t('game_details.tips_download')}}</span>
          <div slot="footer" class="dialog-footer">
            <el-button class="download-button" type="primary" @click="download_button">{{$t('game_details.download_button')}}</el-button>
          </div>
        </el-dialog>
      </div>

      <!-- 提示登录 -->
      <div>
        <el-dialog title="提示" :visible.sync="dialog_visitor_show" width="30%" center>
          <span>{{$t('game_details.visitor_dont_play')}}</span>
          <div slot="footer" class="dialog-footer">
            <el-button  type="primary" @click="$emit('open_login_frame',dialog_visitor_show=false);">{{$t('game_details.login')}}</el-button>
          </div>
        </el-dialog>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: "game_details",
  data() {
    return {
      game_data: {
        bannerPic: []
      },
      //在线人数
      online_number: unescape(this.$route.query.onlineNumber),

      start_game_src:
        "12" ||
        "bg://start?isVisitor=0&nickName=visitor&userId=4195328&token=5283be6918214e267d870e2dfa1bcd54ad46f76d&ip=127.0.0.1&port=19130&gameTimestamp=123456789&lang=zh_CN&gameType=g1019&mapName=gameName-debug&mapUrl=http://static.sandboxol.cn/mini-game-map/map-debug.zip&strRootPath=./&strConfigPath=./document/",
      //启动游戏需要的参数
      start_game_query: {
        nickName:
          this.$getCookie("userData") != null
            ? JSON.parse(unescape(this.$getCookie("userData"))).nickName
            : this.$getCookie("nickName"), // 用户名字   -nickName
        userId: this.$getCookie("userId") || this.$getCookie("visitor_id"), //   -userId
        token:
          this.$getCookie("accessToken") ||
          this.$getCookie("visitor_accessToken"), //    -AccessToken
        ip: "", //  -服务器ip
        port: "", // 服务器端口
        gameTimestamp: "", // 服务器时间戳
        lang: "zh_CN", // 语言
        gameType: "", // 游戏类型
        mapName: "", // 地图名字
        mapUrl: "" // 地图地址
        // strRootPath: "./", // 根目录路径
        // strConfigPath: "./document/" // 配置路径
      },

      //弹窗显示
      dialog_tips_download_show: false,
      //游客弹框
      dialog_visitor_show: false
    };
  },
  created() {
    this.get_game_data();
  },
  methods: {
    //获取游戏详情
    get_game_data() {
      this.$http({
        method: "get",
        url:
          this.$api.API_BM_APPGAME +
          "api/v1/games/" +
          unescape(this.$route.query.gameId),
        headers: {
          language: "zh_CN"
        },
        data: {}
      })
        .then(result => {
          if (result.data.code == 1) {
            this.game_data = result.data.data;
            this.gameType = result.data.data.gameType;
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

    //启动游戏-获取签名-地图url
    start_game_event() {
      //游客不能试玩锁
      if (!this.game_data.visitorEnter) {
        this.dialog_visitor_show = true;
        return
      }
      /** 
        nickName		// 用户名字
        userId			// 用户ID
        token 			// 校验码
        ip 			// 用户IP
        port			// 网络端口
        gameTimestamp	// 时间戳
        lang 			// 语言
        gameType 		// 游戏类型
        mapName 		// 地图名字
        mapUrl 		// 地图地址
        strRootPath 		// 根目录路径
        strConfigPath		// 配置路径
      */
      this.$http({
        method: "get",
        url:
          this.$api.API_BM_APPGAME +
          "api/v1/game/auth?typeId=" +
          unescape(this.$route.query.gameId) +
          "&gameVersion=" +
          this.game_data.version,
        headers: {
          language: "zh_CN"
        },
        data: {}
      })
        .then(result => {
          if (result.data.code == 1) {
            this.start_game_query.gameTimestamp = result.data.data.timestamp;
            this.get_map_and_other(result.data.data);
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
    // 获取地图地址相关
    get_map_and_other(auth_data) {
      this.$http({
        method: "post",
        // url:"http://120.92.133.131:9902/",
        url: this.$api.API_BM_q + "dispatch",
        headers: {
          language: "zh_CN",
          "x-shahe-uid": this.$getCookie("userId"),
          "x-shahe-token": auth_data.token
        },
        data: {
          name:
            this.$getCookie("userData") != null
              ? JSON.parse(unescape(this.$getCookie("userData"))).nickName
              : this.$getCookie("nickName"),
          rid: auth_data.region,
          ever: 10010, //未知参数
          clz: 0, //未知参数
          pioneer: true //未知参数
        }
      })
        .then(result => {
          if (result.data.code == 1) {
            this.start_game_query.mapUrl = result.data.data.downurl;
            this.start_game_query.mapName = result.data.data.mname;
            var gaddr = result.data.data.gaddr;
            this.start_game_query.ip = gaddr.substring(0, gaddr.indexOf(":"));
            this.start_game_query.post = gaddr.substring(
              gaddr.indexOf(":"),
              gaddr.length - 1
            );
            this.start_game_query.gameType = unescape(this.$route.query.gameId);

            //弹框提示下载
            this.dialog_tips_download_show = true;

            //正式启动游戏
            this.start_game_src =
              "bg://start?isVisitor=0&" +
              this.$encodeSearchParams(this.start_game_query);
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

    //下载按钮
    download_button() {
      this.start_game_src =
        "https://ks3-cn-shanghai.ksyun.com/sandbox-region/blockman-pc/test/blockmanlauncher_boxed.exe";
      setTimeout(() => {
        this.start_game_src = "";
      }, 1000);
    }
  }
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style lang="scss" scoped type="text/css">
.bm-content-box {
  width: 800px;
  height: 400px;
  background-color: rgba(255, 255, 255, 0.8);
  text-align: left;
  .bm-content-box-top {
    display: flex;
    background-color: #fff;
    .bm-game-detils-img {
      img {
        width: 500px;
        height: 270px;
      }
    }
    .bm-game-details-info {
      padding: 10px;
      width: 100%;
      h1 {
        font-size: 25px;
        font-weight: 600;
      }
      .bm-game-tag {
        padding: 30px 0;
        .bm-game-tag-item {
          padding: 3px 6px;
          background-color: rgba(0, 0, 0, 0.1);
          border-radius: 5px;
        }
      }
      .bm-game-status {
        display: flex;
        justify-content: space-between;
        div {
          width: 100%;
        }
      }
    }
    .bm-play-game-button {
      margin: 50px 0;
      width: 100%;
    }
  }
  .bm-content-box-bottom {
    padding: 10px;
    h3 {
      padding-bottom: 10px;
    }
    .bm-game-introduce {
      font-size: 14px;
    }
  }
  .dialog-footer {
    .download-button {
      width: 100%;
    }
  }
}
</style>
