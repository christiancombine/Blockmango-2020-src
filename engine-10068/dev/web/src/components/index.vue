<template>
  <div>
    <div class="bm-all-game-title">
      <h1>{{$t("index.game")}}</h1>
    </div>
    <div class="bm-all-game-list">
      <el-row :gutter="10">
        <el-col class="bm-game-item"  :span="24/6" v-for="(item, index) in all_game_list" :offset="6" :key="index" >
           <router-link  :to="{path:'/game_details',query:item}">
            <el-card class="cursor" shadow="hover">
              <img :src="item.gameCoverPic" class="image">
              <div class="bm-content-info">
                <span>{{item.gameTitle}}</span>
                <div class="bottom clearfix">
                  <span>{{item.onlineNumber+$t("index.onlineNumber")}}</span>
                  <span><i class="el-icon-share"></i>{{item.praiseNumber}}</span>
                </div>
              </div>
            </el-card> 
          </router-link> 
        </el-col>
      </el-row>
    </div>
  </div>
</template>

<script>
export default {
  name: "index",
  data() {
    return {
      all_game_list: {}
    };
  },
  created() {
    this.get_all_game_list();
  },
  methods: {
    //获取游戏列表
    get_all_game_list() {
      this.$http({
        method: "get", //http://mods.sandboxol.com/game/api/v1/games/recommendation
        url: this.$api.API_BM_APPGAME+"api/v1/games?pageNo=0&pageSize=100&orderType=onlineTime&typeId=0&order=dsc&isPublish=1",
        // url: this.$api.API_BM_GAME + "api/v1/games/recommendation/all",
        data: {}
      })
        .then(result => {
          if (result.data.code == 1) {
            this.all_game_list = result.data.data.data;
          } else {
            this.$message({
              showClose: true,
              message: result.data.message,
              type: "error"
            });
          }
        })
        .catch(err => {});
    }
  }
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style  lang="scss"  type="text/css">
.bm-all-game-title {
  text-align: left;
  padding-bottom: 20px;
}
.bm-all-game-list {
  .bm-game-item {
    margin-bottom: 10px;
    margin-left: 0px;
  }
  .el-card__body {
    padding: 0 !important;
  }
  .bm-content-info {
    padding: 10px;
    white-space: nowrap;
    text-align: left;
    .time {
      font-size: 13px;
      color: #999;
    }
    .bottom {
      margin-top: 13px;
      line-height: 12px;
    }
    .button {
      padding: 0;
      float: right;
    }

    .clearfix:before,
    .clearfix:after {
      display: table;
      content: "";
    }
    .clearfix:after {
      clear: both;
    }
  }
  .image {
    width: 100%;
    min-height: 256px;
    display: block;
  }
}
.is-hover-shadow:hover {
  box-shadow: 0 2px 12px 0 rgba(1, 146, 91, 0.6) !important;
}
</style>
