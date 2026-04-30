#include "menu_simp.h"

float **all_var;
char name[NUM_VAR][20];
uint8_t page_num[PAGES + 1];
char page_name[PAGES + 1][10];
menu_opeartion opeartion;
menu_opeartion help_opeartion;

void flash_read (uint8_t region);
void flash_write (uint8_t region);
void all_rec ();
model mod = Menu;
void Keys_update ();
void show_update ();
void ms_op ();
uint8_t page = 0;
int now_t;
float times = 1;
float test;
char op[28] = "Key1:N,Key2:N,Key3:N,Key4:N";
int prev = 0;
uint16 menu_offset = 0;
void menu_simp_init ()
{
    ips200_init(IPS200_TYPE_PARALLEL8);
    ips200_set_color(RGB565_BLACK, RGB565_WHITE);
    ips200_set_font(IPS200_8X16_FONT);
    ips200_full(RGB565_WHITE);
    key_init(10);
    float **all = (float**) malloc(sizeof(float*) * NUM_VAR);
    memset(all, 0, sizeof(float*) * NUM_VAR);
    all_var = all;
    all_rec();
    flash_read(OPERATION_REGION);
}
void menu_simp_update ()
{
    static int cnt = 0;
    if (cnt < 100)
    {
        cnt++;
        return;
    }

    Keys_update();
    ms_op();
    show_update();
}

//kp_servo = 3.0;
//kd_servo = 10.0;
//kp_offset = 0.80;
//kd_offset = 0.5;
//kp_angle = 4.0;
//kp_angle_max = 6.5;
//kp_angle_min = 4.0;
//kd_angle = 16.0;
//kp_palstance = 52.5;
//ki_palstance = 1.57;
//kd_palstance = 0.0;
//angle_range = 30.0;
//

//run_normal = 4000.0;
//run_reduce = 4000.0;
//run_on = 4500.0;
//run_speed = 5000.0;
//run_servo = 6000.0;
//run_angle = 30.0;
//palstance_max = 5800.0;
//palstance_on = 5300.0;
//palstance_speed = 4800.0;
//palstance_servo = 3800.0;
////

//kp_servo_max = 3.0;
//kp_servo_on = 2.0;
//kp_servo_speed = 2.0;
//angle_bias = -0.8;
//mid_std = 39;
//exposure = 60.12;
//TowPoint_1 = 10.1;
//TowPoint_2 = 15.1;
//TowPoint_3 = 15.1;
//TowPoint_std = 15.1;za
//Zebra_break_first = 70.0;
//Zebra_end_distance = 100;
//float Pitch_Ramp = -8;
//float Pitch_Ramp_1 = 4;
//float Pitch_Ramp_2 = 4;
//float Pitch_Ramp_3 = -6;


////
//Repair_X_L = 23.1;
//Repair_X_R = 57.1;
//circle_L_1 = 0;
//circle_L_2 = 0;
//circle_L_3 = 0;
//circle_L_4 = 0;
//k_rlen_l=81;
//circle_R_1 = 0;
//circle_R_2 = 1;
//circle_R_3 = 0;
//circle_R_4 = 0;
//k_rlen_r=81;
//
//special_type = 0.0;
//special_cross = 0.0;
//special_ring = 0.0;
//special_dir_ts=0.0;
//special_S
//dir_ts_limit=8;
////
void all_rec ()
{

    int i = 0, j = 0, k = 0;

    load(all_var, name, run_normal, i, k);
    load(all_var, name, run_reduce, i, k);
    load(all_var, name, run_on, i, k);
    load(all_var, name, run_speed, i, k);
    load(all_var, name, run_servo, i, k);
    load(all_var, name, run_angle, i, k);
    load(all_var, name, palstance_max, i, k);
    load(all_var, name, palstance_on, i, k);
    load(all_var, name, palstance_speed, i, k);
    load(all_var, name, palstance_servo, i, k);
    page_alloc(page_num, page_name, j, k, "run"); //num:10

    load(all_var, name, kp_servo, i, k);
    load(all_var, name, kd_servo, i, k);
    load(all_var, name, kp_offset, i, k);
    load(all_var, name, kd_offset, i, k);
    load(all_var, name, kp_angle, i, k);
    load(all_var, name, kp_angle_max, i, k);
    load(all_var, name, kp_angle_min, i, k);
    load(all_var, name, kd_angle, i, k);
    load(all_var, name, kp_palstance, i, k);
    load(all_var, name, ki_palstance, i, k);
    load(all_var, name, kd_palstance, i, k);
    load(all_var, name, angle_bias, i, k);
    page_alloc(page_num, page_name, j, k, "pid_1"); //num:12

    load(all_var, name, kp_servo_max, i, k);
    load(all_var, name, kp_servo_on, i, k);
    load(all_var, name, kp_servo_speed, i, k);
    load(all_var, name, angle_bias, i, k);
    load(all_var, name, mid_std, i, k);
    load(all_var, name, exposure, i, k);
    load(all_var, name, TowPoint_1, i, k);
    load(all_var, name, TowPoint_2, i, k);
    load(all_var, name, TowPoint_3, i, k);
    load(all_var, name, TowPoint_std, i, k);
    load(all_var, name, Zebra_break_first, i, k);
    load(all_var, name, Zebra_end_distance, i, k);
    load(all_var, name, Pitch_Ramp, i, k);
    page_alloc(page_num, page_name, j, k, "mix"); //num:13

    load(all_var, name, Repair_X_L, i, k);
    load(all_var, name, Repair_X_R, i, k);
    load(all_var, name, circle_L_1, i, k);
    load(all_var, name, circle_L_2, i, k);
    load(all_var, name, circle_L_3, i, k);
    load(all_var, name, circle_L_4, i, k);
    load(all_var, name, k_rlen_l, i, k);
    load(all_var, name, circle_R_1, i, k);
    load(all_var, name, circle_R_2, i, k);
    load(all_var, name, circle_R_3, i, k);
    load(all_var, name, circle_R_4, i, k);
    load(all_var, name, k_rlen_r, i, k);
    page_alloc(page_num, page_name, j, k, "circle"); //num:12

    load(all_var, name, special_type, i, k);
    load(all_var, name, special_cross, i, k);
    load(all_var, name, special_ring, i, k);
    load(all_var, name, special_dir_ts, i, k);
    load(all_var, name, dir_ts_limit, i, k);
    page_alloc(page_num, page_name, j, k, "special"); //num:5

}
void Keys_update ()
{
    help_opeartion = opeartion;
    key_scanner();
    uint32_t state = (uint32_t) key_get_state(0) * 1000 + (uint32_t) key_get_state(1) * 100
            + (uint32_t) key_get_state(2) * 10 + (uint32_t) key_get_state(3);
    opeartion = (menu_opeartion) (state);

}
void show_update ()
{

    if (help_opeartion != opeartion)
        ips200_clear();
    ips200_show_float(24, 256, kp_pl, 2, 3);
    int y = 0;
    switch (mod)
    {
        case Menu :
            ips200_show_string(0, 288, "times:");
            ips200_show_float(8 * 6, 288, times, 3, 5);
            ips200_show_string(8 * 15, 288, "page:");
            ips200_show_uint(8 * 20, 288, page + 1, 2);
            ips200_show_string(0, 272, page_name[page]);
            for (int i = 0;
                    i
                            < ((page_num[page] - menu_offset + 1)
                                    < (page_num[page] + 1 < NUM_PAGE ? page_num[page] + 1 : NUM_PAGE) ?
                                    (page_num[page] - menu_offset + 1) :
                                    (page_num[page] + 1 < NUM_PAGE ? page_num[page] + 1 : NUM_PAGE)); i++)
            {

                for (int j = 0; j < page; j++)
                {
                    prev += page_num[j] + 1;
                }
                if (i + menu_offset == now_t)
                    ips200_set_color(RGB565_RED, RGB565_WHITE);
                ips200_show_string(0, y, name[i + prev + menu_offset]);
                ips200_show_float(8 * 20, y, (double) *all_var[i + prev + menu_offset], 4, 4);
                y += 16;
                ips200_set_color(RGB565_BLACK, RGB565_WHITE);
                // if(i+page*NUM_PAGE==NUM_VAR-1)
                //     break;
                prev = 0;
            }
            break;
        case Flash :
            ips200_show_string(0, 16 * 9, "Region 1:operation");
            for (int i = STORAGE_START_REGION; i <= MAX_REGION; i++)
            {
                if (i == now_t)
                    ips200_set_color(RGB565_RED, RGB565_WHITE);
                ips200_show_string(0, y, "Region ");
                ips200_show_uint(7 * 8, y, i, 1);
                if (flash_check(0, i))
                    ips200_show_string(8 * 8, y, ": Used");
                else
                    ips200_show_string(8 * 8, y, ": Unused");
                ips200_set_color(RGB565_BLACK, RGB565_WHITE);
                y += 16;
            }
            break;
            break;
        default :
            break;
    }
    int t = 1000, pos = 5;
    uint32_t num_op = (uint32_t) opeartion;
    for (int i = 0; i < 4; i++)
    {
        if (num_op / t == 0)
            op[pos] = 'N'; // 鏈寜锟�?
        else if (num_op / t == 1)
            op[pos] = 'S'; // 鐭寜
        else if (num_op / t == 2)
            op[pos] = 'L'; // 闀挎寜
        num_op %= t;
        t /= 10;
        pos += 7;
    }
    ips200_show_string(0, 304, op);
}
void flash_read (uint8_t region)
{
    if (region < OPERATION_REGION || region > MAX_REGION)
    {
        ips200_show_string(0, 256, "Illegal region");
        return;
    }
    flash_buffer_clear();
    flash_read_page_to_buffer(0, region);

    for (int i = 0; i < NUM_VAR; i++)
    {

        // ips200_show_string(0, 0, Main_menu->sub_container[i]->name);
        *all_var[i] = flash_union_buffer[i].float_type;
    }
}

void flash_write (uint8_t region)
{
    if (region < OPERATION_REGION || region > MAX_REGION)
    {
        ips200_show_string(0, 256, "Illegal region");
        return;
    }
    for (int i = 0; i < NUM_VAR; i++)
        flash_union_buffer[i].float_type = *all_var[i];
    if (flash_check(0, region))
        flash_erase_page(0, region);
    flash_write_page(0, region, (uint32*) flash_union_buffer, NUM_VAR);
    flash_buffer_clear();
}

void ms_op ()
{
    static uint8_t press_count = 0;

    if (opeartion != None)
    {

        switch (mod)
        {
            case Menu :
            {
                switch (opeartion)
                {
                    case Key1 :
                        if (times == 1)
                            times = 10;
                        else if (times == 10)
                            times = 100;
                        else if (times == 100)
                            times = 0.01;
                        else if (times == 0.01)
                            times = 0.1;
                        else if (times == 0.1)
                            times = 1;
                        break;
                    case Key2 :

                        if (page == PAGES)
                            page = 0;
                        else
                            page++;
                        now_t = 0;
                        menu_offset = 0;
                        break;
                    case Long_Key3 :
                        if (press_count < KEY_CONTINOUS)
                        {
                            press_count++;
                            break;
                        }
                        else
                            press_count = 0;
                    case Key3 :

                        if (now_t)
                            now_t--;
                        else
                        {

                            now_t = page_num[page];
                            if (page_num[page] + 1 > NUM_PAGE)
                                menu_offset = (page_num[page] + 1) / (uint8_t) NUM_PAGE * NUM_PAGE - 1;
                        }
                        if (now_t < menu_offset)
                            menu_offset--;
                        break;
                    case Long_Key4 :
                        if (press_count < KEY_CONTINOUS)
                        {
                            press_count++;
                            break;
                        }
                        else
                            press_count = 0;
                    case Key4 :

                        if (now_t != page_num[page])
                            now_t++;
                        else
                        {
                            now_t = 0;
                            menu_offset = 0;
                        }
                        if (now_t == NUM_PAGE + menu_offset)
                            menu_offset++;
                        break;
                    case Key1_LongKey2 :
                        mod = Flash;
                        menu_offset = 0;
                        now_t = STORAGE_START_REGION;
                        break;
                    case Key3_LongKey1 :
                        for (int j = 0; j < page; j++)
                        {
                            prev += page_num[j] + 1;
                        }
                        *all_var[prev + now_t] += times;
                        break;
                    case Key4_LongKey1 :
                        for (int j = 0; j < page; j++)
                        {
                            prev += page_num[j] + 1;
                        }
                        *all_var[prev + now_t] -= times;
                        break;
                    case Key3_LongKey2 :
                        flash_write(OPERATION_REGION);
                        break;
                    case Key4_LongKey2 :
                        flash_read(OPERATION_REGION);
                        break;
                    default :
                        break;
                }
                prev = 0;
            }
                break;
            case Flash :
                switch (opeartion)
                {
                    case Key3_LongKey2 :
                        flash_write(OPERATION_REGION);
                        break;
                    case Key4_LongKey2 :
                        flash_read(OPERATION_REGION);
                        break;
                    case Long_Key1 :
                        flash_erase_page(0, now_t);
                        break;
                    case Long_Key3 :
                        flash_write(now_t);
                        break;

                    case Key3 :
                        if (now_t != STORAGE_START_REGION)
                        {
                            now_t--;
                        }
                        else
                        {
                            now_t = MAX_REGION;
                        }
                        break;
                    case Long_Key4 :
                        flash_read(now_t);
                        break;
                    case Key4 :

                        if (now_t != MAX_REGION)
                        {
                            now_t++;
                        }
                        else
                        {
                            now_t = STORAGE_START_REGION;
                        }
                        break;
                    case Long_Key1234 :
                        for (int i = STORAGE_START_REGION; i <= MAX_REGION; i++)
                            flash_erase_page(0, i);
                        break;
                    case Key1_LongKey2 :
                        mod = Menu;
                        now_t = 0;
                        break;
                    default :
                        break;
                }
                break;

            default :
                break;
        }
    }
}
