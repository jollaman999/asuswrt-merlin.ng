#ifndef _MODEL_H__
#define _MODEL_H__

#define MODEL_UNUSED(x)	__MDL_UNUSED(x)
#define __MDL_UNUSED(s)	MODEL_unused ## s

/* NOTE: Do not insert new entries in the middle of this enum,
 * always add them to the end! */

enum {
	MODEL_GENERIC = -1
};

#define ASUS_MODELS {   \
	MODELID(MODEL_UNKNOWN),\
	MODELID(MODEL_DSLN55U),\
	MODELID(MODEL_DSLAC68U),\
	MODELID(MODEL_EAN66),\
	MODELID(MODEL_RTN11P),\
	MODELID(MODEL_RTN300),\
	MODELID(MODEL_RTN13U),\
	MODELID(MODEL_RTN14U),\
	MODELID(MODEL_RTAC52U),\
	MODELID(MODEL_RTAC51U),\
	MODELID(MODEL_RTN54U),\
	MODELID(MODEL_RTAC54U),\
	MODELID(MODEL_RTN56UB1),\
	MODELID(MODEL_RTN56UB2),\
	MODELID(MODEL_RTAC1200HP),\
	MODELID(MODEL_RTAC55U),\
	MODELID(MODEL_RTAC55UHP),\
	MODELID(MODEL_RT4GAC55U),\
	MODELID(MODEL_RTAC59U),\
	MODELID(MODEL_PLN12),\
	MODELID(MODEL_PLAC56),\
	MODELID(MODEL_PLAC66U),\
	MODELID(MODEL_RTAC58U),\
	MODELID(MODEL_RT4GAC53U),\
	MODELID(MODEL_RTAC82U),\
	MODELID(MODEL_MAPAC1300),\
	MODELID(MODEL_MAPAC2200),\
	MODELID(MODEL_VZWAC1300),\
	MODELID(MODEL_MAPAC1750),\
	MODELID(MODEL_RTAC95U),\
	MODELID(MODEL_MAPAC2200V),\
	MODELID(MODEL_RTN36U3),\
	MODELID(MODEL_RTN56U),\
	MODELID(MODEL_RTN65U),\
	MODELID(MODEL_RTN67U),\
	MODELID(MODEL_RTN12),\
	MODELID(MODEL_RTN12B1),\
	MODELID(MODEL_RTN12C1),\
	MODELID(MODEL_RTN12D1),\
	MODELID(MODEL_RTN12VP),\
	MODELID(MODEL_RTN12HP),\
	MODELID(MODEL_RTN12HP_B1),\
	MODELID(MODEL_APN12),\
	MODELID(MODEL_APN12HP),\
	MODELID(MODEL_RTN16),\
	MODELID(MODEL_RTN18U),\
	MODELID(MODEL_RTN15U),\
	MODELID(MODEL_RTN53),\
	MODELID(MODEL_RTN66U),\
	MODELID(MODEL_RTAC66U),\
	MODELID(MODEL_RTAC68U),\
	MODELID(MODEL_UNUSED(__LINE__)),\
	MODELID(MODEL_RTAC87U),\
	MODELID(MODEL_RTAC56S),\
	MODELID(MODEL_RTAC56U),\
	MODELID(MODEL_RTAC53U),\
	MODELID(MODEL_RTAC3200),\
	MODELID(MODEL_RTAC88U),\
	MODELID(MODEL_RTAC3100),\
	MODELID(MODEL_RTAC5300),\
	MODELID(MODEL_GTAC5300),\
	MODELID(MODEL_RTN14UHP),\
	MODELID(MODEL_RTN10U),\
	MODELID(MODEL_RTN10P),\
	MODELID(MODEL_RTN10D1),\
	MODELID(MODEL_RTN10PV2),\
	MODELID(MODEL_RTAC1200),\
	MODELID(MODEL_RTAC1200G),\
	MODELID(MODEL_RTAC1200GP),\
	MODELID(MODEL_RTAC1200GA1),\
	MODELID(MODEL_RTAC1200GU),\
	MODELID(MODEL_RPAC66),\
	MODELID(MODEL_RPAC51),\
	MODELID(MODEL_RTAC51UP),\
	MODELID(MODEL_RTAC53),\
	MODELID(MODEL_RTN11P_B1),\
	MODELID(MODEL_RPAC87),\
	MODELID(MODEL_RTAC85U),\
	MODELID(MODEL_RTAC85P),\
	MODELID(MODEL_RTACRH26),\
	MODELID(MODEL_RTN800HP),\
	MODELID(MODEL_RTAC88N),\
	MODELID(MODEL_BRTAC828),\
	MODELID(MODEL_RTAC88S),\
	MODELID(MODEL_RPAC53),\
	MODELID(MODEL_RPAC68U),\
	MODELID(MODEL_RPAC55),\
	MODELID(MODEL_RTAC86U),\
	MODELID(MODEL_GTAC9600),\
	MODELID(MODEL_BLUECAVE),\
	MODELID(MODEL_RTAD7200),\
	MODELID(MODEL_GTAXY16000),\
	MODELID(MODEL_GTAX6000N),\
	MODELID(MODEL_RTAX89U),\
	MODELID(MODEL_RTAC1200V2),\
	MODELID(MODEL_RTN19),\
	MODELID(MODEL_TUFAC1750),\
	MODELID(MODEL_RTAX88U),\
	MODELID(MODEL_GTAX11000),\
	MODELID(MODEL_RTAX92U),\
	MODELID(MODEL_RTAX95Q),\
	MODELID(MODEL_RTAXE95Q),\
	MODELID(MODEL_RTAX56_XD4),\
	MODELID(MODEL_RTAX58U),\
	MODELID(MODEL_RTAX56U),\
	MODELID(MODEL_RPAX56),\
	MODELID(MODEL_SHAC1300),\
	MODELID(MODEL_RPAC92),\
	MODELID(MODEL_RTAC59CD6R),\
	MODELID(MODEL_RTAC59CD6N),\
	MODELID(MODEL_RTAX86U),\
	MODELID(MODEL_RTAX68U),\
	MODELID(MODEL_RT4GAC56),\
	MODELID(MODEL_DSLAX82U),\
	MODELID(MODEL_RTAX55),\
	MODELID(MODEL_GTAXE11000),\
	MODELID(MODEL_RTACRH18),\
	MODELID(MODEL_PLAX56XP4),\
	MODELID(MODEL_CTAX56_XD4),\
	MODELID(MODEL_RTAC68U_V4),\
	MODELID(MODEL_GTAX11000_PRO),\
	MODELID(MODEL_ET12),\
	MODELID(MODEL_XT12),\
	MODELID(MODEL_RTAX86U_PRO),\
	MODELID(MODEL_ETJ),\
	MODELID(MODEL_RT4GAC86U),\
	MODELID(MODEL_RT4GAX56),\
	MODELID(MODEL_RTAX53U),\
	MODELID(MODEL_RTAX58U_V2),\
	MODELID(MODEL_GTAX6000),\
	MODELID(MODEL_XD4PRO),\
        MODELID(MODEL_XC5),\
	MODELID(MODEL_EBA63),\
	MODELID(MODEL_XT8PRO),\
	MODELID(MODEL_TUFAX3000_V2),\
	MODELID(MODEL_RTAX82_XD6S),\
	MODELID(MODEL_RPAX58),\
	MODELID(MODEL_RPBE58),\
	MODELID(MODEL_ET8PRO),\
	MODELID(MODEL_GTAXE16000),\
	MODELID(MODEL_RTAXE7800),\
	MODELID(MODEL_RTAX57Q),\
	MODELID(MODEL_XT8_V2),\
	MODELID(MODEL_RTAX54),\
	MODELID(MODEL_XD4S),\
	MODELID(MODEL_GT10),\
	MODELID(MODEL_RTAX3000N),\
	MODELID(MODEL_RTAX82U_V2),\
	MODELID(MODEL_PANTHERA),\
	MODELID(MODEL_PANTHERB),\
	MODELID(MODEL_CHEETAH),\
	MODELID(MODEL_TUFAX4200),\
	MODELID(MODEL_TUFAX5400_V2),\
	MODELID(MODEL_BM68),\
	MODELID(MODEL_RTAX88U_PRO),\
	MODELID(MODEL_ET8_V2),\
	MODELID(MODEL_TUFAX6000),\
	MODELID(MODEL_XD6_V2),\
	MODELID(MODEL_RTAX59U),\
	MODELID(MODEL_RTAX5400),\
	MODELID(MODEL_BR63),\
	MODELID(MODEL_BC109),\
	MODELID(MODEL_EBG19),\
	MODELID(MODEL_EBG15),\
	MODELID(MODEL_EBP15),\
	MODELID(MODEL_BC105),\
	MODELID(MODEL_GTBE98),\
	MODELID(MODEL_RTBE96U),\
	MODELID(MODEL_GTBE98_PRO),\
	MODELID(MODEL_BT12),\
	MODELID(MODEL_BQ16),\
	MODELID(MODEL_PRTAX57GO),\
	MODELID(MODEL_RTAX52),\
	MODELID(MODEL_RTAX9000),\
	MODELID(MODEL_GTBE96),\
	MODELID(MODEL_BQ16_PRO),\
	MODELID(MODEL_RTBE88U),\
	MODELID(MODEL_RTBE86U),\
	MODELID(MODEL_RTBE58U),\
	MODELID(MODEL_GTBE19000),\
	MODELID(MODEL_BT10),\
	MODELID(MODEL_RTBE92U),\
	MODELID(MODEL_RTBE95U),\
	MODELID(MODEL_RTBE82U),\
	MODELID(MODEL_RTBE58U_PRO),\
	MODELID(MODEL_RTBE58_GO),\
	MODELID(MODEL_GTBE19000AI),\
	MODELID(MODEL_RTBE82M),\
	MODELID(MODEL_GSBE18000),\
	MODELID(MODEL_GS7_PRO),\
	MODELID(MODEL_GTBE96_AI),\
	MODELID(MODEL_RTBE58U_V2),\
	MODELID(MODEL_GT7),\
	MODELID(MODEL_GSBE12000),\
	MODELID(MODEL_MAX)\
}

#define MODELID(a)       a
typedef enum ASUS_MODELS asus_model_e;
#undef MODELID

extern char *asus_models_str[];

/* NOTE: Do not insert new entries in the middle of this enum,
 * always add them to the end! */
enum {
	SWITCH_GENERIC = -1,
	SWITCH_UNKNOWN = 0,
	SWITCH_BCM5325,
	SWITCH_BCM53115,
	SWITCH_BCM53125,
	SWITCH_BCM5301x,
	SWITCH_BCM5301x_EX,
	SWITCH_BCM53134,
	SWITCH_RT305x,
	SWITCH_RT3350,
	SWITCH_RT3352,
	SWITCH_RT5350,
	SWITCH_MT7620,
	SWITCH_MT7621,
	SWITCH_MT7628,
	SWITCH_RTL8365MB,
	SWITCH_RTL8367MB,
	SWITCH_RTL8367RB,
	SWITCH_RTL8370M,
};

extern int get_blver(char *bls);
extern int get_fwver(char *buildno, char *extendno);
extern int get_model(void);
extern char *get_modelid(int model);
extern int get_switch(void);

/* define external PHY */
enum {
	EXT_PHY_BCM54991 = 0,
	EXT_PHY_RTL8226  = 1,
	EXT_PHY_GPY211   = 2,
	EXT_PHY_BCM84891L = 3,
	EXT_PHY_BCM50901E = 4,
	EXT_PHY_MAX
};

#endif /* _MODEL_H__ */
