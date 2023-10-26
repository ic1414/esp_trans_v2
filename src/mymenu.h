#ifndef mymenu_h
#define mymenu_h

#include <Arduino.h>
#include <AiEsp32RotaryEncoder.h>


#define mymenu_max_subs 7 // max submenu per menu

class MYMENU
{
private:
    bool menu_state = 0;
    uint8_t num_valid_submenus = 0;
    void (*ptr_4in1)(int16_t) = NULL;

public:    
    void init(void (*)(int16_t));
    bool is_activated();
    void activate_node();
    void deactivate_node();

    void showOnDisplay1();
    void showOnDisplay2();
    void function_setup();
    void function_loop();
    void function_exit();

    // void add_submenu(MYMENU *); abandoned
    void set_submenu(MYMENU *); void set_submenu(MYMENU *, MYMENU *); void set_submenu(MYMENU *, MYMENU *, MYMENU *);
    void set_submenu(MYMENU *, MYMENU *, MYMENU *, MYMENU *); void set_submenu(MYMENU *, MYMENU *, MYMENU *, MYMENU *, MYMENU *);
    void set_submenu(MYMENU *, MYMENU *, MYMENU *, MYMENU *, MYMENU *, MYMENU *); void set_submenu(MYMENU *, MYMENU *, MYMENU *, MYMENU *, MYMENU *, MYMENU *, MYMENU *);
    int16_t get_num_valid_submenus();
    MYMENU *submenu_list[mymenu_max_subs] = {NULL};  
};


class NAVI
{
private:
    MYMENU *primitive = NULL;
    MYMENU *exitt = NULL;
    MYMENU *current_menu = NULL;
    MYMENU *find_offspring(MYMENU *);
    AiEsp32RotaryEncoder *encoder;

public:
    void init(MYMENU *, MYMENU *, AiEsp32RotaryEncoder *);
    void update();
};

#endif
