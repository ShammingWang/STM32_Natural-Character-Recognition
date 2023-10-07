实验器材:
	战舰STM32F103开发板V3版本
	
硬件资源:
	1,DS0(连接在PB5)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,ALIENTEK 2.8/3.5/4.3/7寸TFTLCD模块(通过FSMC驱动,FSMC_NE4接LCD片选/A10接RS)
	4,按键KEY0(PE4)/KEY_UP(PA0)
	5,W25Q128(SPI FLASH芯片,连接在SPI2上) 
	6,ESP8266模块

开发软件：
	Keil

程序功能：
	STM32驱动电容屏实现手写输入功能，通过ESP8266模块将图片发送至服务器处理，等待返回结果后，再将识别结果通过串口发送至主机。