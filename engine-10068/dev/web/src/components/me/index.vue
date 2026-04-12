<template>
  <div>
    <div class="bm-my-index">
      <div class="bm-personal-box">
        <div class="bm-img-box ">
            <img src="http://p1.music.126.net/AIvo3OXdclsfak8-dwsezg==/109951163318209149.jpg?param=180y180" alt="">
        </div>
        <div class="personal-box f1">
            <div class="personal-top">
                <h1>安吉斯接口看</h1> ♂ <span>mvp</span>
            </div>
            <div class="personal-bottom">
                <span>金币</span>123123
                <span>魔方</span>123123
                <span>财富明细</span>
            </div>
        </div>
      </div>
      <div class="bm-reloading">
          <div class="bm-window">
              
          </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: "index",
  data() {
    return {};
  },
  created() {},
  methods: {}
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style  lang="scss" scoped type="text/css">
.bm-my-index {
  .bm-personal-box {
    margin: 0 auto;
    width: 90%;
    height: 200px;
    background-color: rgba(255, 255, 255, 0.5);
    display: flex;
    .bm-img-box {
      padding: 20px;
      img {
        border-radius: 100%;
      }
    }
    .personal-box {
      .personal-top {
        display: flex;
      }
      .personal-bottom {
        display: flex;
      }
    }
  }
  .bm-reloading {
    width: 90%;
    height: 400px;
    background-color: rgba(255, 255, 255, 0.5);
    margin: 10px auto;
    .bm-window{
        width:300px;
        height:100%;
        background-color: #fff;
    }
  }
}
</style>
