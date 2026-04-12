<template>
  <div class="bm-avatar">
    <el-breadcrumb separator-class="el-icon-arrow-right">
      <el-breadcrumb-item>avatar</el-breadcrumb-item>
    </el-breadcrumb>
    <div class="bm-content ">
      <div class="bm-show-box" id="bmox">

      </div>
      <div class="bm-select-box">
        <el-menu :default-active="activeIndex"  class="el-menu-demo" mode="horizontal"  @select="get_dress_data">
          <el-menu-item index="0">全部</el-menu-item>
          <el-menu-item index="1">服饰</el-menu-item>
          <el-menu-item index="2">发饰</el-menu-item>
          <el-menu-item index="3">配件</el-menu-item>
          <el-menu-item index="4">脸型</el-menu-item>
          <el-menu-item index="6">肤色</el-menu-item>
        </el-menu>
        <div class="bm-content-avatar clearfix">
            <div  @click="put_dress_data(item)" v-for="(item,index) in avatar_data" :key="index">
              <el-card class="bm-card-item" :class="item.status==0?'':'bm-avtive'">
                  <img :src="item.iconUrl" alt="">
                  <div>{{item.name}}</div>
              </el-card>
            </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
// import * as THREE from "three";
// var Zlib = require("../../node_modules/three/examples/js/libs/inflate.min.js");
// window.Zlib = Zlib.Zlib;
window.THREE = require("Three");
import * as dat from "dat.gui";

require("../../node_modules/three/examples/js/loaders/OBJLoader.js");
require("../../node_modules/three/examples/js/loaders/TGALoader.js");
require("../../node_modules/three/examples/js/loaders/MTLLoader.js");
require("../../node_modules/three/examples/js/controls/OrbitControls.js");
import Stats from "../../static/util/stat.js";

export default {
  name: "avatar",
  data() {
    return {
      //初始化webgl相关
      renderer: null,
      camera: null,
      scene: null,
      gui: null,
      datGui: null,
      light: null,
      stats: null,
      controls: null,

      //模型装饰
      file_base_src: "../../static/file/new/",
      // file_base_src: "../../static/file/resource/widget/",
      file_modules_src: "../../static/file/resource/modules",
      module_data: {
        hair: {},
        shoes: {
          resourceId: "custom_shoes.2"
        },
        tops: {
          resourceId: "clothes_tops.4"
        }
      },

      //页面
      activeIndex: "0",
      avatar_index: 0,

      //装饰列表
      avatar_data: []
    };
  },
  mounted() {
    this.draw();
    this.get_dress_data();
    this.get_using_dress_data();
  },
  methods: {
    /***初始化webgl相关 ***/
    initRender() {
      this.renderer = new THREE.WebGLRenderer({
        antialias: true, //平滑过渡
        alpha: true //alpha通道
      });
      this.renderer.setSize(300, 500);
      //告诉渲染器需要阴影效果
      this.renderer.setClearColor(0xb0c4de);
      // document.body.appendChild(this.renderer.domElement);
      document.querySelector("#bmox").appendChild(this.renderer.domElement);
    },
    //相机
    initCamera() {
      // this.camera = new THREE.PerspectiveCamera(120, 300 / 500, 0.1, 1000);
      this.camera = new THREE.OrthographicCamera(
        300 / -4,
        300 / 4,
        500 / 4,
        500 / -4,
        0.1,
        1000
      );
      this.camera.position.set(0, 0, 200);
      // this.camera.lookAt(new THREE.Vector3(0, 0, 0));
      this.camera.lookAt(this.scene.position);
    },
    //场景
    initScene() {
      this.scene = new THREE.Scene();
    },
    //gui监控
    initGui() {
      //声明一个保存需求修改的相关数据的对象
      this.gui = {};
      this.datGui = new dat.GUI();
      //将设置属性添加到gui当中，gui.add(对象，属性，最小值，最大值）
    },
    //光源/光线
    initLight() {
      this.scene.add(new THREE.AmbientLight(0xffffff));

      //增加光源
      this.light = new THREE.PointLight(0xffffff);
      this.light.position.set(0, 50, 50);
      // this.datGui.add(this.controls, 'autoRotateSpeed', 0, 100);
      //告诉平行光需要开启阴影投射
      this.light.castShadow = true;

      this.scene.add(this.light);
    },
    //模型-几何体
    initModel() {
      var that = this;
      // 辅助线
      // var helper = new THREE.AxesHelper(50);
      // this.scene.add(helper);

      THREE.Loader.Handlers.add(/\.tga$/i, new THREE.TGALoader()); //tga支持

      //小人模型
      var module = {
        init() {
          this.hair();
          this.face();
          this.tops();
          this.pants();
          // this.shoes();
        },
        //头发
        hair() {
          this.module_import("hair");
        },
        //脸
        face() {
          this.module_import("face");
        },
        //服饰
        tops() {
          this.module_import("tops");
          // console.log(that.module_data.tops);
          // this.module_import(that.resourceId_fileName(that.module_data.tops));
        },
        //裤子
        pants() {
          this.module_import("pants");
        },
        //鞋子
        shoes() {
          this.module_import("shoes");

          // this.module_import(that.resourceId_fileName(that.module_data.shoes));
        },
        //背包
        backpack() {
          this.module_import("backpack");
        },
        //头饰-耳机
        hat() {
          this.module_import("hat");
        },
        //围巾-首饰
        scarf() {
          this.module_import("scarf");
        },
        //眼镜
        glasses() {
          this.module_import("glasses");
        },
        //模块导入
        module_import(module_name, mtl_name) {
          var mtlLoader = new THREE.MTLLoader();
          mtlLoader.setPath(that.file_base_src);
          mtlLoader.load(module_name + ".mtl", function(materials) {
            materials.preload();

            var objLoader = new THREE.OBJLoader();
            objLoader.setMaterials(materials);
            objLoader.setPath(that.file_base_src);
            objLoader.load(module_name + ".obj", function(object) {
              object.position.y = -100;
              that.scene.add(object);
            });
          });
        }
      };
      module.init();
    },
    //stats-统计控件
    initStats() {
      this.stats = new Stats();
      document.body.appendChild(this.stats.dom);
    },
    //控制器啊
    initControls() {
      this.controls = new THREE.OrbitControls(
        this.camera,
        this.renderer.domElement
      );

      // 如果使用animate方法时，将此函数删除
      //controls.addEventListener( 'change', render );
      // 使动画循环使用时阻尼或自转 意思是否有惯性
      this.controls.enableDamping = true;
      //动态阻尼系数 就是鼠标拖拽旋转灵敏度
      this.controls.dampingFactor = 0.5;
      //是否可以缩放
      this.controls.enableZoom = true;
      //是否自动旋转
      this.controls.autoRotate = true;
      this.controls.autoRotateSpeed = 2.8;
      //设置相机距离原点的最远距离
      this.controls.minDistance = 0.1;
      //设置相机距离原点的最远距离
      this.controls.maxDistance = 1000;
      //是否开启右键拖拽
      this.controls.enablePan = false;
      //水平方向视角限制
      //是否可旋转，旋转速度
      this.controls.enableRotate = true;
      this.controls.rotateSpeed = 0.6;
      //最大仰视角和俯视角
      this.controls.minPolarAngle = 0; // radians
      this.controls.maxPolarAngle = Math.PI; // radians
    },
    //加载器
    render() {
      this.renderer.render(this.scene, this.camera);
    },
    //窗口大小更改(适合全屏)
    onWindowResize() {
      // this.camera.aspect = window.innerWidth / window.innerHeight;
      this.camera.aspect = 300 / 500;
      this.camera.updateProjectionMatrix();

      this.render();
      this.renderer.setSize(300, 500);
    },
    //动画
    animate() {
      // this.camera.position.z=30
      //更新控制器
      this.render();
      //更新性能插件
      this.stats.update();
      this.controls.update();

      requestAnimationFrame(this.animate);
    },
    //所有组件准备好,生成
    draw() {
      this.initGui();
      this.initRender();
      this.initScene();
      this.initCamera();
      this.initLight();
      this.initModel();
      this.initControls();
      this.initStats();

      this.animate();
      window.onresize = this.onWindowResize;
    },

    /***===========================事件数据========================= ***/
    //获取真正穿戴列表
    get_using_dress_data() {
      this.$http({
        method: "get",
        url:
          this.$api.API_BM_decoration + "/api/v1/decorations/using?otherId=0",
        data: {}
      })
        .then(result => {
          if (result.data.code == 1) {
            console.log("otherId", result.data.data);
            this.avatar_data = result.data.data;
            for (var i = 0; i < result.data.data.length; i++) {
              var sex =
                result.data.data[i].sex == 1
                  ? "boy_"
                  : result.data.data[i].sex == 2 ? "girl_" : "";
              var file_name = this.resourceId_fileName(result.data.data[i]);
            }
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
    //获取装扮列表数据
    get_dress_data(key) {
      this.avatar_index = key||0;
      if (key==0) {
        this.get_using_dress_data();
        return
      }
      // if (this.avatar_index == key) return;
      this.$http({
        method: "get",
        url:
          this.$api.API_BM_decoration +
          "api/v1/decorations/" +
          this.avatar_index,
        data: {}
      })
        .then(result => {
          if (result.data.code == 1) {
            this.avatar_data = result.data.data;
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
    //更新/增加用户穿着
    put_dress_data(item) {
      this.$http({
        method: item.status == 1 ? "delete" : "put",
        url:
          this.$api.API_BM_decoration + "api/v1/decorations/using/" + item.id,
        data: {}
      })
        .then(result => {
          if (result.data.code == 1) {
            this.get_dress_data(this.avatar_index) ; 
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
    //文件名处理(resourceId=>fileName)
    resourceId_fileName(item) {
      var fileName = item.resourceId.slice(
        item.resourceId.indexOf("_") + 1,
        item.resourceId.indexOf(".")
      );
      var file_number = item.resourceId.slice(
        item.resourceId.indexOf(".") + 1,
        item.resourceId.length
      );
      console.log(fileName + (file_number > 9 ? file_number : 0 + file_number));
      return fileName + (file_number > 9 ? file_number : 0 + file_number);
    }
  }
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style  lang="scss" scoped type="text/css">
.bm-avatar {
  text-align: left;
}
.bm-content {
  display: flex;
  .bm-show-box {
    width: 300px;
    height: 500px;
    border: 1px solid #000;
  }
  .bm-select-box {
    width: 700px;
    .bm-content-avatar {
      margin-top: 10px;
      background-color: #fff;
      width: 100%;
      .bm-card-item {
        float: left;
        width: 170px;
        cursor: pointer;
        margin: 1px;
      }
      .bm-avtive {
        position: relative;
        &:after {
          content: "";
          position: absolute;
          left: 0;
          top: 0;
          width: 100%;
          height: 100%;
          border: 4px solid #01925b;
          box-sizing: border-box;
        }
      }
    }
  }
}
canvas {
  width: 100%;
  height: 100%;
}
</style>
