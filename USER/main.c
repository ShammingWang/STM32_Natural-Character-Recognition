#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"      
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h" 
#include "atk_ncr.h"
#include "touch.h"	 
#include "my8266.h"
#include "timer.h"
#include "string.h"

//����¼�Ĺ켣����

atk_ncr_point READ_BUF[200];
u16 pcnt=0;

//��ˮƽ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//��ʵ��Բ
//x0,y0:����
//r:�뾶
//color:��ɫ
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//������֮��ľ���ֵ 
//x1,x2����ȡ��ֵ��������
//����ֵ��|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//��һ������
//(x1,y1),(x2,y2):��������ʼ����
//size�������Ĵ�ϸ�̶�
//color����������ɫ
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		gui_fill_circle(uRow,uCol,size,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
 int main(void)
 {	 
	u8 last = 0;
	//u8 err=0;
 	int i=0;
	//int j=0;
	u8 tcnt=0;    
	u8 res[10];
	u8 key;		    
	u8 mode=4;					//Ĭ���ǻ��ģʽ
 	u16 lastpos[2];				//���һ�ε�����
	char* p;
	 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	LCD_Init();			   		//��ʼ��LCD     
	W25QXX_Init();				//��ʼ��W25Q128
	tp_dev.init();				//��ʼ��������
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	//alientek_ncr_init();		//��ʼ����дʶ��
	POINT_COLOR=RED;			//�����ʳ�ʼ��Ϊ��ɫ
	
	while(font_init()) 			//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,(u8*)"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
	}

	
//	esp8266_tcp_post(); //����ʶ������
	
 	
RESTART:
	ESP8266_RESTART:
	while(esp8266_init() || esp8266_tcp_init()) {
		printf("�����Զ�����ESP8266...\r\n");
		Show_Str(30,150,200,16,(u8*)"�����Զ�����ESP8266...",16,0);
		for(i=0;i<20000000;i++); //delay
		LCD_Clear(WHITE);
		goto ESP8266_RESTART;
	}
	
	LCD_Clear(WHITE);
	Show_Str(30,10,200,16,(u8*)"ս��STM32F103ZET6",16,0);				    	 
	Show_Str(30,30,200,16,(u8*)"����STM32����д����ʶ��ϵͳ",16,0);	//
	Show_Str(30,50,200,16,(u8*)"�ƿ�201/2",16,0);				    	 
	Show_Str(30,70,200,16,(u8*)"������ ҦԶ ������",16,0);			    
	Show_Str(30,90,200,16,(u8*)"ʶ����:",16,0);
	
	LCD_DrawRectangle(19,114,lcddev.width-20,lcddev.height-5);
	//LCD_DrawRectangle(49,400-5,lcddev.width-50,lcddev.height-5);
	POINT_COLOR=BLUE;
	//Show_Str(96,207,200,16,(u8*)"��д��",16,0);
	//tcnt=100;
	tcnt=100;
	while(1)
	{
		key=KEY_Scan(0); //һֱɨ�谴��
		if(key==WKUP_PRES&&(tp_dev.touchtype&0X80)==0)
		{
			TP_Adjust();  	//��ĻУ׼
			LCD_Clear(WHITE); 
			goto RESTART;	//���¼��ؽ���
		}
		//Show_Str(88,207,200,16,(u8*)"ȫ��ʶ��",16,0);
 		tp_dev.scan(0);//ɨ��
 		if(tp_dev.sta&TP_PRES_DOWN)//�������
		{
			delay_ms(1);//��Ҫ����ʱ,��������Ϊ�а�������.
 			tcnt=0; //����ʱ ���������
			if((tp_dev.x[0]<(lcddev.width-20-2)&&tp_dev.x[0]>=(20+2))&&(tp_dev.y[0]<(lcddev.height-5-2)&&tp_dev.y[0]>=(115+2)))
			{ //���������ָ��������
				if(lastpos[0]==0XFFFF) //����ǵ�һ����
				{
					lastpos[0]=tp_dev.x[0];
					lastpos[1]=tp_dev.y[0];
				}
				lcd_draw_bline(lastpos[0],lastpos[1],tp_dev.x[0],tp_dev.y[0],2,BLUE);//����
				lastpos[0]=tp_dev.x[0];
				lastpos[1]=tp_dev.y[0];
				if(pcnt<200)//�ܵ�������200
				{
					if(pcnt)
					{
						if((READ_BUF[pcnt-1].y!=tp_dev.y[0])&&(READ_BUF[pcnt-1].x!=tp_dev.x[0]))//x,y�����
						{
							READ_BUF[pcnt].x=tp_dev.x[0];
							READ_BUF[pcnt].y=tp_dev.y[0];
							pcnt++;
						}	
					}else //pcnt==0
					{
						READ_BUF[pcnt].x=tp_dev.x[0];
						READ_BUF[pcnt].y=tp_dev.y[0]; 
						pcnt++;
					}
				}   						  				  
			}
			last = 1;
		}else //�����ɿ��� 
		{
			if(last == 1) {
				if(pcnt < 200) {
					READ_BUF[pcnt].x=0;
					READ_BUF[pcnt].y=0; 
					pcnt++;
				} else {
					READ_BUF[199].x=0;
					READ_BUF[199].y=0; 
				}
			}
			lastpos[0]=0XFFFF; //��ʼ����һ�ε���һ����
			tcnt++;
			delay_ms(10); //�ɿ�һ����ʱ10ms
			//��ʱʶ��
			i++;
			//printf("tcnt: %d\r\n", tcnt);
			if(tcnt==40) //�ɿ���400ms ��Ϊ�˴��������
			{
				if(pcnt)//�� ��Ч������
				{
					for(i=0;i<10;i++) res[i]=0;
					p = esp8266_tcp_post(); //����ʶ������
					strcpy((char*)res, p); //д����������
					//alientek_ncr(READ_BUF,pcnt,1,mode,(char*)res);
					//printf("%c\r\n",*res);
					pcnt=0;
	  				POINT_COLOR=BLUE;//���û�����ɫ
		 			LCD_ShowString(30+72,90,200,16,16,res);
				}
				LCD_Fill(20,115,lcddev.width-20-1,lcddev.height-5-1,WHITE);
			}
			last = 0;
		}
		if(i==30)
		{
			i=0;
			LED0=!LED0;
		}  
	}       										    			    
} 

	

