#ifndef PCI_8132_H
    #define PCI_8132_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/*      Typedef  Definitions                                                */
/****************************************************************************/
typedef unsigned char   U8;
typedef short           I16;
typedef unsigned short  U16;
typedef long            I32;
typedef unsigned long   U32;
typedef float           F32;
typedef double          F64;
typedef char            Boolean;

#define     MAX_PCI_CARDS       12

#define     TRUE                1
#define     FALSE               0

#define     HIGH                1
#define     LOW                 0

typedef struct
{
        U16 baseAddr[MAX_PCI_CARDS];
	U16 lcrAddr[MAX_PCI_CARDS];
	U16 irqNo   [MAX_PCI_CARDS];
} PCI_INFO;

/* ------------ I/O Status Definition ------------------------------------- */
#define         pos_limit       0x01
#define         neg_limit       0x02
#define         psd_switch      0x04
#define         nsd_switch      0x08
#define         org_limit       0x10
#define         idx_switch      0x20
#define         alm_switch      0x40
#define         svon_sts        0x80
#define         rdy_sts         0x100
#define         int_sts         0x200
#define         erc_sts         0x400
#define         inp_sts         0x800

/* ------------ Motion Done Reurn Value Definition ------------------------ */
#define         BUSY            0x00
#define         DONE            0x01
#define         POS_LIM         0x02
#define         NEG_LIM         0x03
#define         ORG_LIM         0x04
#define         ALARM           0x05

/****************************************************************************/
/*      Function  Declerations                                              */
/****************************************************************************/

U16   _8132_Initial(U16 *existCards, PCI_INFO *info);

U16   _8132_Set_SVON(int axis, int on_off);

void  _8132_Get_IRQ_Status (U16 cardNo, U16 *ch1, U16 *ch2 );
void  _8132_Get_IRQ_Channel(U16 cardNo, U16 *irq_no );
void  _8132_Get_Base_Addr(U16 cardNo, U16 *base_addr1, U16 *base_addr2 );
void  _8132_Set_INT_Control(U16 cardNo, U16 int1Flag, U16 int2Flag );

U16  _8132_Set_Config(char *fileName);

U16  _8132_start_a_move(I16 axis, F64 pos, F64 str_vel, F64 max_vel, F64 accel);
U16  _8132_a_move(I16 axis, F64 pos, F64 str_vel, F64 max_vel, F64 accel);
U16  _8132_start_r_move(I16 axis, F64 distance, F64 str_vel, F64 max_vel, F64 accel);
U16  _8132_r_move(I16 axis, F64 distance, F64 str_vel, F64 max_vel, F64 accel);
U16  _8132_start_t_move(I16 axis, F64 distance, F64 str_vel, F64 max_vel, F64 accel, F64 decel);
U16  _8132_t_move(I16 axis, F64 distance, F64 str_vel, F64 max_vel, F64 accel, F64 decel);
U16  _8132_wait_for_done(I16 axis);

U16  _8132_set_move_ratio(I16 axis, F64 ratio);
U16  _8132_get_position(I16 axis, F64 *pos);
U16  _8132_set_position(I16 axis, F64 pos);
U16  _8132_get_command(I16 axis, F64 *pos);
U16  _8132_set_command(I16 axis, F64 pos);
U16  _8132_v_move(I16 axis, F64 str_vel, F64 max_vel, F64 accel);
U16  _8132_sv_move(I16 axis, F64 str_vel, F64 max_vel, F64 Tlacc, F64 Tsacc);
U16  _8132_v_change(I16 axis, F64 max_vel, F64 accel);
U16  _8132_v_stop(I16 axis, F64 decel);
U16  _8132_fix_max_speed(I16 axis, F64 max_vel);

U16  _8132_get_io_status(I16 axis, U16 *io_sts);

U16  _8132_motion_done(I16 axis);

U16  _8132_map_axes(I16 n_axes, I16 *map_array);
U16  _8132_set_move_mode(int mode);
U16  _8132_set_move_pos(double pos);
U16  _8132_set_move_speed(double str_vel, double max_vel);
U16  _8132_set_move_accel(double accel);
U16  _8132_set_move_saccel(double tlacc, double tsacc);
U16  _8132_start_motion(void);
U16  _8132_stop_motion(void);
U16  _8132_set_sync_mode(I16 ext_trig, I16 trig_mode);
U16  _8132_set_arc_division(I16 axis, F64 degrees);
U16  _8132_arc_optimization(int optimize);

U16  _8132_move_xy(int cardNo, double x, double y);
U16  _8132_start_move_xy(int cardNo, double x, double y);
U16  _8132_recover_xy(int cardNo);

U16  _8132_arc_xy(int cardNo, double x_center, double y_center, double angle);

U16  _8132_set_home_config(int axis,int home_mode,int org_logic,int org_latch,int EZ_logic);
U16  _8132_home_move(int axis,double str_vel,double max_vel,double accel);

U16  _8132_set_manu_iptmode(int axis, int manu_iptmode, int op_mode);
U16  _8132_set_manu_axis(I16 cardno, I16 manu_axis );
U16  _8132_manu_move(int axis, double max_vel);
U16  _8132_set_step_unit(I16 axisno, I16 unit);

U16  _8132_set_pls_outmode(int axis, int pls_outmode);
U16  _8132_set_pls_iptmode(int axis, int pls_iptmode);
U16  _8132_set_cnt_src(int axis, int cnt_src);

U16  _8132_set_alm_logic(int axis, int alm_logic, int alm_mode);
U16  _8132_set_inp_logic(int axis, int inp_logic, int inp_enable);
U16  _8132_set_erc_enable(int axis, int erc_enable);
U16  _8132_set_sd_logic(int axis, int sd_logic, int sd_latch, int sd_enable);   // Ver 2.0, Stephen
U16  _8132_set_sd_stop_mode(I16 axisno, I16 stop_mode);

U16  _8132_set_int_factor(U16 axis, U32 int_factor );
U16  _8132_get_int_status(int axis, U32 *int_status );

U16  _8132_start_ta_move(I16 axis, F64 pos, F64 str_vel, F64 max_vel, F64 Tacc, F64 Tdec);
U16  _8132_ta_move(I16 axis, F64 pos, F64 str_vel, F64 max_vel, F64 Tacc, F64 Tdec);
U16  _8132_start_s_move(I16 axis, F64 pos, F64 str_vel, F64 max_vel, F64 Tlacc, F64 Tsacc);
U16  _8132_s_move(I16 axis, F64 pos, F64 str_vel, F64 max_vel, F64 Tlacc, F64 Tsacc);
U16  _8132_start_rs_move(I16 axis, F64 distance, F64 str_vel, F64 max_vel, F64 Tlacc, F64 Tsacc);
U16  _8132_rs_move(I16 axis, F64 distance, F64 str_vel, F64 max_vel, F64 Tlacc, F64 Tsacc);
U16  _8132_start_tas_move(I16 axis, F64 pos, F64 str_vel, F64 max_vel, F64 Tlacc, F64 Tsacc, F64 Tldec, F64 Tsdec);
U16  _8132_tas_move(I16 axis, F64 pos, F64 str_vel, F64 max_vel, F64 Tlacc, F64 Tsacc, F64 Tldec, F64 Tsdec);
U16  _8132_start_move_all(I16 len, I16 *map_array, F64 *pos, F64 *str_vel, F64 *max_vel, F64 *Tacc);
U16  _8132_move_all(I16 len, I16 *map_array, F64 *pos, F64 *str_vel, F64 *max_vel, F64 *Tacc);
U16  _8132_wait_for_all(I16 len, I16 *map_array);

U16   _8132_DO(U16 cardNo, U16 doData);
U16   _8132_DI(U16 cardNo, U16 *diData);
void  _8132_Close(U16 cardNo);

/****************************************************************************/
/*      Comparator Function Declare                                         */
/****************************************************************************/
U16    _8132_Get_CompCnt(U16 axis, double *act_pos);
U16    _8132_Set_CompCnt(U16 axis, double cnt_value);
U16    _8132_Set_CompMode(U16 axis, I16 comp_mode);
U16    _8132_Set_CompData(U16 axis, double comp_data);
U16    _8132_Get_CompData(U16 axis, double *comp_data);
U16    _8132_Set_CompInt(U16 axis, U16 enable);
U16    _8132_Set_CompHome(U16 axis);
U16    _8132_Get_CompSts(U16 cardNo, U16 *Comp_Sts);
U16    _8132_Build_Comp_Table(U16 axis, I32 *table, I16 Size);
U16    _8132_Build_Comp_Function(U16 axis, I32 Start, I32 End, I32 Interval);
U16    _8132_Set_Comp_Table(U16 axis, U16 Control);

#ifdef __cplusplus
}
#endif

#endif
