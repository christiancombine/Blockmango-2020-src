<template>
  <div>
    <el-aside width="200px" class="bm-left-aside" style="background-color: #eee">
        <el-menu active-text-color="#01925b" router >
            <el-menu-item :index="item.path" v-for="(item,index) in asideMenu" :key="index">
                <i class="el-icon-menu"></i>
                <span slot="title">{{item.title}}</span>
            </el-menu-item>
        </el-menu>
    </el-aside>
  </div>
</template>

<script>
export default {
  name: "bm-aside",
  props: ['asideMenu'],
  data() {
    return {
      /**
       * 侧边栏
       */
    //   aside_menu: [
    //     {
    //       path: "/",
    //       title: "我的主页"
    //     },
    //     {
    //       path: "/security",
    //       title: "账号安全"
    //     },
    //     {
    //       path: "/language",
    //       title: "语言"
    //     }
    //   ]

    };
  },
  created() {
          this.$i18n.locale = this.$getCookie("language").slice(
        this.$getCookie("language").length - 2,
        this.$getCookie("language").length)
  },
  methods: {}
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style  lang="scss" scoped type="text/css">
</style>
