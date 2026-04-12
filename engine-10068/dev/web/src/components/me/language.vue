<template>
  <div>
    <div class="bm-language">
        <el-select v-model="select_value" :placeholder="select_value">
            <el-option
            v-for="(item,index) in language_list"
            :key="index"
            :label="item.label"
            :value="item.code">
            </el-option>
        </el-select>
        <el-button @click="$setCookie('language',select_value),$router.go(0)">确定</el-button>
    </div>
  </div>
</template>

<script>
export default {
  name: "index",
  data() {
    return {
        language_list:[
            {
                label:"简体中文",
                code:"zh_CN"
            },
            {
                label:"English",
                code:"en_US"
            },
        ],
        select_value:this.$getCookie("language")
    };
  },
  created() {},
  methods: {
  }
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style  lang="scss" scoped type="text/css">
.bm-language{

}
</style>
