<template>
  <div id="app">
    <router-view pageName=""/>
  </div>
</template>

<script>
export default {
  name: "App"
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
