#include <msp430.h> 
#include "I2C.h"
#include "Mpu.h"
#include "lcd.h"
#include <math.h>

//#include <string>
/*
 * main.c
 */
#define uint unsigned int
#define uchar unsigned char
//void CLK_Init();
void Montor_PWM();
void PWM_Init();
void delay(unsigned int n)    ;
void delay_us(unsigned int n);
void CLK_Init() ;
void Timer_Init();
void Test_Init();
//void delay_us(unsigned int n);
//void delay(unsigned int n) ;
float PID1(float e);
float PID2(float e);
float PID3(float e);
float PID4(float e);
void AHRS();
unsigned long Timebase=0;
unsigned long risingTime,fallingTime;
unsigned int risingCap,fallingCap;
unsigned long sonicDist,sonicTime;
int ax,ay,az,gx,gy,gz;
int offset_ax=0,offset_ay=0,offset_az=0,offset_gx=0,offset_gy=0,offset_gz=0;
float ax_accu,ay_accu,az_accu,gx_accu,gy_accu,gz_accu;
float k = 0.998;
float t = 0.01;
float kp = 0.8,ki = 0.05,kd = 0;
float G_pwm1=10,G_pwm2=10,G_pwm3=10,G_pwm4=10;
float angle_x=0.0,angle_y=0.0,angle_z=0.0;
float Set_x=0.0,Set_y=0.0,Set_z=0.0,Set_g=1.0;
char temp[6]={48,48,48,48,48,45};
int status = 0;
void Data_Init();
void Turn2char(float x);
void Get_Data();
void Ultra_Init();
void Ultra_Tx();
int main(void) {


    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    P11DIR = BIT2 + BIT1 + BIT0;              // P11.2,1,0 to output direction
    P11SEL = BIT2 + BIT1 + BIT0;              // P11.2 to output SMCLK, P11.1to output MCLK and P11.0 tooutput ACLK

    CLK_Init(); //时钟初始化,SMCLK=MCLK=24MHZ,ACLK=32768HZ

    TA0CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, up mode, clear TAR


    MPU_Init(); //传感器
    //delay(5000); //启动延时
    //Data_Init(); //零点漂移矫正
    PWM_Init();  //PWM功能开启
    Timer_Init(); //定时器功能开启
    //Ultra_Init()	;
     //Ultra_Tx()	;
    //Test_Init();  //测试起飞启动

    Init_Port()                                             ;
    Init_SPI()                                              ;
    Init_LCD();
      //Init_AD();
    LCD_Clear();



    while(1)
    {

    	LCD_Printf(0,1,"ag_x is ");
    	Turn2char(angle_x);
    	LCD_Char(temp[5]);
    	//LCD_Char(temp[0]);
    	//LCD_Char(temp[1]);
    	LCD_Char(temp[2]);
    	LCD_Char(temp[3]);

    	LCD_Char(temp[4]);


    	LCD_Printf(0,2,"ag_y is ");
    	    	//LCD_Set(0,4);
    	    	Turn2char(angle_y);
    	    	LCD_Char(temp[5]);
    	    	//LCD_Char(temp[0]);
    	    	//LCD_Char(temp[1]);
    	    	LCD_Char(temp[2]);
    	    	LCD_Char(temp[3]);

    	    	LCD_Char(temp[4]);

    	    	LCD_Printf(0,3,"dist is ");

    	    	    	Turn2char(sonicDist);
    	    	    	//LCD_Char(temp[5]);
    	    	    	//LCD_Char(temp[0]);
    	    	    	//LCD_Char(temp[1]);
    	    	    	LCD_Char(temp[2]);
    	    	    	LCD_Char(temp[3]);

    	    	    	LCD_Char(temp[4]);

    	    	LCD_Printf(0,4,"g_x is ");

    	    	    Turn2char(gx_accu);
    	    	    LCD_Char(temp[5]);
    	    	    	    	    	//LCD_Char(temp[0]);
    	    	    	    	    	//LCD_Char(temp[1]);
    	    	    LCD_Char(temp[2]);
    	    	    LCD_Char(temp[3]);

    	    	    LCD_Char(temp[4]);
    	    	    LCD_Printf(0,5,"g_y is ");

    	    	    	Turn2char(gy_accu);
    	    	    	  LCD_Char(temp[5]);
    	    	    	   //LCD_Char(temp[0]);
    	    	    	    //LCD_Char(temp[1]);
    	    	    	   LCD_Char(temp[2]);
    	    	    	   LCD_Char(temp[3]);
    	    	    	   LCD_Char(temp[4]);
    	    	    	    	delay(100);

    	/*
    	LCD_Printf(0,1,"angle_y is ");
    	TTurn2char(angle_y);
    	LCD_Char(temp[0]);
    	LCD_Char(temp[1]);
    	LCD_Char(temp[2]);
    	LCD_Char(temp[3]);
    	LCD_Char(temp[4]);
    	delsy(1000);
    	LCD_Printf(0,1,"angle_z is ");
    	Turn2char(angle_z);
    	    	LCD_Char(temp[0]);
    	    	LCD_Char(temp[1]);
    	    	LCD_Char(temp[2]);
    	    	LCD_Char(temp[3]);
    	    	LCD_Char(temp[4]);
    	    	delsy(1000);
    	    	*/

    }

}

void Turn2char(float x)
{
	int a;
	int p;
	a = (int)x;  //x =23145
	if(a<0)
	{
		a = -a;
		temp[5] = 45;
	}
	else
		{temp[5] = 43;}

	p = (a/10000)+48;

	temp[0]= p; // b= 2

	p =(a%10000/1000)+48;

	temp[1] = p; //c=3;

	p = (a%10000%1000/100)+48;

	temp[2] =p;//e = 1

	p =(a%10000%1000%100/10)+48;

		temp[3] =p;//e = 1

	p = (a%10000%1000%100%10)+48;

	temp[4] = p; //g = 5




}

void Test_Init()
{
	int num;

	for(num=0;num<30;num++)
	{
		 G_pwm4 =  G_pwm4 +6;
		 G_pwm3 =  G_pwm3 +6;
		 G_pwm2 =  G_pwm2 +6;
		 G_pwm1 =  G_pwm1 +6;

		delay(50);
		Montor_PWM();
	}
	for(num=0;num<30;num++)
		{
			 G_pwm4 =  G_pwm4 -10;
			 G_pwm3 =  G_pwm3 -10;
			 G_pwm2 =  G_pwm2 -10;
			 G_pwm1 =  G_pwm1 -10;

			delay(50);
			Montor_PWM();
		}
	for(num=0;num<100;num++)
		{
			 G_pwm4 =  G_pwm4 +4;
			 G_pwm3 =  G_pwm3 +4;
			 G_pwm2 =  G_pwm2 +4;
			 G_pwm1 =  G_pwm1 +4;

			delay(50);
			Montor_PWM();
		}

}
void Get_Data()
{
	ax = GetAccelX()-offset_ax;
	ay = GetAccelY()-offset_ay;
	az = GetAccelZ()-offset_az;
	gx = GetAnguX()- offset_gx;
	gy = GetAnguY()-offset_gy;
	gz = GetAnguZ()-offset_gz;

	ax_accu=(float)(ax/16384) ;
	ay_accu=(float)(ay/16384) ;
	az_accu=(float)(az/16384) ;

	gx_accu=(float)(gx/131) + 2 ;
	gy_accu=(float)(gy/131) - 2 ;
	gz_accu=(float)(gz/131);

	if(gx_accu < 1.3 && gx_accu>-1.3) gx_accu =0;
	if(gy_accu < 1.3 && gy_accu>-1.3) gy_accu =0;
	if(gz_accu < 1.3 && gz_accu>-1.3) gz_accu =0;

		angle_x = k*(angle_x + gx_accu*t )+(1-k)*(ax_accu);
		angle_y = k*(angle_y + gy_accu*t )+(1-k)*(ay_accu);
		angle_z = k*(angle_z + gz_accu*t )+(1-k)*(az_accu);


}
void Data_Init()
{
	int count;
    offset_ax=0;
    offset_ay=0;
    offset_az=0;
    offset_gz=0;
    offset_gx =0;
    offset_gy=0;
	for(count=0;count<300;count++)
	    {

	    	offset_ax += (GetAccelX()/16384);
	    	offset_ay += (GetAccelY()/16384);
	    	offset_az += (GetAccelZ()/16384);

	    	offset_gy += (GetAnguY()/131);
	    	offset_gx += (GetAnguX()/131);
	    	offset_gz += (GetAnguZ()/131);

	    }
	    offset_ax = offset_ax/300;
	    offset_ay =offset_ay/300;
	    offset_az = offset_az/300;
	    offset_gx = offset_gx/300 ;
	    offset_gy = offset_gy/300 ;
	    offset_gz = offset_gz/300;
	    status =1;
}


void Timer_Init()
{
	TA1CCTL0 = CCIE;                           // CCR1 interrupt enabled
	TA1CCR0 = 30000-1;
	TA1CTL = TASSEL_2 + MC_1 + TACLR;          // SMCLK, contmode, clear TBR
}


void PWM_Init()
{
	P1DIR |= BIT2+BIT3+BIT4+BIT5;                            // P2.2 and P2.3 output
	P1SEL |= BIT2+BIT3+BIT4+BIT5;                            // P2.2 and P2.3 options select
	TA0CCR0 = 60000-1;                          // PWM Period
    TA0CCTL1 = OUTMOD_7;                      // CCR1 reset/set

	TA0CCTL2 = OUTMOD_7;                      // CCR2 reset/set

	TA0CCTL3 = OUTMOD_7;                      // CCR2 reset/set

	TA0CCTL4 = OUTMOD_7;                      // CCR2 reset/set

	TA0CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, up mode, clear TAR
}
void Montor_PWM()
{
	int a,b,c,d;
	if(G_pwm1<0) G_pwm1=0;
	if(G_pwm2<0) G_pwm2=0;
	if(G_pwm3<0) G_pwm3=0;
	if(G_pwm4<0) G_pwm4=0;
	if(G_pwm1>1000)  G_pwm1=1000;
	if(G_pwm2>1000)  G_pwm2=1000;
	if(G_pwm3>1000) G_pwm3=1000;
	if(G_pwm4>1000)  G_pwm4=1000;

	int state = 3000; //1ms
	 a =  (int)(G_pwm1 * 3) + state;
	 b =  (int)(G_pwm2 * 3) + state;
	 c =  (int)(G_pwm3 * 3 )+ state;
	 d =  (int)(G_pwm4 * 3) + state;
	TA0CCR1 =  a;                            // CCR1 PWM duty cycle
	TA0CCR2 =  b;                            // CCR2 PWM duty cycle
	TA0CCR3 =  c;                            // CCR2 PWM duty cycle
	TA0CCR4 =  d;                            // CCR2 PWM duty cycle
}
void AHRS()
{	
	float p_x,p_y,p_z,p_g;
	float pwm1=0,pwm2=0,pwm3=0,pwm4=0;
	p_x = angle_x -Set_x;
	p_y = angle_y - Set_y;
	p_z = angle_z - Set_z;
	p_g = Set_g-az_accu;
	if(p_x<0.01 &&p_x>-0.01) p_x= 0;
	if(p_y<0.01 &&p_y>-0.01) p_y= 0;
	if(p_g<0.01 &&p_z>-0.01) p_g= 0;
	if (p_x!=0){
		pwm1 = PID1(p_x);
		pwm2 = PID2(p_x);
		p_x = 0 - p_x;
		pwm3 = PID3(p_x);
		pwm4 = PID4(p_x);
	}
	if (p_y!=0){
		pwm4 = pwm4 + PID4(p_y);
		pwm2 = pwm2+ PID2(p_y);
		p_y = 0 - p_y;
		pwm3 = pwm3+PID3(p_y);
		pwm1 =pwm1+ PID1(p_y);

	}
	/*
	if (p_z!=0){
		pwm1 = PID1(p_x);
		pwm2 = PID2(- p_x);
		pwm3 = PID3(- p_x);
		pwm4 = PID4(p_x);
	}
	*/
/*
	if(p_g!=0 )
	{	

		pwm1 = PID1(p_g);
		pwm2 = PID2(p_g);
		pwm3 = PID3(p_g);
		pwm4 = PID4(p_g);

	}
*/
	G_pwm1 += pwm1;
	G_pwm2 += pwm2;
	G_pwm3 += pwm3;
	G_pwm4 += pwm4;
	Montor_PWM();

}
float PID1(float e)
{
	static float e_s=0.0,sum=0.0;
	float r;
	//sum+=e;//计算积分累加和
	r=kp*e+ki*sum;//+kd*(e-e_s);//从左至右分别是比例项、积分项、微分项
	//e_s=e;//保存这一次的误差值用于下一次微分计算
	return r;
}
float PID2(float e)
{
	static float e_s=0.0,sum=0.0;
	float r;
	sum+=e;//计算积分累加和
	r=kp*e+ki*sum;//+kd*(e-e_s);//从左至右分别是比例项、积分项、微分项
	//e_s=e;//保存这一次的误差值用于下一次微分计算
	return r;
}
float PID3(float e)
{
	static float e_s=0.0,sum=0.0;
	float r;
	sum+=e;//计算积分累加和
	r=kp*e+ki*sum;//+kd*(e-e_s);//从左至右分别是比例项、积分项、微分项
	//e_s=e;//保存这一次的误差值用于下一次微分计算
	return r;
}
float PID4(float e)
{
	static float e_s=0.0,sum=0.0;
	float r;
	sum+=e;//计算积分累加和
	r=kp*e+ki*sum;//+kd*(e-e_s);//从左至右分别是比例项、积分项、微分项
	//e_s=e;//保存这一次的误差值用于下一次微分计算
	return r;
}

void Ultra_Init()
{
	P8DIR|=BIT7;                   /*P8.7连接Trig*/
	P8OUT&=~BIT7;

	P8DIR&=~BIT6;		           /*P8.6连接Echo为输入*/
	P8SEL|=BIT6;

	TA1CCTL1|=CCIE;                /*CCR1中断使能*/
	TA1CCTL1=CM_3+CCIS_1+CAP+SCS;  /*双边出发，CCIxB为输入参照，捕获模式，同步捕获*/
	//TA1CTL1 = TASSEL_2 + MC_1 + TACLR;          // SMCLK, contmode, clear TBR
	//TimerA1 is defined in main.c
}
void Ultra_Tx()		   /*P8.7发送脉冲*/
{
	P8OUT|=BIT7;
	delay_us(10);
	P8OUT&=~BIT7;
}
void delay(unsigned int n)                 //1ms延时函数
  {
   unsigned int i;
   for (i=0;i<n;i++)
	   delay_us(1000);
  }
void delay_us(unsigned int n)
{

	unsigned int i;
	  for (i=0;i<n;i++)
		  _delay_cycles(24);
}
void CLK_Init()  //XT1作为基准时钟源，XT2作为主时钟源
{
	   P5SEL    |= 0x0c  ; // 端口选择外部低频晶振XT2 P5.2 P5.3
	   P7SEL	|= 0x03 ; //P7.0,P7.1
	   UCSCTL6   &= (~XT2OFF + ~XT1OFF) ; // 使能外部晶振 XT1,XT2
	   UCSCTL6   |= XCAP_3                                     ; // 设置内部负载电容

	   UCSCTL3   |= SELREF_0                ; // DCOref = XT1
	   UCSCTL4 |= SELA_0;        // Set ACLK = XT1=32768HZ

	   __bis_SR_register(SCG0)                                 ; // 关闭FLL控制回路

	   UCSCTL0    = 0x0000                                     ; // 设置DCOx, MODx
	   UCSCTL1    = DCORSEL_2                                  ; // 设置DCO振荡范围 3MHZ
	   //UCSCTL2    = FLLD_3                       ; // Fdco = ( 1/8)×FLLRef = (1/8) * 24MHZ = 3MHz
	   UCSCTL2    = FLLD__1 + 91                       ; // Fdco = ( FLL_FACTOR + 1)×FLLRef = (91 + 1) * 32768 = 3.014656MHz
	   __bic_SR_register(SCG0)                                 ; // 打开FLL控制回路
	   __delay_cycles(750000);

	   // 清除 XT2,XT1,DCO 错误标志
	   do
	   {
	     UCSCTL7 &= ~(XT2OFFG + + XT1LFOFFG + XT1HFOFFG+ DCOFFG); //
	                                             // 清除 XT2,XT1,DCO 错误标志
	     SFRIFG1 &= ~OFIFG;                      // 清除错误标志
	   }while (SFRIFG1&OFIFG);                   // 等待中断

	   UCSCTL6   |= XT2DRIVE0                       ; // XT2 驱动模式 8~12MHz
	   UCSCTL4   |= SELS_4 + SELM_5                            ; // SMCLK = DCOCLKDIV = 3MHZ  MCLK = XT2 =24MHZ

	   __bis_SR_register(GIE);       // Enter LPM3, enable interrupts


}



#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR (void)
{
	//AHRS(az,gx,gy,gz);

		//Get_Data();

	Get_Data();
	AHRS();
	//Timebase++;
	//Ultra_Tx();
}












