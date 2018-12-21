#include "Misc.h"


extern int LOW_POWER_SET;
extern int HIGH_POWER_SET;

extern robot_t g_robot;
extern unsigned char last_beep;
unsigned char is_low_power_cnt;


int ADvalue;
int System_mode;
unsigned char reg_led = 0;
/* dribbler */

//initial the dribbler, variable freq is the frequence of DBPWM
unsigned int db_pwm_freq=0xffff;
unsigned char g_power = 0;


/*---------function headers----------*/
int is_power_low();



/*-----------------------------------------------------------------
 *  �������ƣ� is_ball_detected
 *  ���������� ͨ�������ж����Ƿ���С������������Ӧ���ţ�pin138���ĸߵ�
 *			               ��ƽ������ƽΪ�ͣ�˵�������������������1������ƽΪ�ߣ�
 *                               ˵��С������������������0.
 *  ���ߣ�  		  ����
 *  �޸����ڣ� 2012��11��29
 -----------------------------------------------------------------*/ 
int is_ball_detected()
{
  if( 0x0 == (IORD(PIO_INFRA_BASE,0) & 0x1))
    return 1;
  else
    return 0;
}


/*-----------------------------------------------------------------
 *  �������ƣ�is_infra_broken
 *  ���������� �������Ƿ񻵵���ԭ�����Ǽ������Ƿ�һֱ����ס
 *  
 *
 *
 *  ���ߣ�   �����
 *  �޸����ڣ� 2012-12-5
 -----------------------------------------------------------------*/ 
 int is_infra_broken()
{	
	int n=0;
	int i;
	for(i=1;i<=5;i++)
		{	
			if(!(IORD(PIO_INFRA_BASE,0) & 0x1))
			{
				n++;
			}
			else n=0;
		}
	if(n==5)
		return 1;//���˷���1
	else return 0;
}

/*-----------------------------------------------------------------
 *  �������ƣ� is_cap_low
 *  ���������� �����ݵ�ѹ�Ƿ���ͣ������ݵ�ѹ�����򷵻�1��
 *    				  �����ݵ�ѹ�����򷵻�0
 *
 *
 *  ���ߣ�    	 ����
 *  �޸����ڣ� 2012-12-4
 -----------------------------------------------------------------*/ 
int is_cap_low()
{
	   //ADvalue = Get_CapVoltage();
	ADvalue=(IORD(ADGETNEW2_0_BASE,1)&0x00ff);
	if(ADvalue <= CAP_POWER_LOW)
		return 1;
	else
	  	return 0;
}




/*-----------------------------------------------------------------
 *  �������ƣ� is_power_low
 *  ���������� ����ص�ѹ�Ƿ���ͣ������ڱ�����ѹ������1��������
 *			   ǿ��ֹͣ�ĵ�ѹ������2����ѹ��������0
 *
 *  ���ߣ�    	   �
 *  �޸����ڣ� 2012-11-29
 -----------------------------------------------------------------*/ 
int is_power_low()
{
  /* compare battery power AD value with low power threashold */
	static int powerarr[5]={0};
  	static int num=0;
	static int avepower;
	static int limpower=5;//,counter=0;   //the range of power is -2 ~ +2
	int i,u,d;
  	
   	g_power=(IORD(ADGETNEW2_0_BASE,0)&0x00ff);   //����ص�ѹ
   	powerarr[num++]=g_power;
	if(num==5) num=0;

	u=d=0;
	if(powerarr[4])
	{
		for(i=1;i<5;i++)
		{
			if(powerarr[i] > powerarr[u]) u = i;
			if(powerarr[i] < powerarr[d]) d = i;			
		}
        
		if(powerarr[u] > avepower + limpower) powerarr[u] = avepower;
		if(powerarr[d] < avepower - limpower) powerarr[d] = avepower;
        
        avepower = powerarr[0];
		
        for(i=1;i<5;i++)
		{
			avepower = avepower + powerarr[i];
		}

		avepower = avepower / 5;
        //counter++;
        //printf("%d  %d\n",counter,avepower);

		if(avepower < (int)WARNING_POWER_D)
		{
  			if(avepower <= (int)FORCESTOP_POWER_D)
  			{
				return 2;
  			}
			return 1;
    	}
	}
	else avepower = (powerarr[0]+powerarr[1]+powerarr[2]+powerarr[3])/4;
    return 0;
}


/*-----------------------------------------------------------------
 *  �������ƣ� do_power_monitor
 *  ���������� �Ե�ص�ѹ���м�⣬�����ʱ���ѹ���ͣ��ͱ����������ض�Ӧ�ı�־
 *
 *  ���ߣ� 	�
 *  �޸����ڣ� 2012-12-23
 -----------------------------------------------------------------*/ 
int do_power_monitor()
{
	char retflag;
	
	retflag=is_power_low();
    if( retflag )
	{ 
		if(is_low_power_cnt<5)
			is_low_power_cnt++;
		else
		{
			is_low_power_cnt=5;
		
#ifdef ENABLE_BEEP
			beep_on( BEEP_POWER );
#endif
			return retflag;
		}
	}
	else
	{
        is_low_power_cnt=0;		
		
#ifdef ENABLE_BEEP
		//if(last_beep == 0)	//���䱨�����رշ�����
		beep_off( BEEP_POWER );
#endif
	}

	return 0;
}



/*-----------------------------------------------------------------
 *  �������ƣ� init_led
 *  ���������� ��ʼ��LED�ƣ���LED�������¡���Ӧ����������
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
int init_led()
{  
	led_on(1);
	led_on(0); 
	delay(100000);
	led_off(1);
	led_off(0);
	delay(100000);
	led_on(1);
	led_on(0); 
	delay(100000);
	led_off(1);
	led_off(0); 
	delay(100000);
	led_on(1);
	led_on(0); 
	return 0;

}


/*-----------------------------------------------------------------
 *  �������ƣ� led_off
 *  ���������� �ر�led��
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
int led_off( int channel )
{
  switch ( channel )
  {
    //LED 0
    case 0 : reg_led = reg_led | 0x1; break;
    //LED 1
    case 1 : reg_led = reg_led | 0x2; break;
    
    default  : return -1; break;
  }

  IOWR(PIO_LED_BASE,0,reg_led);

	return 0; 
}




/*-----------------------------------------------------------------
 *  �������ƣ� led_on
 *  ���������� ����led��
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
int led_on( int channel )
{
  switch ( channel )
  {
    //LED 0
    case 0 : reg_led = reg_led & (~0x1); break;
    //LED 1
    case 1 : reg_led = reg_led & (~0x2); break;
    
    default  : return -1; break;
  }

  IOWR(PIO_LED_BASE,0,reg_led);  

  return 0;
}



/*-----------------------------------------------------------------
 *  �������ƣ� beep_on
 *  ���������� ʹ�ñ�����������
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
int beep_on( int channel )
{
	IOWR( PIO_BUZZER_BASE , 1 , 1);
  	IOWR( PIO_BUZZER_BASE , 0 , 1 );
  	return 0;
}


/*-----------------------------------------------------------------
 *  �������ƣ� beep_off
 *  ���������� ʹ�ñ�����ֹͣ����
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
int beep_off( int channel )
{
	IOWR( PIO_BUZZER_BASE , 1 , 1);
  	IOWR( PIO_BUZZER_BASE , 0 , 0);
  	return 0;
}



/*-----------------------------------------------------------------
 *  �������ƣ� record_err
 *  ���������� ��¼�������Ϣ
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
void record_err(int err_no)
{
	static int error_pos = 0;
	g_robot.error[error_pos].name = err_no;
	error_pos++;
	if(error_pos >= ERROR_NUM)
	{
		error_pos = 0;
	}
}



/*-----------------------------------------------------------------
 *  �������ƣ� heart_beat
 *  ���������� ������ִ�к�������������ͱ䰵������ư��ͱ���
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 
int heart_beat()
{
    static int is_led_on = 0;
  
    if( is_led_on )
    {
        led_off( 0 );    

        is_led_on = 0;
    }
    else
    {
  
        led_on( 0 );
        is_led_on = 1;
    }
  
	return 0;
}



/*-----------------------------------------------------------------
 *	�������ƣ� delay
 *	���������� д��һ����ʱ��������Ϊ��ͬ�ı�����������ʱ�ĳ��̲�ͬ��������OP_NONE������һ���Ż�����
 *		       ������Ż�����ָ�����������ļ��е����ԡ�build������á���Ҫ���ó�һ�µġ�
 *			   �����ڿ����Ե�system library����Ҫֻѡ��clean exit /reduced device drivers/small C library. ������ʱ���в��
 *			   ������ʾ��Ҫ��ʱ��ʱ�䣬��λ��us�������ʱΪ1us��
 *			   �ݲ���ȫ���ԣ������1us����10%���¡�
 *			   �ڲ��Ż�������£���ʱ�����ʱ������ʵ�֣���Լ��10us����
 *			   ��Ҫ��ȷ��ʱ����ö�ʱ����
 *	
 *
 *
 *	���ߣ�	   ����
 *	�޸����ڣ� 2011-9-15
 -----------------------------------------------------------------*/ 

void delay(int us)
{
	unsigned int i = 0;
	#ifdef OP_NONE 
		usleep(us);
	#else
		if(us == 0)
		{

		}
		else if(us == 1)
		{
			for(i=0;i<5;i++)
                		asm("nop");
		}
		else if(us < 5)
		{
			usleep(us-2);
		}
		else if(us < 11)
		{
			usleep(us-1);
		}
		else
		{
			usleep(us);
		}
	#endif 
}


