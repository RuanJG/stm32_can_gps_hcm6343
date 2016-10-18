不少朋友都知道uCGUI3.98的VC工程源码并不完整，基于这个原因，本人对uCOS3.98的VC工程作了修改。主要修改如下：

1、增加uCGUI3.90的AntiAlias、JPEG、MemDev、MultiLayer到GUI目录；
2、GUI目录下，新建VNC目录，增加GUI_ProcessVNC.c文件，此文件为空，为与原工程保持一致性，另新建Touch目录；
3、Simulation目录下增加BASETSD.H头文件，新建GUISim目录，把SIM.h、SIM_X.c、GUI_SIM_NoWin32.h、GUI_SIM_Win32.h、GUISim.lib添加进该目录，因此本人需要更改工程的相关设置；
4、GUIConf.h中，打开GUI_SUPPORT_MEMDEV宏；
5、工程里，Simulation删除SIM_GUI组及相关文件，增加GUISim组，添加BASETSD.H头文件等；
6、增加Debug支持；
7、重新安排目录，重建VC工程；
8、VC工程同时融合了GUIDemo，便于GUI的学习，配置宏在uCGUI\Config\GUIConf.h文件中；
9、修复GUIDemo的Bug，第二个GUIDEMO_main()创建Stop按扭，按键无效，本人新添加了Start提示。

推荐:Visual C++ 6.0 完整绿色版.rar( http://ishare.iask.sina.com.cn/f/25330274.html?w= )，该版本精简小巧，总大小45.26MB。



                                                                                                                编辑： 华兄 

                                                                                                                Email: 591881218@qq.com (注：留言请使用邮箱，勿使用QQ即时聊天)