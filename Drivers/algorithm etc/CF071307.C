
#include <REG52.H>
#include <MATH.H>
#include <STRING.H>

#define VER          7

#define FALSE        0
#define TRUE         1

#define OVERHIGH     1000000
#define OVERLOW      -200000

#define MTYPE        5           // 系列定时/计数器

#define PVWEI        6           // PV屏幕显示位数
#define PVWEI0       5
#define SVWEI        6           // SV屏幕显示位数
#define SVWEI0       5

#define LEDST        14          // LED闪烁开始位
#define LEDED        15          // LED闪烁停止位


#define  S10KEY       0x07
#define  LNOKEY       0x08
#define  S05KEY       0x09
#define  SSETDNKEY    0X0A
#define  SUP05KEY     0x0B
#define  SETKEYTI     0x0A        //按“SET”时间
#define  NOKEYTI      0x60        //无按键时间

#define  ADRNULL      0X6E

#define  ADRCLK       0X00
#define  ADRDE        0X01
#define  ADRBT        0X02
#define  ADRCP        0X03
#define  ADRCT        0X08
#define  ADRDOT       0X09

#define  ADRALT1      0X0A
#define  ADRALT2      0X0B
#define  ADRALT3      0X0C
#define  ADRALT4      0X0D
#define  ADRALT5      0X0E
#define  ADRALT6      0X0F

#define  ADRCONT      0X10
#define  ADRAL        0X14

#define  ADROUTH      0X18
#define  ADROUTL      0X1C

#define  ADRINC       0X20
#define  ADROUT       0X21
#define  ADRRST       0X22
#define  ADRAUTO      0X23
#define  ADRTUNI      0X24
#define  ADRSAVE      0X25
#define  ADRLOCK      0X26
#define  ADRALT       0X27
#define  ADRINT       0X28
#define  ADRRUN       0X29

#define  ADROUKK      0X2C
#define  ADROUPB      0X2E

#define  ADRKK        0X30
#define  ADRPB        0X34


#define  ADRKK        0X30
#define  ADRPB        0X34
#define  ADROUT1      0X38
#define  ADROUT2      0X3C

#define  ADRFLAG      0XFF

#define  ADREEPRE     0X00
#define  ADRTYPE      0X01

#define  ADRDT1CH     0x02
#define  ADRDT1CH1    0x02
#define  ADRDT1CH2    0x03
#define  ADRDT1CH3    0x04
#define  ADRDT1CH4    0x05

#define  ADRDT2CH     0x06
#define  ADRDT2CH1    0x06
#define  ADRDT2CH2    0x07
#define  ADRDT2CH3    0x08
#define  ADRDT2CH4    0x09

#define  ADRDTAL1     0X0A
#define  ADRDTAL2     0X0B

union word_byte{

      float fval;
      long  lval;
      int   word;
      uint  uword;
      uchar ubyte;

      struct{
             uchar char1;
             uchar char2;
             uchar char3;
             uchar char4;
      }bytes;
};




struct caidang{
    unsigned char biaoti[4];      //显示参数
    unsigned char address;        //数据地址
    unsigned char leng;           //数据长度
    long          hign;           //数据下限
    long          low;            //数据上限
    unsigned char dot;            //数据小数点
    unsigned char wenable;        //外部不开放使能参数
    unsigned char nenable1;       //内部不开放使能参数1
    unsigned char nenable2;       //内部不开放使能参数2
};

struct caidang code caidan[100]={
//     显示代码,地址,长度,上限,下限,       小数点  外使能 内使能

//一级参数
       " CLK",  ADRCLK, 1,   250,      0,   0x00,0x00, 0x00,0x00,   //0:密码
       "CONT", ADRCONT, 4,999999,-199999, ADRDOT,0X00,ADRCT,0x03,   //
       "  AL",   ADRAL, 4,999999,-199999,   0x0F,0X00,ADRCT,0x03,   //
       "NULL", ADRNULL, 2,  9999,  -1999,   0x00,0x00, 0x00,0xFF,   //
       "NULL", ADRNULL, 2,  9999,  -1999,   0x00,0x00, 0x00,0xFF,   //

       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,    //5
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,    //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,    //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,    //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,    //

       "1AT1", ADRALT1, 1,  99,    0,       0x00,0X00,ADRCT,0x04,   //10
       "1AT2", ADRALT2, 1,  59,    0,       0x00,0X00,ADRCT,0x04,   //
       "1AT3", ADRALT3, 1,  59,    0,       0X00,0X00,ADRCT,0x04,   //
       "2AT1", ADRALT4, 1,  99,    0,       0X00,0X00,ADRCT,0x04,   //
       "2AT2", ADRALT5, 1,  59,    0,       0X00,0X00,ADRCT,0x04,   //

       "2AT3", ADRALT6, 1,  59,    0,       0X00,0x00,ADRCT,0x04,   //15
       "NULL", ADRNULL, 1,9999,-1999,       0x00,0x00, 0x00,0xFF,   //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00, 0x00,0xFF,   //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00, 0x00,0xFF,   //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00, 0x00,0xFF,   //

//二级参数
       "  DE",   ADRDE, 1, 200,    1,       0x00,0x00, 0x00,0x00,   //20:
       "  BT",   ADRBT, 1,   5,    0,       0x00,0x00, 0x00,0x00,   //
       "  CP",   ADRCP, 1,   1,    0,       0x00,0x00, 0x00,0x00,   //
       " C-T",   ADRCT, 1,   2,    0,       0x00,0x00, 0x00,0x00,   //
       " DOT",  ADRDOT, 1,   3,    0,       0x00,0x00,ADRCT,0x03,   //

       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,   //25:
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,   //

       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00, 0x00,0xFF,   //30:
       " INC",  ADRINC, 1,   5,    0,       0x00,0x00,ADRCT,0x01,   //
       " INT",  ADRINT, 1,   1,    0,       0x00,0x00,ADRCT,0x06,   //
       " OUT",  ADROUT, 1,   2,    0,       0x00,0x00, 0x00,0xFF,   //
       "OUT1", ADROUT1, 4,360000,  0,       0x02,0x00, 0x00,0x00,   //

       "OUT2", ADROUT2, 4,360000,  0,       0x02,0x00,ADRCT,0x04,   //35:
       "NULL", ADRNULL, 4,360000,  0,       0x02,0x00, 0x00,0xFF,   //
       "AL-T",  ADRALT, 1,   2,    0,       0x00,0x00,ADRCT,0x07,   //
       "DELA",  ADRRST, 1,  20,    0,       0x00,0x00, 0x00,0xFF,   //
       "AUTO", ADRAUTO, 1,   1,    0,       0x00,0x00, 0x00,0x00,   //

       "TUNI", ADRTUNI, 1,   4,    0,       0x00,0x00,ADRCT,0x02,   //40:
       "SAVE", ADRSAVE, 1,   1,    0,       0x00,0x00,0x00, 0x00,   //
       "LOCK", ADRLOCK, 1,   3,    0,       0x00,0x00,0x00, 0x00,   //
       " AUN",  ADRRUN, 1,   1,    0,       0x00,0x00,0x00, 0x00,   //
       "  KK",   ADRKK, 4,999999,-199999,   0X0F,0x00,ADRCT,0x03,   //

       "  PB",   ADRPB, 4,999999,-199999,   0x0F,0x00,ADRCT,0x03,   //45:
       "OUTL", ADROUTL, 4,999999,-199999,   0x0f,0x00,ADRCT,0x03,   //
       "OUTH", ADROUTH, 4,999999,-199999,   0x0f,0x00,ADRCT,0x03,   //
       "OUKK", ADROUKK, 2,1999,    0,       0x03,0x00,ADRCT,0x03,   //
       "OUPB", ADROUPB, 2,1000,    0,       0x01,0x00,ADRCT,0x03,   //

       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //50
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //

       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //55
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //

       "  T1", ADRNULL, 1,  99,    0,       0x00,0x00,ADRCT,0xFF,   //60
       "  T2", ADRNULL, 1,  12,    1,       0x00,0x00,ADRCT,0xFF,   //
       "  T3", ADRNULL, 1,  31,    1,       0X00,0x00,ADRCT,0xFF,   //
       "  T4", ADRNULL, 1,  23,    0,       0X00,0x00,ADRCT,0xFF,   //
       "  T5", ADRNULL, 1,  59,    0,       0X00,0x00,ADRCT,0xFF,   //

       "  T6", ADRNULL, 1,  59,    0,       0X00,0x00,ADRCT,0xFF,   //65
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //

       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //70
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //

       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //75
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //

       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //80
       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,   //

       "NULL", ADRNULL, 2,1000,    0,       0x01,0x00,0x00,0xFF,   //85
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,   //
       "NULL", ADRNULL, 2,9999,-1999,       0x00,0x00,0x00,0xFF,   //

};

#define LEVEL1ST  0
#define LEVEL1END 19
#define LEVEL2ST  20
#define LEVEL2END 84
#define LEVEL3ST  85
#define LEVEL3END 89
#define LEVEL4ST  90

//显示段码
uchar code leddm[64]={
/*0    1    2    3    4    5    6    7    8    9    -    -1                       */
  0x3F,0x06,0x5B,0x4f,0x66,0x6d,0x7d,0x07,0x7F,0x6F,0x40,0x46,0x00,0x00,0x00,0x00,
/*     A    b    C    d    E    F    G    H    I    J    K    L    M    N    O    */
  0x00,0x77,0x7C,0x39,0x5E,0x79,0x71,0x00,0x76,0x06,0x0E,0x7A,0x38,0x37,0x37,0x3F,
/*P    Q    R    S    T    U    V    W    X    Y    Z                        _    */
  0x73,0x00,0x50,0x6D,0x31,0x3E,0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,
/*0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   -.  -1.             A.   P.   */
  0xBF,0x86,0xDB,0xCf,0xE6,0xEd,0xFd,0x87,0xFF,0xEF,0xC0,0xC6,0x00,0x00,0xF7,0xF3
};
//显示位控
uchar code conled[13]={0x13,0x1B,0x12,0x11,0x10,0x18,0x19,0x1A,0x0E,0x0F,0x04,0x0C,0x0D,};
//LED灯段码
uchar code aldm[8]={0xBF,0x7F,0xFB,0xF7,0xEF,0xFE,0xDF,0xFD,};


uchar code ibitval[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
/*------------------------变量区------------------------*/

uint   data ms_num;
long   data ms_count[2];
long   data ms_count_bak[2];
long   data pulse_count;
long   data pulse_count_bak;
long   data sum_pulse_count;
long   data sum_pulse_count_bak;
uchar  data pulse_over0;
uchar  data pulse_over0_bak;
uchar  data TH0_bak;
uchar  data TL0_bak;

uchar  idata second_bak;

uchar  idata led_count;          // 显示步骤
uchar  idata ch1;                // 显示临时变量
uchar  idata ch2;                // 显示临时变量

uchar  idata i;                  // 定时变量

uchar  idata ch[6];              // 显示临时变量
uchar  idata led[16];            // 显示内容
uchar  idata displayrun;         // 显示状态

uchar bdata flag1;                // 报警灯标志
sbit bal1             = flag1^0;  // 计时器指示
sbit bal2             = flag1^1;  // 定时器指示
sbit bal3             = flag1^2;  // 报警1
sbit bal4             = flag1^3;  // 报警2
sbit bal5             = flag1^4;  // 脉冲
sbit bal6             = flag1^5;  // 运行
sbit bal7             = flag1^6;  // 自动
sbit bal8             = flag1^7;  // LOCK



uchar bdata flag3;
sbit bDogstatue       = flag3^0;  // 读狗标志
sbit bkeyclear        = flag3^1;  // 无按键标志
sbit bCshStatue       = flag3^2;  // 参数修改标志
sbit bsecondadd       = flag3^4;  // 秒标志
sbit bout1            = flag3^5;  // 通道1报警标志
sbit bout2            = flag3^6;  // 通道2报警标志
sbit b_TJ             = flag3^7;  // 变送调校标志

uchar bdata flag4;
sbit baorm            = flag4^0; // 手/自动清零标志
sbit bct              = flag4^1;
sbit bnumfull         = flag4^2; // 0:计数寄存器满清零,1:计数寄存器满保持
sbit bkeyrun          = flag4^3;
sbit bkeystop         = flag4^4;
sbit bkeyrst          = flag4^5;
sbit b_run_stop       = flag4^6; // 秒计时起.停标志
sbit bkeyrunstop      = flag4^7; // 单键控制启动,停止标志



uchar idata leng;                  // 显示长度
uchar idata Bclk;                  // 按键值
uchar idata fun;                   // 使能变量

uchar idata  modtimenum;

uchar idata oldoutkeyval;
uchar idata newoutkeyval;
uchar idata time[7];
uchar idata mm;
uchar idata ss;
uchar idata mm_bak;
uchar idata ss_bak;


uchar idata De;                  // 站号
uchar idata err[2];              // 0:正常,1:超上限,2:超下限,3:闪烁报警
uint  idata CRC2;

#define DTLENG 12
#define EEPENG 512
uchar  idata DtVal[DTLENG];           // 动态参数

uchar xdata iRece;
uchar xdata LSend;
uchar xdata iSend;

uchar xdata Recebuf[20];          // 接收缓冲区
uchar xdata CRC;
uchar xdata Sendbuf[65];          // 发送缓冲区
uchar xdata SENDTIME;
uchar xdata ENDSENDTIME;
uint  xdata BsTjval[2];

long  xdata sum_pulse_count_last;
uchar xdata DogVal[256];

long  xdata sum_time_count[2];
long  xdata sum_time_count_bak[2];
//------------------------------通用函数-------------------------------------------


void  InitCount(void);



void  InitPulse(void);


void  read_alleep(void);

void  wpvled(long longval,unsigned char dot);
void  wsvled(long longval,unsigned char dot);


void  scanoutkey (void);
void  wpa(void);
void  walled(void);
/*---------------------------------------------------------------------------------*/

void  InitLed(void);
void  display(unsigned int st);
void  refcsh(void);

void  run_prs(void);
void  stop_prs(void);
void  xiansi(void);
void  count_prs(void);
void  timer_prs(void);
void  altime_prs(void);
void  initcsh(void);
void  alarm_prs(void);
void  save_prs(void);
void  comm_prs(void);
void  refram(uchar add);

void  bs_prs(void);
void  Bsout(int v);


//int   SwpToHex(uchar addh, uchar addl);
//void  HexToSwp(int inthex, uchar add);
void  setsendval(val);                    // 发送缓冲区值设置
void  Mcrc (unsigned int jj);              // Modbus 校验字计算

/*-----------------------------------------------------------------------------------*/
void  Dogout(uchar v);
uchar rDog(uchar x,uchar y);
float eeprom_read(uchar x, uchar y);
void  wDog(uchar addh,uchar addl, uchar val);
void  eeprom_write(uchar x, float val, uchar y);
void  InitDog(void);

/*-----------------------------------------------------------------------------------*/



/*-----------------------------------主函数区----------------------------------------*/
void main (void) 
{

        for(i=0;i<DTLENG;i++){DtVal[i]=0;}displayrun=0;
        DtVal[1]=MTYPE;
        InitIo();    /* 初始化IO */
        InitEx();    /* 初始化外部中断 */
        InitDog();   /* 初始化看门狗 */
        InitIP();    /* 初始化中断优先级*/
        InitIE();    /* 初始化中断控制*/

        led[LEDST]=255;led[LEDED]=255;
        wpvled(888888,0);
        wsvled(888888,0);
        for(i=0;i<50;i++){
            nop(250); nop(250); nop(250);
            nop(250); nop(250); nop(250);
            nop(250); nop(250); nop(250);
        }
        /* 读取初始化标志 */
        if(rDog(0,ADRFLAG)!=MTYPE){
                if(rDog(0,ADRFLAG)!=MTYPE){
                       if(rDog(0,ADRFLAG)!=MTYPE){initcsh();}
                }
        }
        SENDTIME=0; CON_485=1;
        starttime();
        read_alleep();
        if(DogVal[ADRCT]<=2){
             if(DogVal[ADRRUN]==0){bkeyrunstop=0; stop_prs();}
             else{bkeyrunstop=1; run_prs();}
        }
        refcsh();
        InitPulse();
        if(DogVal[ADRSAVE]==1){save_prs();}
        InitLed();


        while(1){
             if(DogVal[ADRCT]==0){count_prs(); alarm_prs(); bs_prs();}
             else if(DogVal[ADRCT]==1){timer_prs() ;alarm_prs(); bs_prs();}
             //else if(DogVal[ADRCT]==2){altime_prs(); bal4=0; P_AL2=1; bout2=bal4;}
             else if(DogVal[ADRCT]==2){altime_prs();}

             key_prs();
             xiansi();
             comm_prs();
        }
}

/*-------------------------------------------------------------------------*/
/* 外部0中断 */
void exint0 (void) interrupt 0 {
}

/* 定时器0中断 */
void timer0 (void) interrupt 1 {
    if(bct==0){//计数器
        if(pulse_over0==0){
            if(baorm==1){TR0=0;bnumfull=1;}
            else{
                 TH0=TH0_bak;TL0=TL0_bak;
                 pulse_over0=pulse_over0_bak;
            }
            bout1=1; ms_count[0]=ms_count_bak[0];
        }
        else{pulse_over0--;}
    }
    else{//定时器  10ms定时
        //TL0=0X00;TH0=0XD9;
        TL0=0X0D;TH0=0XD9;
        //TL0=0X0C;TH0=0XD9;
        sum_pulse_count_bak++;
        if(sum_pulse_count_bak>=pulse_count_bak){
            if(baorm==1){TR0=0; bnumfull=1;}else{sum_pulse_count_bak=0;}
            bout1=1; ms_count[0]=ms_count_bak[0];
       }
    }
}

/* 外部1中断 */
void exint1 (void) interrupt 2 {
        switch (led_count){
               case  0:
	       case  1:
	       case  2:
	       case  3:
	       case  4:
	       case  5:
               case  6:
               case  7:
               case  8:
               case  9:
               case  10:
               case  11:// PV,SV显示
		      ch1=leddm[led[led_count]];ch2=0;
                      if ((ch1&0x01)==0x01){ch2=ch2+0x80;}
	              if ((ch1&0x02)==0x02){ch2=ch2+0x40;}
	              if ((ch1&0x04)==0x04){ch2=ch2+0x20;}
	              if ((ch1&0x08)==0x08){ch2=ch2+0x10;}
		      if ((ch1&0x10)==0x10){ch2=ch2+0x08;}
		      if ((ch1&0x20)==0x20){ch2=ch2+0x04;}
		      if ((ch1&0x40)==0x40){ch2=ch2+0x02;}
		      if ((ch1&0x80)==0x80){ch2=ch2+0x01;}
                      P0=0xFF;
                      PAval=conled[led_count];wpa();
                      if ((time1<25)||(led_count<led[LEDST])||(led_count>led[LEDED])){P0=~ch2;}
                      break;
	      case 12:// LED显示
                      P0=0xFF;
                      PAval=conled[led_count];wpa();
                      P0=led[led_count];
                      break;
              case  13:// 按键扫描
                      
                      break;
             default:
                      break;
        }
	led_count++; if(led_count>13){led_count=0;}

        if(bout1==1){
              bal3=1; P_AL1=0;
              if(ms_count_bak[0]>0){
                  ms_count[0]--;
                  if(ms_count[0]<=0){ms_count[0]=0; bal3=0;P_AL1=1;bout1=0;}
              }              
        }

        if(bout2==1){
              bal4=1; P_AL2=0;
              if(ms_count_bak[1]>0){
                  ms_count[1]--;
                  if(ms_count[1]<=0){ms_count[1]=0; bal4=0; P_AL2=1;bout2=0;}
              }
        }

        ms_num++;if(ms_num>=200){ms_num=0; bsecondadd=1;}

        if(modtimenum < 10){modtimenum++;
            if(modtimenum >= 10){
                if(Recebuf[0]==De) {bswpcommok=0;bmodcommok=1;}
                else {iRece=0;}
            }
        }
}

/* 定时器1中断 */
void timer1 (void) interrupt 3 {
     //pulse_over1++;
}



/*--------------------------------函数区----------------------------------------*/

void wpvled(long longval,unsigned char dot){
    unsigned char minus;
    unsigned char i;

    if(longval==OVERHIGH){led[0]=0x0A;led[1]=0x0A;led[2]=0x1F;led[3]=0x18;led[4]=0x0A;led[5]=0x0A;} // -OH-
    else if(longval==OVERLOW){led[0]=0x0A;led[1]=0x0A;led[2]=0x1F;led[3]=0x1C;led[4]=0x0A;led[5]=0x0A;} // -OL-
    else{
	//取符号位
	if(longval>=0){minus=FALSE;}else{longval=0-longval;minus=TRUE;}

	for(i=0;i<PVWEI;i++){
		ch[PVWEI0-i]=longval%10;
		if(dot>0 && dot==i) {ch[PVWEI0-i]+=0x30;}
		if(longval==0 && ch[PVWEI0-i]==0 && dot<i){
			if(minus==FALSE){ch[PVWEI0-i]=0x10;}else{ch[PVWEI0-i]=0x0A;minus=FALSE;}
		}
		else if(i==PVWEI0){if(minus==TRUE){ch[0]+=0x0A;minus=FALSE;}}
		longval=longval/10;
	}
	for(i=0;i<PVWEI;i++){led[i]=ch[i];}
   }
}

//将有符号整型数显示在SV的窗口
void wsvled(long longval,unsigned char dot){
    unsigned char minus;
    unsigned char i;

    if(longval>=OVERHIGH){led[6]=0x0A;led[7]=0x0A;led[8]=0x1F;led[9]=0x18;led[10]=0x0A;led[11]=0x0A;}// -OH-
    else if(longval<=OVERLOW){led[6]=0x0A;led[7]=0x0A;led[8]=0x1F;led[9]=0x1C;led[10]=0x0A;led[11]=0x0A;}// -OL-
    else{
	//取符号位
	if(longval>=0){minus=FALSE;}else{longval=0-longval; minus=TRUE;}

	for(i=0;i<SVWEI;i++){
		ch[SVWEI0-i]=longval%10;
		if(dot>0 && dot==i) {ch[SVWEI0-i]+=0x30;}
		if(longval==0 && ch[SVWEI0-i]==0 && dot<i){
			if(minus==FALSE){//正数添加空格
				if((displayrun<0x80) ||(displayrun>(0x8B-i))){ch[SVWEI0-i]=0x10;}
			}
			else{//负数添加负号
				if(displayrun<0x80 || (i>=5)){ch[SVWEI0-i]=0x0A;minus=FALSE;}
			}
		}
		else if(i==SVWEI0){if(minus==TRUE){ch[0]+=0x0A;minus=FALSE;}}
		longval=longval/10;
	}
	for(i=0;i<SVWEI;i++){led[i+PVWEI]=ch[i];}
    }
}

void walled(void){
        if(bal1==1){led[12]=led[12]&aldm[0];}else{led[12]=led[12]|(~aldm[0]);}
        if(bal2==1){led[12]=led[12]&aldm[1];}else{led[12]=led[12]|(~aldm[1]);}
        if(bal3==1){led[12]=led[12]&aldm[2];}else{led[12]=led[12]|(~aldm[2]);}
        if(bal4==1){led[12]=led[12]&aldm[3];}else{led[12]=led[12]|(~aldm[3]);}
        if(bal5==1){led[12]=led[12]&aldm[4];}else{led[12]=led[12]|(~aldm[4]);}
        if(bal6==1){led[12]=led[12]&aldm[5];}else{led[12]=led[12]|(~aldm[5]);}
        if(bal7==1){led[12]=led[12]&aldm[6];}else{led[12]=led[12]|(~aldm[6]);}
        if(bal8==1){led[12]=led[12]&aldm[7];}else{led[12]=led[12]|(~aldm[7]);}
}

void initcsh(void){

     eeprom_write(ADRCLK,  132,1);
     eeprom_write(ADRDE,     1,1);
     eeprom_write(ADRBT,     5,1);
     eeprom_write(ADRCP,     0,1);
     eeprom_write(ADRCT,     0,1);
     eeprom_write(ADRDOT,    0,1);

     eeprom_write(ADRALT1,   8,1);
     eeprom_write(ADRALT2,   8,1);
     eeprom_write(ADRALT3,   8,1);
     eeprom_write(ADRALT4,   8,1);
     eeprom_write(ADRALT5,   0,1);
     eeprom_write(ADRALT6,   0,1);

     eeprom_write(ADRCONT,1000,4);
     eeprom_write(ADRAL,  1000,4);
     eeprom_write(ADROUTH,   0,4);
     eeprom_write(ADROUTL,1000,4);

     eeprom_write(ADRINC,     0,1);
     eeprom_write(ADROUT,    0,1);
     eeprom_write(ADRRST,    0,1);
     eeprom_write(ADRAUTO,   0,1);
     eeprom_write(ADRTUNI,   0,1);
     eeprom_write(ADRSAVE,   0,1);
     eeprom_write(ADRLOCK,   0,1);
     eeprom_write(ADRRUN,    0,1);
     eeprom_write(ADRALT,    0,1);
     eeprom_write(ADRINT,    0,1);
     eeprom_write(ADRPB,     0,2);
     eeprom_write(ADRKK,  1000,2);

     eeprom_write(ADRPB,     0,4);
     eeprom_write(ADRKK,     1,4);
     eeprom_write(ADROUT1, 0.2,4);
     eeprom_write(ADROUT2, 0.2,4);

     eeprom_write(ADRFLAG,MTYPE,1);
}
void refcsh(void){
     De = eeprom_read(ADRDE,1);
     bCp = eeprom_read(ADRCP,1);

     if(DogVal[ADRCT]<=2){
          baorm=eeprom_read(ADRAUTO,1);
          if(baorm==0){// 连续模式,自动清零
                bal7=1;bnumfull=0;
                if(bal6==1){
                    if(DogVal[ADRCT]==1 && DogVal[ADRTUNI]>1){TR0=0; b_run_stop=1; runtime();}
                    else if(DogVal[ADRCT]==2){TR0=0; b_run_stop=1;}
                    else{TR0=1; b_run_stop=0;}
                }
           }
           else{bal7=0;}
     }
     else{bal7=1;}

     if(eeprom_read(ADRLOCK,1)==0){bal8=0;}else{bal8=1;}

     InitComm();  /* 初始化串口 */
}

/*
int SwpToHex(uchar addh, uchar addl){  // SWP格式数据转换为十六进制数
        int IntHex;
        IntHex=(addh & 0x7F)*256+addl;
        if(addh>=128){IntHex=0-IntHex;}
        return IntHex;

}
*/

/*
void HexToSwp(int inthex, uchar add){  // 十六进制数转换为SWP格式数据
        if(inthex>=0){ DtVal[add]=inthex&0x00ff; DtVal[add+1]=inthex>>8;}
        else{DtVal[add]=(0-inthex)&0x00ff; DtVal[add+1]=((0-inthex)>>8)|0x80;}
}
*/

void refram(uchar add){
     switch(add){
         case ADRDE:
             De=eeprom_read(ADRDE,1);
             break;
         case ADRBT:
             InitComm();
             break;
         case ADRCP:
             bCp = eeprom_read(ADRCP,1);
         case  ADRAUTO:
             break;
         default:
             break;
     }
}



void InitLed(void){
     led[0]=0X10;led[1]=0X13;led[2]=0X16;led[3]=0X0A;led[4]=0X03;led[5]=0X05;
     led[6]=0X10;led[7]=0X10;led[8]=0X25;led[9]=0x31;led[10]=VER/10;led[11]=VER%10;
     delay();//显示软件版本号

     led[0]=0X10;led[1]=0X10;led[2]=0X10;led[3]=0X10;led[4]=0X14;led[5]=0X15;
     wsvled(De,0);
     delay();//显示DE
}



//按键扫描


//修改参数
//st=开始修改的参数位置
void display(unsigned int st){

unsigned char i;
unsigned char wei;
unsigned char datadot;
unsigned char cshlevel;
unsigned char changemenu;
unsigned char wen;
unsigned char change;
float         high;
float         low;
float         newdata;
float         olddata;
float         ftemp;
long          longdata;

    if(st==LEVEL1ST){cshlevel=1;}
    else if(st==LEVEL2ST){cshlevel=2;}
    else if(st==LEVEL3ST){cshlevel=3;}

    for (bCshStatue=0,leng=st;leng<100;){

         high=caidan[leng].hign;
         low =caidan[leng].low;

              if(leng==60){newdata=bcdtohex(ds1302_rbyte(DS1302COM_RYEAR));}
         else if(leng==61){newdata=bcdtohex(ds1302_rbyte(DS1302COM_RMONTH));}
         else if(leng==62){newdata=bcdtohex(ds1302_rbyte(DS1302COM_RDATE));}
         else if(leng==63){newdata=bcdtohex(ds1302_rbyte(DS1302COM_RHOUR));}
         else if(leng==64){newdata=bcdtohex(ds1302_rbyte(DS1302COM_RMINUTE));}
         else if(leng==65){newdata=bcdtohex(ds1302_rbyte(DS1302COM_RSECOND));}
         else{newdata=eeprom_read(caidan[leng].address,caidan[leng].leng);}
         olddata=newdata;
         if (leng==0){Bclk=newdata;}

         if(caidan[leng].dot==0x0f){
             datadot=0;
             for(i=0;i<5;i++){
                 ftemp=newdata*10;
                 if(ftemp>999999){break;}
                  else if(ftemp<-199999){break;}
                  else{newdata=ftemp;datadot++;}
             }
         }
         else if (caidan[leng].dot < 6){datadot=caidan[leng].dot;}
         else {datadot=eeprom_read(caidan[leng].dot,1);}
         if(newdata>=0){longdata=newdata+0.5;}else{longdata=newdata-0.5;}

         led[0]=0x10;led[1]=0x10;
         if (caidan[leng].biaoti[0]>=0x30) {led[2]=caidan[leng].biaoti[0]-0x30;}else {if(caidan[leng].biaoti[0]==45){led[2]=0x0A;}else{led[2]=0x10;}}
         if (caidan[leng].biaoti[1]>=0x30) {led[3]=caidan[leng].biaoti[1]-0x30;}else {if(caidan[leng].biaoti[1]==45){led[3]=0x0A;}else{led[3]=0x10;}}
         if (caidan[leng].biaoti[2]>=0x30) {led[4]=caidan[leng].biaoti[2]-0x30;}else {if(caidan[leng].biaoti[2]==45){led[4]=0x0A;}else{led[4]=0x10;}}
         if (caidan[leng].biaoti[3]>=0x30) {led[5]=caidan[leng].biaoti[3]-0x30;}else {if(caidan[leng].biaoti[3]==45){led[5]=0x0A;}else{led[5]=0x10;}}
         displayrun=0; wsvled(longdata,datadot); led[LEDST]=255;led[LEDED]=255;

         for (;;) {
            if (keyst==0){break;}
            if (keyst!=KEY_SET) {keytime=0;}
            //if ((keytime>SETKEYTI)||(nokeytime>NOKEYTI)){return;}
         }
         for (;;) {
            if (keyst==KEY_SET){
                      fkey=keyst;
                      changemenu=0;
                      break;
            }
            else if(keyst==KEY_RIGHT){
                      for(;;){leng++;
                               if(cshlevel==1){if(leng==LEVEL2ST){leng=LEVEL1ST;}}
                          else if(cshlevel==2){if(leng==LEVEL3ST){leng=LEVEL2ST;}}
                          else if(cshlevel==3){if(leng==LEVEL4ST){leng=LEVEL3ST;}}

                          if(caidan[leng].nenable2==0xFF){continue;}
                          if(caidan[leng].nenable1!=0x00){
                              wen=eeprom_read(caidan[leng].nenable1,1);
                              if(wen==0){if((caidan[leng].nenable2&0x01)==0){continue;}}
                              if(wen==1){if((caidan[leng].nenable2&0x02)==0){continue;}}
                              if(wen==2){if((caidan[leng].nenable2&0x04)==0){continue;}}
                          }

                          if(caidan[leng].wenable!=0){
                              wen=eeprom_read(caidan[leng].wenable,1);
                              if(wen==1){continue;}else{break;}
                          }
                          else{break;}
                      }
                      changemenu=1; break;
            }
            else if(keyst==KEY_DOWN){
                      for(;;){leng--;
                               if(cshlevel==1){if(leng==0xFF)     {leng=LEVEL1END;}}
                          else if(cshlevel==2){if(leng==LEVEL1END){leng=LEVEL2END;}}
                          else if(cshlevel==3){if(leng==LEVEL2END){leng=LEVEL3END;}}

                          if(caidan[leng].nenable2==0xFF){continue;}
                          if(caidan[leng].nenable1!=0x00){
                              wen=eeprom_read(caidan[leng].nenable1,1);
                              if(wen==0){if((caidan[leng].nenable2&0x01)==0){continue;}}
                              if(wen==1){if((caidan[leng].nenable2&0x02)==0){continue;}}
                              if(wen==2){if((caidan[leng].nenable2&0x04)==0){continue;}}
                          }
                          if(caidan[leng].wenable!=0){
                              wen=eeprom_read(caidan[leng].wenable,1);
                              if(wen==1){continue;}else{break;}
                          }
                          else{break;}
                      }
                      changemenu=1; break;
            }
            else if(keyst==KEY_CHANGE){return;}
            else {keytime=0;}
            if (nokeytime>NOKEYTI){return;}
         }
         if(changemenu==1){continue;}
         //单个参数修改
         if (leng==0||Bclk==132||Bclk==123) {wei=11;led[LEDST]=11;led[LEDED]=11;}else{continue;}
         for (change=1;change<2;){

               if(longdata>high){longdata=high;}else if(longdata<low){longdata=low;}
               newdata=longdata;
               if(caidan[leng].dot==0x0f){
                   if(datadot==1){newdata=newdata/10;}
                   else if(datadot==2){newdata=newdata/100;}
                   else if(datadot==3){newdata=newdata/1000;}
                   else if(datadot==4){newdata=newdata/10000;}
                   else if(datadot==5){newdata=newdata/100000;}
               }
               displayrun=wei+0x80;
               wsvled(longdata,datadot);

               switch (scankey()){
                   case  NOKEY:
                       change=0;
                       break;
                   case  SETKEY:
                            if(leng==60){
                               ds1302_wbyte(DS1302COM_WWP,0x00); // 控制命令,WP=0,写操作
                               ds1302_wbyte(DS1302COM_WYEAR,hextobcd(newdata)); // year
                               ds1302_wbyte(DS1302COM_WWP,0x80); // 控制命令,WP=0,写操作
                      }
                      else if(leng==61){
                               ds1302_wbyte(DS1302COM_WWP,0x00); // 控制命令,WP=0,写操作
                               ds1302_wbyte(DS1302COM_WMONTH,hextobcd(newdata)); // month
                               ds1302_wbyte(DS1302COM_WWP,0x80); // 控制命令,WP=0,写操作
                      }
                      else if(leng==62){
                               ds1302_wbyte(DS1302COM_WWP,0x00); // 控制命令,WP=0,写操作
                               ds1302_wbyte(DS1302COM_WDATE,hextobcd(newdata)); // date
                               ds1302_wbyte(DS1302COM_WWP,0x80); // 控制命令,WP=0,写操作
                      }
                      else if(leng==63){
                               ds1302_wbyte(DS1302COM_WWP,0x00); // 控制命令,WP=0,写操作
                               ds1302_wbyte(DS1302COM_WHOUR,hextobcd(newdata)); // hour
                               ds1302_wbyte(DS1302COM_WWP,0x80); // 控制命令,WP=0,写操作
                      }
                      else if(leng==64){
                               ds1302_wbyte(DS1302COM_WWP,0x00); // 控制命令,WP=0,写操作
                               ds1302_wbyte(DS1302COM_WMINUTE,hextobcd(newdata)); // MINUTE
                               ds1302_wbyte(DS1302COM_WWP,0x80); // 控制命令,WP=0,写操作
                      }
                      else if(leng==65){
                               ds1302_wbyte(DS1302COM_WWP,0x00); // 控制命令,WP=0,写操作
                               ds1302_wbyte(DS1302COM_WSECOND,hextobcd(newdata)); // year
                               ds1302_wbyte(DS1302COM_WWP,0x80); // 控制命令,WP=0,写操作
                      }
                      else{eeprom_write(caidan[leng].address,newdata,caidan[leng].leng);}

                      if((newdata!=olddata) && (leng==1 || leng==23 || leng==40)){bCshStatue=1;}
                      if (leng==0){Bclk=newdata;}
                      change=2;
                      break;
                   case  UPKEY:
                       if(caidan[leng].dot==0x0f){
                            if(datadot==5){datadot=0;}else{datadot++;}
                       }
                       break;
                   case  DOWNKEY:
                       if (wei==11)      {longdata-=1;}
                       else if (wei==10) {longdata-=10;}
                       else if (wei==9)  {longdata-=100;}
                       else if (wei==8)  {longdata-=1000;}
                       else if (wei==7)  {longdata-=10000;}
                       else if (wei==6)  {longdata-=100000;}
                       break;
                   case  LEFTKEY:
                       if (wei==6){wei=11;}else{wei--;}
                       if      ((high<10)    &&(wei==10)){wei=11;}
                       else if ((high<100)   &&(wei==9)) {wei=11;}
                       else if ((high<1000)  &&(wei==8)) {wei=11;}
                       else if ((high<10000) &&(wei==7)) {wei=11;}
                       else if ((high<100000)&&(wei==6)) {wei=11;}
                       led[LEDST]=wei;led[LEDED]=wei;
                       break;
                   case RIGHTKEY:
                       if (wei==11)      {longdata+=1;}
                       else if (wei==10) {longdata+=10;}
                       else if (wei==9)  {longdata+=100;}
                       else if (wei==8)  {longdata+=1000;}
                       else if (wei==7)  {longdata+=10000;}
                       else if (wei==6)  {longdata+=100000;}
                       break;
                   case S05KEY:
                           if(leng==60){
                               ds1302_wbyte(DS1302COM_WWP,0x00); // 控制命令,WP=0,写操作
                               ds1302_wbyte(DS1302COM_WYEAR,hextobcd(newdata)); // year
                               ds1302_wbyte(DS1302COM_WWP,0x80); // 控制命令,WP=0,写操作
                      }
                      else if(leng==61){
                               ds1302_wbyte(DS1302COM_WWP,0x00); // 控制命令,WP=0,写操作
                               ds1302_wbyte(DS1302COM_WMONTH,hextobcd(newdata)); // month
                               ds1302_wbyte(DS1302COM_WWP,0x80); // 控制命令,WP=0,写操作
                      }
                      else if(leng==62){
                               ds1302_wbyte(DS1302COM_WWP,0x00); // 控制命令,WP=0,写操作
                               ds1302_wbyte(DS1302COM_WDATE,hextobcd(newdata)); // date
                               ds1302_wbyte(DS1302COM_WWP,0x80); // 控制命令,WP=0,写操作
                      }
                      else if(leng==63){
                               ds1302_wbyte(DS1302COM_WWP,0x00); // 控制命令,WP=0,写操作
                               ds1302_wbyte(DS1302COM_WHOUR,hextobcd(newdata)); // hour
                               ds1302_wbyte(DS1302COM_WWP,0x80); // 控制命令,WP=0,写操作
                      }
                      else if(leng==64){
                               ds1302_wbyte(DS1302COM_WWP,0x00); // 控制命令,WP=0,写操作
                               ds1302_wbyte(DS1302COM_WMINUTE,hextobcd(newdata)); // MINUTE
                               ds1302_wbyte(DS1302COM_WWP,0x80); // 控制命令,WP=0,写操作
                      }
                      else if(leng==65){
                               ds1302_wbyte(DS1302COM_WWP,0x00); // 控制命令,WP=0,写操作
                               ds1302_wbyte(DS1302COM_WSECOND,hextobcd(newdata)); // year
                               ds1302_wbyte(DS1302COM_WWP,0x80); // 控制命令,WP=0,写操作
                      }
                      else{eeprom_write(caidan[leng].address,newdata,caidan[leng].leng);}
                      if((newdata!=olddata) && (leng==1 || leng==23 || leng==40)){bCshStatue=1;}
                      if (leng==0){Bclk=newdata;}
                      if ((leng==0)&&(newdata==132)){cshlevel=2; leng=LEVEL2ST;}
                      else if ((leng==0)&&(newdata==123)){cshlevel=3; leng=LEVEL3ST;}
                      keytime=0;
                      change=2;
                      break;
                   default:
                      return;
            }
         }
     }
}

void key_prs(void){

     if(DogVal[ADRCT]==1 && DogVal[ADRINT]>=2 && DogVal[ADRINT]<=3){
         if(DogVal[ADRINT]==2 || DogVal[ADRINT]==3){
             if(keyst==KEY_UP || newoutkeyval==0x01){
                 if(brekey==0){brekey=1;bkeyrunstop=1;bkeyrun=1;}
             }
             else{brekey=0; bkeyrunstop=0;bkeystop=1;}
         }
     }

     switch (scankey()) {
          case SETKEY:
              break;
          case  DOWNKEY:  // RST
              if((DogVal[ADRLOCK]==0 || DogVal[ADRLOCK]==1) && DogVal[ADRCT]<=2){bkeyrst=1;}
              break;
          case  LEFTKEY:
              break;
          case  RIGHTKEY: // STOP
              if(DogVal[ADRCT]==1 && DogVal[ADRINT]>=2 && DogVal[ADRINT]<=3){}
              else{
                  if((DogVal[ADRLOCK]==0 || DogVal[ADRLOCK]==2) && DogVal[ADRCT]<=2){
                      if(bkeyrunstop==1){bkeyrunstop=0;bkeystop=1;}
                  }
              }
              break;
          case UPKEY:     // RUN
              if(DogVal[ADRCT]==1 && DogVal[ADRINT]>=2 && DogVal[ADRINT]<=3){}
              else{
                  if((DogVal[ADRLOCK]==0 || DogVal[ADRLOCK]==2) && DogVal[ADRCT]<=2){
                      if(bkeyrunstop==0){bkeyrunstop=1;bkeyrun=1;}
                  }
              }
              break;
          case  CHANGEKEY:
              // if(bfrq==0){bfrq=1;}else{bfrq=0;}
              break;
          case  SSETDNKEY:
              break;
          case  S05KEY:
              if(bkeyrunstop==1 && DogVal[ADRCT]<2){TR0=0;b_run_stop=0;}
              display(0); DtVal[0]=1; refcsh(); InitPulse();
              if(DogVal[ADRSAVE]==1){save_prs();}
              displayrun=0; leng=0;
              if(bkeyrunstop==1 && DogVal[ADRCT]<2){
                   if(bnumfull==0){
                       if(DogVal[ADRCT]==1 && DogVal[ADRTUNI]>1){TR0=0; b_run_stop=1; runtime();}
                       else if(DogVal[ADRCT]==2){TR0=0; b_run_stop=1;}
                       else{TR0=1; b_run_stop=0;}
                   }
              }
              break;
          case  SUP05KEY:
              break;
          default:
              break;
     }

     if(DogVal[ADRCT]<=2){scanoutkey();}

     if(bkeyrst==1){bkeyrst=0;
         InitPulse(); bout2en=0;
         if(bal6==1 && baorm==1){
              if(DogVal[ADRCT]==1 && DogVal[ADRTUNI]>1){TR0=0; b_run_stop=1; runtime();}
              else if(DogVal[ADRCT]==2){TR0=0; b_run_stop=1;}
              else{TR0=1; b_run_stop=0;}
         }
     }
     else if(bkeystop==1){bkeystop=0; stop_prs();}
     else if(bkeyrun==1){bkeyrun=0; run_prs();}
}

void xiansi(void){
    union word_byte z;

    if(DogVal[ADRCT]<2){
            z.fval=sum_pulse_count;
            if(eeprom_read(ADRKK,4)==0){z.fval=z.fval/0.000001+eeprom_read(ADRPB,4);}
            else{z.fval=z.fval/eeprom_read(ADRKK,4)+eeprom_read(ADRPB,4);}

            DtVal[2]=z.bytes.char1;
            DtVal[3]=z.bytes.char2;
            DtVal[4]=z.bytes.char3;
            DtVal[5]=z.bytes.char4;

            if(DogVal[ADRDOT]==1){z.fval*=10;}
            else if(DogVal[ADRDOT]==2){z.fval*=100;}
            else if(DogVal[ADRDOT]==3){z.fval*=1000;}

            if(z.fval>999999){z.fval=999999; err[0]=1;}
            else if(z.fval<-199999){z.fval=-199999; err[0]=2;}
            else{err[0]=0;}

            if(err[0]==1){wpvled(OVERHIGH,0);}
            else if(err[0]==2){wpvled(OVERLOW,0);}
            else{wpvled(z.fval,DogVal[ADRDOT]);}

/*-------------------------------------------------------------------------------*/

            z.fval=eeprom_read(ADRCONT,4);
            wsvled(z.fval,DogVal[ADRDOT]);

            if(DogVal[ADRDOT]==1){z.fval/=10;}
            else if(DogVal[ADRDOT]==2){z.fval/=100;}
            else if(DogVal[ADRDOT]==3){z.fval/=1000;}

            DtVal[6]=z.bytes.char1;
            DtVal[7]=z.bytes.char2;
            DtVal[8]=z.bytes.char3;
            DtVal[9]=z.bytes.char4;
    }
    /*
    else{
         led[0]=0X10; led[1]=MONTH/10; led[2]=MONTH%10;
         led[3]=0X0A; led[4]=DATE/10;  led[5]=DATE%10;

         led[6]=HOUR/10;    led[7]=HOUR%10+0X30;
         led[8]=MINUTE/10;  led[9]=MINUTE%10+0X30;
         led[10]=SECOND/10; led[11]=SECOND%10;

    }
    */
    else{
         if((DogVal[ADRINT]%2)==0){z.lval=sum_time_count[0];}
         else{
             z.lval=(long)DogVal[ADRALT1]*3600+DogVal[ADRALT2]*60+DogVal[ADRALT3];
             z.lval-=sum_time_count[0];
         }

         DtVal[3]=z.lval/3600; z.lval=z.lval%3600;
         DtVal[4]=z.lval/60;   z.lval=z.lval%60;
         DtVal[5]=z.lval;      DtVal[2]=0;

         led[0]=DtVal[3]/10;   led[1]=DtVal[3]%10+0X30;
         led[2]=DtVal[4]/10;   led[3]=DtVal[4]%10+0X30;
         led[4]=DtVal[5]/10;   led[5]=DtVal[5]%10;

         led[6] =DogVal[ADRALT1]/10;  led[7] =DogVal[ADRALT1]%10+0X30;
         led[8] =DogVal[ADRALT2]/10;  led[9] =DogVal[ADRALT2]%10+0X30;
         led[10]=DogVal[ADRALT3]/10;  led[11]=DogVal[ADRALT3]%10;

         DtVal[6]=0x00;               DtVal[7]=DogVal[ADRALT1];
         DtVal[8]=DogVal[ADRALT2];    DtVal[9]=DogVal[ADRALT3];

    }
/*-------------------------------------------------------------------------------*/
    led[LEDST]=255;led[LEDED]=255;
    walled();
    DtVal[10]=bal3;
    DtVal[11]=bal4;
}

void count_prs(void){
union word_byte z;

        if (DogVal[ADRINC]==0){    // 计数器递增
              sum_pulse_count=65536-TH0*256-TL0 ; if(sum_pulse_count==65536){sum_pulse_count=0;}
              sum_pulse_count=pulse_count-(pulse_over0*65536)-sum_pulse_count;
        }
        else if(DogVal[ADRINC]==1){// 计数器递减
              sum_pulse_count=65536-TH0*256-TL0 ; if(sum_pulse_count==65536){sum_pulse_count=0;}
              sum_pulse_count=pulse_over0*65536+sum_pulse_count;
        }
        else if(DogVal[ADRINC]==2){// 计数器递增,通道2高时累计
              if(P_PCON==1 && bkeyrunstop==1){TR0=1;}else{TR0=0;}
              sum_pulse_count=65536-TH0*256-TL0 ; if(sum_pulse_count==65536){sum_pulse_count=0;}
              sum_pulse_count=pulse_count-(pulse_over0*65536)-sum_pulse_count;
        }
        else if(DogVal[ADRINC]==3){// 计数器递减,通道2高时累计
              if(P_PCON==1 && bkeyrunstop==1){TR0=1;}else{TR0=0;}
              sum_pulse_count=65536-TH0*256-TL0 ; if(sum_pulse_count==65536){sum_pulse_count=0;}
              sum_pulse_count=(pulse_over0*65536)+sum_pulse_count;
        }
        else if(DogVal[ADRINC]==4){// 通道1递增,通道2低时累计
              if(P_PCON==0 && bkeyrunstop==1){TR0=1;}else{TR0=0;}
              sum_pulse_count=65536-TH0*256-TL0 ; if(sum_pulse_count==65536){sum_pulse_count=0;}
              sum_pulse_count=pulse_count-(pulse_over0*65536)-sum_pulse_count;
        }
        else if(DogVal[ADRINC]==5){// 通道1递减,通道2低时累计
              if(P_PCON==0 && bkeyrunstop==1){TR0=1;}else{TR0=0;}
              sum_pulse_count=65536-TH0*256-TL0 ; if(sum_pulse_count==65536){sum_pulse_count=0;}
              sum_pulse_count=(pulse_over0*65536)+sum_pulse_count;
        }
        else if(DogVal[ADRINC]==6){// 通道1递增,通道2递减
        }
        if(sum_pulse_count>=10000000){sum_pulse_count-=10000000;}

        if(sum_pulse_count_last!=sum_pulse_count){bal5=!bal5;}
        sum_pulse_count_last=sum_pulse_count;

        if(DogVal[ADRSAVE]==1){
              if(DogVal[ADRINC]==0){z.lval=sum_pulse_count;}
              else{z.lval=pulse_count-sum_pulse_count;}
              ram_write(9,0);//写开始标志
              ram_write(0,z.bytes.char1);
              ram_write(1,z.bytes.char2);
              ram_write(2,z.bytes.char3);
              ram_write(3,z.bytes.char4);
              ram_write(4,bnumfull);
              ram_write(9,0);//写开始标志

              ram_write(19,0);//写开始标志
              ram_write(10,z.bytes.char1);
              ram_write(11,z.bytes.char2);
              ram_write(12,z.bytes.char3);
              ram_write(13,z.bytes.char4);
              ram_write(14,bnumfull);
              ram_write(19,0);//写开始标志
        }
}

void timer_prs(void){
union word_byte z;

        if(DogVal[ADRTUNI]==0){
            if(DogVal[ADRINT]%2==0){sum_pulse_count=sum_pulse_count_bak;}
            else{sum_pulse_count=pulse_count-sum_pulse_count_bak;}
        }
        else if(DogVal[ADRTUNI]==1){
           if(DogVal[ADRINT]%2==0){sum_pulse_count=sum_pulse_count_bak/10;}
             else{sum_pulse_count=pulse_count-(sum_pulse_count_bak/10);}
        }
        else {
             SECOND = bcdtohex(ds1302_rbyte(DS1302COM_RSECOND));
	     if(b_run_stop==1){
                 if(second_bak!=SECOND){ss++;if(ss>=60){ss=0;mm++;if(mm>=60){mm=0;}}}

                 if(DogVal[ADRTUNI]==2){
                     if(ss!=ss_bak){sum_pulse_count_bak++;}
                 }
                 else if(DogVal[ADRTUNI]==3){
                     if((ss_bak!=ss) && ((ss%6)==0)){sum_pulse_count_bak++;}
                 }
                 else if(DogVal[ADRTUNI]==4){
                     if(mm!=mm_bak){sum_pulse_count_bak++;}
                 }
                 ss_bak=ss; mm_bak=mm;
                 if(sum_pulse_count_bak>=pulse_count_bak){
                     if(baorm==1){b_run_stop=0;bnumfull=1;}
                     else{sum_pulse_count_bak=0;}
                     bout1=1; ms_count[0]=ms_count_bak[0];
                 }
             }
             second_bak=SECOND;
             if(DogVal[ADRINT]%2==0){sum_pulse_count=sum_pulse_count_bak;}
             else{sum_pulse_count=pulse_count-sum_pulse_count_bak;}
        }

        if(DogVal[ADRSAVE]==1){
              if(DogVal[ADRINT]%2==0){z.lval=sum_pulse_count;}
              else{z.lval=pulse_count-sum_pulse_count;}
              ram_write(9,0);//写开始标志
              ram_write(0,z.bytes.char1);
              ram_write(1,z.bytes.char2);
              ram_write(2,z.bytes.char3);
              ram_write(3,z.bytes.char4);
              ram_write(4,bnumfull);
              ram_write(9,0);//写开始标志

              ram_write(19,0);//写开始标志
              ram_write(10,z.bytes.char1);
              ram_write(11,z.bytes.char2);
              ram_write(12,z.bytes.char3);
              ram_write(13,z.bytes.char4);
              ram_write(14,bnumfull);
              ram_write(19,0);//写开始标志
        }
}

/*
void altime_prs(void){
        if(bsecondadd==1){bsecondadd=0; readtime();
              if(HOUR==DogVal[ADRALT4] && MINUTE==DogVal[ADRALT5] && SECOND==DogVal[ADRALT6]){bout1=1;ms_count=ms_count_bak;}
        }
}
*/

void altime_prs(void){
union word_byte z;

        if(bsecondadd==1){bsecondadd=0;
             SECOND = bcdtohex(ds1302_rbyte(DS1302COM_RSECOND));
             if(SECOND!=second_bak){second_bak=SECOND;
                 if(b_run_stop==1 && bnumfull==0){
                     sum_time_count[0]++;
                  
                     if(sum_time_count[0]>=sum_time_count_bak[0]){
                         if(baorm==0){//自动
                             sum_time_count[0]=0;bnumfull=0;
                             if(ms_count_bak[0]<=0){bal3=0;P_AL1=1;bout1=0;}
                             else{bout1=1; ms_count[0]=ms_count_bak[0];}
                         }
                         else{
                             b_run_stop=0; bnumfull=1;
                             bout1=1; ms_count[0]=ms_count_bak[0];
                         }
                     }
                     else if(ms_count_bak[0]<=0){bal3=0;P_AL1=1;bout1=0;}
                  }
                  
                  if(DogVal[ADRINT]%2==0){sum_time_count[1]=sum_time_count[0];}
                  else{sum_time_count[1]=sum_time_count_bak[0]-sum_time_count[0];}
                  
                     if(DogVal[ADRALT]==1){
                         if(sum_time_count[1]<=sum_time_count_bak[1]){
                             if(bout2en==0){bout2en=1; bout2=1; ms_count[1]=ms_count_bak[1];}
                         }
                         else{bal4=0;P_AL2=1;bout2en=0; bout2=0;}
                     }
                     else if(DogVal[ADRALT]==2){
                         if(sum_time_count[1]>=sum_time_count_bak[1]){
                             if(bout2en==0){bout2en=1; bout2=1; ms_count[1]=ms_count_bak[1];}
                         }
                         else{bal4=0;P_AL2=1;bout2en=0;bout2=0;}
                     }
                     else{bal4=0;P_AL2=1;bout2en=0;bout2=0;}
                 
                 
                 if(DogVal[ADRSAVE]==1){
                     z.lval=sum_time_count[0];
                     ram_write(9,0);//写开始标志
                     ram_write(0,z.bytes.char1);
                     ram_write(1,z.bytes.char2);
                     ram_write(2,z.bytes.char3);
                     ram_write(3,z.bytes.char4);
                     ram_write(4,bnumfull);

                     z.lval=sum_time_count[1];
                     ram_write(5,z.bytes.char1);
                     ram_write(6,z.bytes.char2);
                     ram_write(7,z.bytes.char3);
                     ram_write(8,z.bytes.char4);
                     ram_write(9,1);//写成功标志


                     z.lval=sum_time_count[0];
                     ram_write(19,0);//写开始标志
                     ram_write(10,z.bytes.char1);
                     ram_write(11,z.bytes.char2);
                     ram_write(12,z.bytes.char3);
                     ram_write(13,z.bytes.char4);
                     ram_write(14,bnumfull);

                     z.lval=sum_time_count[1];
                     ram_write(15,z.bytes.char1);
                     ram_write(16,z.bytes.char2);
                     ram_write(17,z.bytes.char3);
                     ram_write(18,z.bytes.char4);
                     ram_write(19,1);//写成功标志

                 }
             }
        }
}

/*-------------------------------------------------------------------------------------*/
void Dogout(uchar v) {  /* 看门狗数据v移位输出 -ok */
     uchar iDog;
     for (iDog=128;iDog>0;iDog/=2) {IIC_SIO=v & iDog;nop(3);IIC_CLK=0;nop(3);IIC_CLK=1;}
}

uchar rDog(uchar x,uchar y){  //读看门狗数据
     uchar iDog;
     CS_DOG=0;
     if(x>=1) {Dogout(0x0b);}  else {Dogout(0x03);}
     Dogout(y);x=0;
     for(iDog=128;iDog>0;iDog/=2) {IIC_CLK=1;nop(3);IIC_CLK=0;nop(3);y=IIC_SIO;x=x+y*iDog;}
     CS_DOG=1;

     return x;
}

float eeprom_read(uchar x, uchar y){
    union word_byte z;
    if(y==1){return DogVal[x];}
    else if(y==2){
         if(DogVal[x+1]>=128){z.word=0-(int)(DogVal[x+1]-128)*256-DogVal[x];}
         else{z.word=(int)(DogVal[x+1])*256+DogVal[x];}
         return z.word;
    }
    else if(y==4){
         z.bytes.char1=DogVal[x];
         z.bytes.char2=DogVal[x+1];
         z.bytes.char3=DogVal[x+2];
         z.bytes.char4=DogVal[x+3];
         return z.fval;
    }

}

void wDog(uchar addh,uchar addl, uchar val)  {
        CS_DOG=1;
        CS_DOG=0;Dogout(0x06);CS_DOG=1;		/*写看门狗允许写命令 */
        CS_DOG=0;if(addh>=1) {Dogout(0x0a);} else{Dogout(0x02);}
	Dogout(addl); Dogout(val);CS_DOG =1;
        nop(250);nop(250);nop(250);nop(250);
        CS_DOG=0;Dogout(0x04); CS_DOG=1;         /* 禁止写 */
        DogVal[addl]=val;
}

void eeprom_write(uchar x, float val, uchar y){
    union word_byte z;
    if(y==1){
         z.ubyte=val;
         wDog(0,x,z.ubyte);
    }
    else if(y==2){
         z.word=val;
         if(z.word<0){z.word=(0-z.word)|0X8000;}
         wDog(0,x,z.bytes.char2);wDog(0,x+1,z.bytes.char1);
    }
    else if(y==4){
         z.fval=val;
         wDog(0,x,  z.bytes.char1);
         wDog(0,x+1,z.bytes.char2);
         wDog(0,x+2,z.bytes.char3);
         wDog(0,x+3,z.bytes.char4);
    }
}

void InitDog(void){
       /* 看门狗状态寄存器初始化子程序, 写看门狗允许写命令, 无复位周期, 片选有效 */
  	CS_DOG=0; Dogout(0x06); CS_DOG=1;
        CS_DOG=0; Dogout(0x01); Dogout(0x30); CS_DOG=1; nop(250); nop(250); nop(250);
        CS_DOG=0; Dogout(0x04); CS_DOG=1;
}

void read_alleep(void){
     uchar i;
     for(i=0;i<0xff;i++){DogVal[i]=rDog(0,i);}
     DogVal[0XFF]=rDog(0,0XFF);
}

/*----------------------------------------------------------------------------------*/
void  InitPulse(void){
uint  uval;
float ftemp;

      sum_pulse_count_last=0;
      sum_pulse_count_bak=0;
      sum_pulse_count=0;
      bnumfull=0;
      ms_count_bak[0]=eeprom_read(ADROUT1,4)*10;
      ms_count[0]=ms_count_bak[0];

      ms_count_bak[1]=eeprom_read(ADROUT2,4)*10;
      ms_count[1]=ms_count_bak[1];

      if(DogVal[ADRCT]==0){
           bct=0; bal1=1,bal2=0; bal5=0;
           InitCount();    //初始化计数器

           //pulse_count=eeprom_read(ADRCONT,4);
                if(DogVal[ADRDOT]==1){ftemp=eeprom_read(ADRCONT,4)/10;}
           else if(DogVal[ADRDOT]==2){ftemp=eeprom_read(ADRCONT,4)/100;}
           else if(DogVal[ADRDOT]==3){ftemp=eeprom_read(ADRCONT,4)/1000;}
           else{ftemp=eeprom_read(ADRCONT,4);}

           ftemp=ftemp*eeprom_read(ADRKK,4)-eeprom_read(ADRPB,4);
           if(ftemp>=1){pulse_count=ftemp;}else{pulse_count=1;}

           pulse_over0_bak=pulse_count/65536;
           uval=pulse_count%65536;
           uval=65536-uval;
           TH0_bak=uval/256;
           TL0_bak=uval%256;

           pulse_over0=pulse_over0_bak;
           TH0=TH0_bak; TL0=TL0_bak;
      }
      else if(DogVal[ADRCT]==1){
           bct=1; bal1=0; bal2=1; bal5=0;
           InitTimer();    //初始化计时器

           //pulse_count=eeprom_read(ADRCONT,4);
                if(DogVal[ADRDOT]==1){ftemp=eeprom_read(ADRCONT,4)/10;}
           else if(DogVal[ADRDOT]==2){ftemp=eeprom_read(ADRCONT,4)/100;}
           else if(DogVal[ADRDOT]==3){ftemp=eeprom_read(ADRCONT,4)/1000;}
           else{ftemp=eeprom_read(ADRCONT,4);}

           ftemp=ftemp*eeprom_read(ADRKK,4)-eeprom_read(ADRPB,4);
           if(ftemp>=1){pulse_count=ftemp;}else{pulse_count=1;}

           TL0=0XF0; TH0=0XD8;
           if(DogVal[ADRTUNI]==1){pulse_count_bak=pulse_count*10;}else{pulse_count_bak=pulse_count;}
      }
      else if(DogVal[ADRCT]==2){
           bct=1; bal1=0; bal2=1; bal5=0;
           //mm=0;  mm_bak=0; ss=0;  ss_bak=0;
           sum_time_count[0]=0; sum_time_count[1]=0;
           sum_time_count_bak[0]=(long)DogVal[ADRALT1]*3600+DogVal[ADRALT2]*60+DogVal[ADRALT3];
           sum_time_count_bak[1]=(long)DogVal[ADRALT4]*3600+DogVal[ADRALT5]*60+DogVal[ADRALT6];
      }
}

void save_prs(void){
union word_byte z;
       if(ram_read(9)==1){
           z.bytes.char1=ram_read(0);
           z.bytes.char2=ram_read(1);
           z.bytes.char3=ram_read(2);
           z.bytes.char4=ram_read(3);
           bnumfull=ram_read(4);
       }
       else{
           z.bytes.char1=ram_read(10);
           z.bytes.char2=ram_read(11);
           z.bytes.char3=ram_read(12);
           z.bytes.char4=ram_read(13);
           bnumfull=ram_read(14);
       }

       if(DogVal[ADRCT]==0){
           pulse_over0=(pulse_count-z.lval)/65536;
           z.uword=(pulse_count-z.lval)%65536;
           z.uword=65536-z.word;
           TH0=z.uword/256;
           TL0=z.uword%256;
       }
       else if(DogVal[ADRCT]==1){
            sum_pulse_count_bak=z.lval;
            if(DogVal[ADRTUNI]==1){sum_pulse_count_bak*=10;}
       }
       else{
           sum_time_count[0]=z.lval;
           if(ram_read(9)==1){
               z.bytes.char1=ram_read(5);
               z.bytes.char2=ram_read(6);
               z.bytes.char3=ram_read(7);
               z.bytes.char4=ram_read(8);
           }
           else{
               z.bytes.char1=ram_read(15);
               z.bytes.char2=ram_read(16);
               z.bytes.char3=ram_read(17);
               z.bytes.char4=ram_read(18);
           }
           sum_time_count[1]=z.lval;
       }
}

void alarm_prs(void){
float ftemp1,ftemp2;

      ftemp1=eeprom_read(ADRKK,4);
      if(ftemp1==0){ftemp1=sum_pulse_count/0.000001+eeprom_read(ADRPB,4);}
      else{ftemp1=sum_pulse_count/ftemp1+eeprom_read(ADRPB,4);}
      ftemp2=eeprom_read(ADRAL,4);

      if(DogVal[ADRALT]==1){if(ftemp1<=ftemp2){bal4=1;P_AL2=0;}else{bal4=0;P_AL2=1;}}
      else if(DogVal[ADRALT]==2){if(ftemp1>=ftemp2){bal4=1;P_AL2=0;}else{bal4=0;P_AL2=1;}}
      else {bal4=0;P_AL2=1;}
      bout2=bal4;
}




/*----------------------外接按键处理------------------------------*/
void scanoutkey(void){

	oldoutkeyval=newoutkeyval;
	      if(P_KEY_STOP==0){newoutkeyval=0x02;}
	else if(P_KEY_START==0){newoutkeyval=0x01;}
	else if(P_KEY_ZERO ==0){newoutkeyval=0x04;}
	else{newoutkeyval=0x00;}

        if(DogVal[ADRCT]==1 && DogVal[ADRINT]>=2 && DogVal[ADRINT]<=3){
             if(DogVal[ADRINT]==2 || DogVal[ADRINT]==3){
                 if(keyst==KEY_UP || newoutkeyval==0x01){if(brekey==0){brekey=1;bkeyrunstop=1;bkeyrun=1;}} //启动
                 else {brekey=0;bkeyrunstop=0;bkeystop=1;} //停止
             }
             if(oldoutkeyval!=newoutkeyval && newoutkeyval>0){
                 if(newoutkeyval==0x04){bkeyrst=1;}//清零
             }
        }
        else{
	    if(oldoutkeyval!=newoutkeyval && newoutkeyval>0){
                      if(newoutkeyval==0x01){if(bkeyrunstop==0){bkeyrunstop=1; bkeyrun=1;}}  //启动
		 else if(newoutkeyval==0x02){if(bkeyrunstop==1){bkeyrunstop=0; bkeystop=1;}} //停止
		 else if(newoutkeyval==0x04){bkeyrst=1;}//清零
	    }
        }
}
/*----------------------------------------------------------------*/
void comm_prs(void){
    uchar i,j,k;
    uchar address;

    if(bswpcommok==1 && bmodcommok==0 && bcommover==0){// SWP通讯协议

        if(SENDTIME==0){
            CRC=0; for (i=1;i<(iRece-2);i++) {CRC=CRC^Recebuf[i];}// CRC校验计算
	    i=AscToHex2(Recebuf[iRece-2],Recebuf[iRece-1]);// 校验字计算
        }

	if (CRC==i || SENDTIME>0) {// 校验字判断

            if(SENDTIME==0){
                for(i=1;i<5;i++) {Sendbuf[i]=Recebuf[i];}
                CRC=0; for(i=1;i<5;i++){CRC=CRC^Sendbuf[i];}
                address=AscToHex2(Recebuf[1],Recebuf[2]);// 取通讯站地址
            }

	    if(De==address || SENDTIME>0) {// 主机板站地址判断
	        if (Recebuf[3] == 'R') {
                    if (Recebuf[4] == 'D') {// "RD" LSend=i*2+8
                        bcommover=1;     // 缓冲区满
                        ENDSENDTIME=1;   // 发送总页数
                        bcommdisre=1;    // 485发送允许
                        LSend=4;
		        for(i=0;i<DTLENG;i++){setsendval(DtVal[i]);}
                        for(i=5;i<=LSend;i++){CRC=CRC^Sendbuf[i];}
                        LSend++; Sendbuf[LSend]=HexToAsc(CRC>>4);
	                LSend++; Sendbuf[LSend]=HexToAsc(CRC&0x0F);
	                LSend++; Sendbuf[LSend]=0x0D;
                        iRece=0;CON_485=0;
                        SBUF='@';
                        iSend=0;
                        bswpcommok=0;
	            }
		    else if(Recebuf[4] == 'R') { // "RR" LSend=i*2+8
                        bcommover=1;
                        if(SENDTIME==0){// 发送第一页
                            ENDSENDTIME=4;
                            bcommdisre=1;
                            LSend=4;
                            iRece=0;CON_485=0;
			    SBUF='@';
                            iSend=0;
                        }
                        else{
                            LSend=0;
                            if(SENDTIME==ENDSENDTIME-1){ //发送最后一页
                                LSend++; Sendbuf[LSend]=HexToAsc(CRC>>4);
                                LSend++; Sendbuf[LSend]=HexToAsc(CRC&0x0F);
                                LSend++; Sendbuf[LSend]=0x0D;
                                bswpcommok=0;DtVal[0]=0;
                            }
                            else{
                                for(i=0;i<32;i++){setsendval(eeprom_read(32*(SENDTIME-1)+i,1));}
                                for(i=1;i<=LSend;i++){CRC=CRC^Sendbuf[i];}
                            }
                            SBUF=Sendbuf[1];
                            iSend=1;
                        }
		    }
		    else if (Recebuf[4] == 'E') {// "RE"
                        bcommover=1;
                        ENDSENDTIME=1;
                        bcommdisre=1;
                        LSend=4;
                        k=AscToHex2(Recebuf[7],Recebuf[8]);
                        j=AscToHex2(Recebuf[9],Recebuf[10]);
                        if(j>16){j=16;}

			for(i=0;i<j;i++){
                            if(k<0x70){setsendval(eeprom_read(k+i,1));}
                            else{setsendval(0xFF);}
                        }
                        for(i=5;i<=LSend;i++){CRC=CRC^Sendbuf[i];}
                        LSend++; Sendbuf[LSend]=HexToAsc(CRC>>4);
	                LSend++; Sendbuf[LSend]=HexToAsc(CRC&0x0F);
	                LSend++; Sendbuf[LSend]=0x0D;
                        iRece=0;CON_485=0;
                        SBUF='@';
                        iSend=0;
                        bswpcommok=0;
                    }
	        }
		else if (Recebuf[3] == 'W') {// "W1"，"W2","W4"写二级参数
                    k=AscToHex2(Recebuf[7],Recebuf[8]);
		    if(Recebuf[4]=='1'){
                        if(k<0x70){
                            j=AscToHex2(Recebuf[9],Recebuf[10]);
                            wDog(0,k,j);
                            refram(k);
                        }
                    }
                    else if(Recebuf[4]=='2'){
                         if(k<0x70){
                             j=AscToHex2(Recebuf[9],Recebuf[10]);
                             wDog(0,k,j);k++;
                             j=AscToHex2(Recebuf[11],Recebuf[12]);
                             wDog(0,k,j);
                             refram(k-1);
                         }
                    }
                    else if(Recebuf[4]=='4'){
                         if(k<0x70){
                             j=AscToHex2(Recebuf[9],Recebuf[10]);
                             wDog(0,k,j);k++;
                             j=AscToHex2(Recebuf[11],Recebuf[12]);
                             wDog(0,k,j);k++;
                             j=AscToHex2(Recebuf[13],Recebuf[14]);
                             wDog(0,k,j);k++;
                             j=AscToHex2(Recebuf[15],Recebuf[16]);
                             wDog(0,k,j);
                             refram(k-3);
                         }
                    }
                    if(Recebuf[4]=='1'||Recebuf[4]=='2'||Recebuf[4]=='4'){
                        bcommover=1;
                        ENDSENDTIME=1;
                        bcommdisre=1;
                        LSend=5; Sendbuf[LSend]=HexToAsc(CRC>>4);
	                LSend++; Sendbuf[LSend]=HexToAsc(CRC&0x0F);
	                LSend++; Sendbuf[LSend]=0x0D;
                        iRece=0;CON_485=0;
                        SBUF='@';
                        iSend=0;
		    }
                    bswpcommok=0;
		}
                if (Recebuf[3] == 'J') {
                    if (Recebuf[4] == 'D') {// "JD" LSend=i*2+8
                        bcommover=1;
                        ENDSENDTIME=1;
                        bcommdisre=1;
                        LSend=4;
		        for(i=0;i<DTLENG;i++){setsendval(DtVal[i]);}
                        for(i=5;i<=LSend;i++){CRC=CRC^Sendbuf[i];}
                        LSend++; Sendbuf[LSend]=HexToAsc(CRC>>4);
	                LSend++; Sendbuf[LSend]=HexToAsc(CRC&0x0F);
	                LSend++; Sendbuf[LSend]=0x0D;
                        iRece=0;CON_485=0;
                        SBUF='@';
                        iSend=0;
                        bswpcommok=0;
	            }
		    else if(Recebuf[4] == 'R') { // "JR" LSend=i*2+8
                        bcommover=1;
                        if(SENDTIME==0){// 发送第一页
                            ENDSENDTIME=10;
                            bcommdisre=1;
                            LSend=4;
                            iRece=0;CON_485=0;
			    SBUF='@';
                            iSend=0;
                        }
                        else{
                            LSend=0;
                            if(SENDTIME==ENDSENDTIME-1){ //发送最后一页
                                LSend++; Sendbuf[LSend]=HexToAsc(CRC>>4);
                                LSend++; Sendbuf[LSend]=HexToAsc(CRC&0x0F);
                                LSend++; Sendbuf[LSend]=0x0D;
                                bswpcommok=0;DtVal[0]=0;
                            }
                            else{
                                for(i=0;i<32;i++){setsendval(eeprom_read(32*(SENDTIME-1)+i,1));}
                                for(i=1;i<=LSend;i++){CRC=CRC^Sendbuf[i];}
                            }
                            SBUF=Sendbuf[1];
                            iSend=1;
                        }
		    }
		    else if (Recebuf[4] == 'E') {// "JE"
                        bcommover=1;
                        ENDSENDTIME=1;
                        bcommdisre=1;
                        LSend=4;
                        k=AscToHex2(Recebuf[7],Recebuf[8]);
                        j=AscToHex2(Recebuf[9],Recebuf[10]);
                        if(j>16){j=16;}

			for(i=0;i<j;i++){setsendval(eeprom_read(k+i,1));}
                        for(i=5;i<=LSend;i++){CRC=CRC^Sendbuf[i];}
                        LSend++; Sendbuf[LSend]=HexToAsc(CRC>>4);
	                LSend++; Sendbuf[LSend]=HexToAsc(CRC&0x0F);
	                LSend++; Sendbuf[LSend]=0x0D;
                        iRece=0;CON_485=0;
                        SBUF='@';
                        iSend=0;
                        bswpcommok=0;
                    }
                    else{
                        k=AscToHex2(Recebuf[7],Recebuf[8]);
		        if(Recebuf[4]=='1'){
                            j=AscToHex2(Recebuf[9],Recebuf[10]);
                            wDog(0,k,j);
                            refram(k);
                        }
                        else if(Recebuf[4]=='2'){
                            j=AscToHex2(Recebuf[9],Recebuf[10]);
                            wDog(0,k,j);k++;
                            j=AscToHex2(Recebuf[11],Recebuf[12]);
                            wDog(0,k,j);
                            refram(k-1);
                        }
                        else if(Recebuf[4]=='4'){
                            j=AscToHex2(Recebuf[9],Recebuf[10]);
                            wDog(0,k,j);k++;
                            j=AscToHex2(Recebuf[11],Recebuf[12]);
                            wDog(0,k,j);k++;
                            j=AscToHex2(Recebuf[13],Recebuf[14]);
                            wDog(0,k,j);k++;
                            j=AscToHex2(Recebuf[15],Recebuf[16]);
                            wDog(0,k,j);
                            refram(k-3);
                        }
                        else if(Recebuf[4]=='B'){
                             if(k<=1){
                                 b_TJ=1;
                                 BsTjval[k]=AscToHex2(Recebuf[11],Recebuf[12]);
                                 BsTjval[k]=BsTjval[k]*256+AscToHex2(Recebuf[9],Recebuf[10]);
                             }
                        }
                        if(Recebuf[4]=='1'||Recebuf[4]=='2'||Recebuf[4]=='4'||Recebuf[4]=='B'){
                            bcommover=1;
                            ENDSENDTIME=1;
                            bcommdisre=1;
                            LSend=5; Sendbuf[LSend]=HexToAsc(CRC>>4);
	                    LSend++; Sendbuf[LSend]=HexToAsc(CRC&0x0F);
	                    LSend++; Sendbuf[LSend]=0x0D;
                            iRece=0;CON_485=0;
                            SBUF='@';
                            iSend=0;
		        }
                        bswpcommok=0;
                    }
                }
            }
            //if(SENDTIME==0){iRece=0;CON_485=0;}// 发送期间，禁止串行口接收
        }
        else{bswpcommok=0; SENDTIME=0; CON_485=1;}

     }
     else if(bswpcommok==0 && bmodcommok==1 && bcommover==0){//MODBUS通讯协议
        CRC2=0XFFFF; Sendbuf[0]=0; LSend=0;
	if (Recebuf[0]==De) {// 地址判断
		Sendbuf[0]=Recebuf[0]; Sendbuf[1]=Recebuf[1];
		for (i=0;i<iRece;i++) {Mcrc(Recebuf[i]);} //CRC校验
/*		if(CRC2==0) {       // 校验正确
			if(Recebuf[1]==0x03) {   // 读取保存寄存器
                            if(Recebuf[3]<0x10){
                                address=Recebuf[5]+Recebuf[3]-1;
			        if(address<0x10) {
			            Sendbuf[2] =Recebuf[5]*2; LSend=Sendbuf[2]+4;
                                    for(j=Recebuf[3];j<=address;j++){
					     if(j==0x00) {Sendbuf[3] =0x00;     Sendbuf[4] =0x00;}
                                        else if(j==0x01) {Sendbuf[5] =0x00;     Sendbuf[6] =DtVal[0];}
				        else if(j==0x02) {Sendbuf[7] =0x00;     Sendbuf[8] =DtVal[1];}
                                        else if(j==0x03) {Sendbuf[9] =0x00;     Sendbuf[10]=De;}
                                        else if(j==0x04) {Sendbuf[11]=DtVal[2]; Sendbuf[12]=DtVal[3];}
                                        else if(j==0x05) {Sendbuf[13]=DtVal[4]; Sendbuf[14]=DtVal[5];}
                                        else if(j==0x06) {Sendbuf[15]=DtVal[6]; Sendbuf[16]=DtVal[7];}
                                        else if(j==0x07) {Sendbuf[17]=DtVal[8]; Sendbuf[18]=DtVal[9];}
                                        else if(j==0x08) {Sendbuf[19]=0x00;     Sendbuf[20]=DtVal[10];}
                                        else if(j==0x09) {Sendbuf[21]=0x00;     Sendbuf[22]=DtVal[11];}
                                        else if(j==0x0A) {Sendbuf[23]=0x00;     Sendbuf[24]=0x00;}
                                        else if(j==0x0B) {Sendbuf[25]=0x00;     Sendbuf[26]=0x00;}
                                        else if(j==0x0C) {Sendbuf[27]=0x00;     Sendbuf[28]=0x00;}
                                        else if(j==0x0D) {Sendbuf[29]=0x00;     Sendbuf[30]=0x00;}
                                        else if(j==0x0E) {Sendbuf[31]=0x00;     Sendbuf[32]=0x00;}
                                        else if(j==0x0F) {Sendbuf[33]=0x00;     Sendbuf[34]=0x00;}
                                    }
			        }
                                else{Sendbuf[1]+=0x80; Sendbuf[2]=0x03; LSend=4;}
                            }
                            else{Sendbuf[1]+=0x80; Sendbuf[2]=0x02; LSend=4;}
			}
			else if(Recebuf[1]==0x04) {Sendbuf[1]+=0x80; Sendbuf[2]=0x99; LSend=4;} // 读取通用参数
			else if(Recebuf[1]==0x06) {Sendbuf[1]+=0x80; Sendbuf[2]=0x99; LSend=4;} // 写取通用参数
			else if(Recebuf[1]==0x10) {Sendbuf[1]+=0x80; Sendbuf[2]=0x99; LSend=4;} // 写取通用参数
			else {Sendbuf[1]+=0x80; Sendbuf[2]=0x01; LSend=4;}// 送无相应命令符出错标记
		}
		else {// 地址正确但校验出错，送CRC出错应答
			Sendbuf[1]+=0x80; Sendbuf[2]=0x04; LSend=4;
		}
                */
                if(CRC2==0) {       // 校验正确
                	if(Recebuf[1]==0x03) {// 读参数寄存器
                            if(Recebuf[4]==0 && Recebuf[5]>=1 && Recebuf[5]<=0X7D){ // 1<=长度<=125
                                if(Recebuf[2]<0x40){// 读动态参数寄存器
                                    address=Recebuf[5]+Recebuf[3]-1;
			            if(Recebuf[2]==0 && Recebuf[3]<0x10 && address<=0x10) {
			                Sendbuf[2] =Recebuf[5]*2; LSend=Sendbuf[2]+4;
                                        for(i=0;i<=Recebuf[5];i++){
                                            j=Recebuf[3]+i;
                                            Sendbuf[i*2+3] = DtVal[MBDTAddr[j][0]];
                                            Sendbuf[i*2+4] = DtVal[MBDTAddr[j][1]];
                                        }
                                    }
                                    else{Sendbuf[1]+=0x80; Sendbuf[2]=0x03; LSend=4;}// 起始地址、长度出错
                                }
                                else{// 读二级参数寄存器 未完成
                                    Sendbuf[1]+=0x80; Sendbuf[2]=0x03; LSend=4; // 起始地址、长度出错
                                }
                            }
                            else{Sendbuf[1]+=0x80; Sendbuf[2]=0x02; LSend=4;}
			}
                        /*
			else if(Recebuf[1]==0x06) {// 写单个二级参数寄存器
                            if(Recebuf[2]==0x40 && Recebuf[3]<=0x40){
                                if(MBDogAddr[Recebuf[3]][0]!=ADRNULL){
                                    wDog(0,MBDogAddr[Recebuf[3]][0],Recebuf[4]);
                                }
                                wDog(0,MBDogAddr[Recebuf[3]][1],Recebuf[5]);

                                Sendbuf[2]=Recebuf[2]; Sendbuf[3]=Recebuf[3];
                                Sendbuf[4]=Recebuf[4]; Sendbuf[5]=Recebuf[5];
                                LSend=7;
                            }
                            else{Sendbuf[1]+=0x80; Sendbuf[2]=0x02; LSend=4;}
                        }
                        */
			else {Sendbuf[1]+=0x80; Sendbuf[2]=0x01; LSend=4;}// 无效命令符出错标记
		}
		else {Sendbuf[1]+=0x80; Sendbuf[2]=0x04; LSend=4;}// 地址正确但校验出错，送CRC出错应答
	}
	if (LSend>2) {
		// CRC计算
		CRC2=0XFFFF;
		for(i=0;i<LSend-1;i++) {Mcrc(Sendbuf[i]);}
		Sendbuf[LSend-1]=CRC2%256;Sendbuf[LSend]=CRC2/256;
		CON_485=0;  //发送期间，禁止串行口接收
		bcommover=1;
                ENDSENDTIME=1;
                bcommdisre=1;
                SBUF=Sendbuf[0];
                iSend=0;
	}
        iRece=0;
	bmodcommok=0;
     }
}

void Mcrc (unsigned int jj) {// Modbus 校验字计算
	unsigned char v;
	unsigned char i;
	CRC2=CRC2 ^ jj;
	for(i=0;i<8;i++) {
		v=CRC2 %2;	CRC2=CRC2>>1;
		if(v==1) {CRC2=CRC2 ^ 0xa001;}
	}
}




void setsendval(val) {// 发送缓冲区值设置
    LSend++; Sendbuf[LSend]=HexToAsc(val>>4);
    LSend++; Sendbuf[LSend]=HexToAsc(val&0x0f);
}
/*-------------------------------------------------------------------------*/
void Bsout(int v) {  // 变送1数据移位输出
     uint i;
     if(v>4095){v=4095;}else if(v<0){v=0;}
     CS_BS=0;
     for (i=0x8000;i>0;i/=2) {IIC_SIO=v & i;IIC_CLK=1;nop(5);IIC_CLK=0;nop(50);}
     CS_BS=1;
}

void bs_prs(void){
     float   iliangcheng;
     float   foutl;
     float   ftemp;
     int     ival;

     if(b_TJ==0){
         ftemp=eeprom_read(ADRKK,4);
         if(ftemp==0){ftemp=sum_pulse_count/0.000001+eeprom_read(ADRPB,4);}
         else{ftemp=sum_pulse_count/ftemp+eeprom_read(ADRPB,4);}

         foutl=eeprom_read(ADROUTL,4);
         iliangcheng=eeprom_read(ADROUTH,4)-foutl;
         ftemp-=foutl;

         if(ftemp<0){ftemp=0;}
         else if(ftemp>iliangcheng){ftemp=1;}
         else{ftemp/=iliangcheng;}

         ftemp*=4095;
         ival=eeprom_read(ADROUPB,2);
         ftemp=(ftemp*(1000-ival)+4095.0*ival)/1000;
         ival=ftemp*eeprom_read(ADROUKK,2)/1000;

         //ftemp = eeprom_read(0X72,2);
         ftemp=14487;
         if (ftemp<0) {ftemp=32767-ftemp;}  // 读调校比例
         ftemp = ftemp * ival/ 16384;       // 采样脉冲*调校比例
         //ival= ftemp + eeprom_read(0X70,2); // 采样脉冲*调校比例+调校零点
         ftemp += 188; // 采样脉冲*调校比例+调校零点
         ival=ftemp*3600/4095;
         Bsout(ival);
     }
     else{
         ftemp=BsTjval[0]; ival=ftemp*3600/4095; Bsout(ival);
     }

}

void run_prs(void){
     bal6=1;
     if(bnumfull==0){
         if(DogVal[ADRCT]==1 && DogVal[ADRTUNI]>1){TR0=0; b_run_stop=1; runtime();}
         else if(DogVal[ADRCT]==2){TR0=0; b_run_stop=1;}
         else{TR0=1; b_run_stop=0;}
     }
}

void stop_prs(void){
     bal6=0;
     TR0=0;
     b_run_stop=0;
}

void wpa(void);
void wpa(void)
{
      if((PAval&0X01)==0X01){P_A0=1;}else{P_A0=0;}
      if((PAval&0X02)==0X02){P_A1=1;}else{P_A1=0;}
      if((PAval&0X04)==0X04){P_A2=1;}else{P_A2=0;}
      if((PAval&0X08)==0X08){P_A3=1;}else{P_A3=0;}
      if((PAval&0X10)==0X10){P_A4=1;}else{P_A4=0;}
}

