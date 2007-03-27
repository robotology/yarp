/* ビットとチャンネルの対応 */
#define CH0 1
#define CH1 2
#define CH2 4
#define CH3 8

/* デジタル入力用のマスク */
#define DIN0 0x10
#define DIN1 0x20

/* デジタル出力用のマスク */
#define DO_DOUT 0x1
#define DOUT0 0x08
#define DOUT1 0x20
#define DOUT2 0x40
#define DOUT3 0x80

struct uin {
  unsigned short time; /* 内部カウンタ (1ms周期)の値 */
  unsigned short ad[4];/* A/D コンバータの値(10bit) */
  short ct[4];         /* 2相カウンタの値(16bit) */
  unsigned short da[4];/* D/A 出力の値(上位 8/12bitが有効) */
  unsigned char din;   /* デジタル入力 */
  unsigned char dout;  /* デジタル出力 */
  unsigned short intmax;  /* ソフトウェアカウンタの最大周期 */
  unsigned short interval;/* ソフトウェアカウンタの最近の周期 */
  unsigned short magicno; /* EEPROM 内の数字 */
  char dmy[30];
};

struct scmd {
  short x;             /* 目標位置 */
  short d;             /* 目標速度 */
  signed short kp;     /* 位置誤差ゲイン(分子) */
  unsigned short kpx;  /* 位置誤差ゲイン(分母) */
  signed short kd;     /* 速度誤差ゲイン(分子) */
  unsigned short kdx;  /* 速度誤差ゲイン(分母) */
  signed short ki;     /* 積分誤差ゲイン(分子) */
  unsigned short kix;  /* 積分誤差ゲイン(分母) */
};

struct uout {
  struct scmd ch[4];
};

#define RETURN_VAL 1
#define SET_SELECT 0x80
#define SET_POSNEG 0x80
#define SET_BREAKS 0x80
#define SET_CH2_HIN 0x40
#define WR_MAGIC 0x80
#define WR_SELOUT 0x40
#define WR_OFFSET 0x20
#define WR_BREAKS 0x10

struct ccmd {
  unsigned char retval;    /* 次に内部カウンタが上がったときの値を EP1 から返す */
  unsigned char setoffset; /* オフセットをセットするチャンネルの指定 */
  unsigned char setcounter;/* カウンタの値をセットするチャンネルの指定 */
  unsigned char resetint;  /* 積分値をリセットするチャンネルの指定 */
  unsigned char selin;     /* counter(0)/ADC(1) の選択 */
  unsigned char dout;      /* デジタル出力 */
  unsigned short offset[4];         /* オフセットの値 */
  short counter[4];        /* カウンタの値 */
  unsigned char selout;    /* PWM を D/A 向け(0)とするか
			      H ブリッジ向け(1)とするか */
  unsigned char wrrom;
  unsigned short magicno;
  unsigned char posneg;    /* PWM パルスの正/負 */
  unsigned char breaks;    /* break 出力 */
  char dummy[36];
};
