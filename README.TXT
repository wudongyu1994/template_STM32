实验器材:
	STM32F103 C8
	
硬件资源:
	x.引脚	引脚名		连接对象
	1.PA9	TX			电脑端RX
	2.PA10	RX			电脑端TX
	
实验现象:
	1. 电脑端观察到3句初始化信息，然后每1s，led灯变换一次。

	
注意事项:
	1.keilkill.bat,是一个批处理文件，双击自动运行，删除程序编译过程中生成的中间文件，方便打包传输。
	2.如果需要更改系统时钟，请直接在 system_stm32f10x.c 中修改宏定义即可。delay_ms 始终是准确的。
	3.移植需要注意以下几点：
		工程中的core文件夹下导入对应的.s文件；
		设置中device标签下选择正确的系列产品；
		设置中C/C++标签下define处填写正确宏定义；
		设置中Utilities标签下setting打开，选择正确的内存产品；