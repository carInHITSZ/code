#define KEY1                    (P21_7)
#define KEY2                    (P20_8)
#define KEY3                    (P20_7)
#define KEY4                    (P20_6)
#define SWITCH1 (P10_3)
#define SWITCH2 (P10_1)

void mode_ts (void);
void state_turn_ts (void);
void TLD_ts (uint8 ts_state);

extern uint8 show_ips_mode;
extern uint8 key_test_mode;
extern uint8 end_dot_show;
extern uint8 start_dot_show;
extern float dir_ts_limit;
extern float special_dir_ts;
extern float special_type;

