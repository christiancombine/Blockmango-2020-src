业务组开发流程说明：
1、 业务组提的基础功能在master上面开发完之后，业务组的童鞋负责将其合到自己的dev_1,dev_2上面

2、 业务组在改造基础逻辑功能的时候一定要切回到master上面开发，然后再合到自己的分支上面

3、 基础组童鞋在master上开发基础功能的时候，在业务组版本还没准备发版本之前，要自己合到业务组的dev_1，dev_2上面，让其跟着业务逻辑多点测试

4、 业务组在封版的时候，要合并到自己的test分支上并打tag, tag示范v2.6-chicken v2.3-hotfix-fix_vehicle_send_too_many_packet, 然后合并代码到mater上面。

5、 平时提交的时候，必须独立的commit对应对立的功能。如果一个commit包含了多个功能的修改，需在提交日志上面分点说明


业务组服务端部署说明：
1、 项目组平时开发用的主分支和测试时候用的测试分支
  	游戏一组： 开发主分支dev_1,测试分支test_1
   	游戏二组： 开发主分支dev_2,测试分支test_2
   	游戏三组： 开发主分支dev_3,测试分支test_3
   	游戏四组： 开发主分支dev_4,测试分支test_4

2、 测试服地址 120.92.133.131 user: ubuntu  passwd: 9P3Ux6dH
 	 测试服代码路径
 	 游戏一组：/data/dev_1/blockmango-client
  	 游戏二组: /data/dev_2/blockmango-client
  	 游戏三组：/data/dev_3/blockmango-client
  	 游戏四组: /data/dev_4/blockmango-client

3、 服务端编译方法：
  	游戏一组：进入/data/dev_1/blockmango-client/dev/deploy，执行./build_and_deploy.h test_1 pull， 即可切换到test_1分支上并更新test_1分支代码，并编译部署到测试服上。如果想用当前代码路径上的代码部署，参数pull改为current即可。
  	引擎号为90001

	游戏二组：进入/data/dev_2/blockmango-client/dev/deploy，执行./build_and_deploy.h test_2 pull， 即可切换到test_2分支上并更新test_2分支代码，并编译部署到测试服上。如果想用当前代码路径上的代码部署，参数pull改为current即可。
	引擎号为90002

	游戏二组：进入/data/dev_3/blockmango-client/dev/deploy，执行./build_and_deploy.h test_3 pull， 即可切换到test_2分支上并更新test_2分支代码，并编译部署到测试服上。如果想用当前代码路径上的代码部署，参数pull改为current即可。
	引擎号为90003

	游戏二组：进入/data/dev_4/blockmango-client/dev/deploy，执行./build_and_deploy.h test_4 pull， 即可切换到test_2分支上并更新test_2分支代码，并编译部署到测试服上。如果想用当前代码路径上的代码部署，参数pull改为current即可。
	引擎号为90004