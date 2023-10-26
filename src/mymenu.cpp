#include "mymenu.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


void MYMENU::init(void (*func_4in1)(int16_t)){ ptr_4in1 = func_4in1; }

void MYMENU::showOnDisplay1(){ (*ptr_4in1)(1);}
void MYMENU::showOnDisplay2(){ (*ptr_4in1)(2); }
void MYMENU::function_setup(){ (*ptr_4in1)(3); }
void MYMENU::function_loop(){ (*ptr_4in1)(4); }
void MYMENU::function_exit(){( *ptr_4in1)(5); }

void MYMENU::activate_node()  { menu_state = 1;    }
void MYMENU::deactivate_node(){ menu_state = 0;    }
bool MYMENU::is_activated()   { return menu_state; }

int16_t MYMENU::get_num_valid_submenus(){ return num_valid_submenus; }

void MYMENU::set_submenu(MYMENU *m1){ submenu_list[0] = m1; num_valid_submenus = 1; }
void MYMENU::set_submenu(MYMENU *m1, MYMENU *m2){ set_submenu(m1); submenu_list[1] = m2; num_valid_submenus = 2; }
void MYMENU::set_submenu(MYMENU *m1, MYMENU *m2, MYMENU *m3){ set_submenu(m1, m2); submenu_list[2] = m3; num_valid_submenus = 3; }
void MYMENU::set_submenu(MYMENU *m1, MYMENU *m2, MYMENU *m3, MYMENU *m4){ set_submenu(m1, m2, m3); submenu_list[3] = m4; num_valid_submenus = 4; }
void MYMENU::set_submenu(MYMENU *m1, MYMENU *m2, MYMENU *m3, MYMENU *m4, MYMENU *m5){ set_submenu(m1, m2, m3, m4); submenu_list[4] = m5; num_valid_submenus = 5; }
void MYMENU::set_submenu(MYMENU *m1, MYMENU *m2, MYMENU *m3, MYMENU *m4, MYMENU *m5, MYMENU *m6){ set_submenu(m1, m2, m3, m4, m5); submenu_list[5] = m6; num_valid_submenus = 6; }
void MYMENU::set_submenu(MYMENU *m1, MYMENU *m2, MYMENU *m3, MYMENU *m4, MYMENU *m5, MYMENU *m6, MYMENU *m7){ set_submenu(m1, m2, m3, m4, m5, m6); submenu_list[6] = m7; num_valid_submenus = 7; }


void NAVI::init(MYMENU *starting_menu, MYMENU* exit_menu, AiEsp32RotaryEncoder *rotaryEncoder){
    primitive = starting_menu;
    // use submenu0(mymenu_home) of mymenu_black when boot
    starting_menu->submenu_list[0]->activate_node(); 
    current_menu = starting_menu->submenu_list[0];
    exitt = exit_menu;
    encoder = rotaryEncoder;
    encoder->setBoundaries(0, current_menu->get_num_valid_submenus()-1, 1);
    encoder->reset(0); // reset the encoder to prevent a[4] = 5 for (a[2])
}


MYMENU* NAVI::find_offspring(MYMENU *amenu){

    for(int16_t i=0; i<amenu->get_num_valid_submenus(); i++){
        if(amenu->submenu_list[i] != NULL) if(amenu->submenu_list[i]->is_activated() == 1) return find_offspring(amenu->submenu_list[i]);
    }
    return amenu;
}


#define time_update_delay 50
#define time_button_delay 400

void NAVI::update(){
    static unsigned long timer_update = millis();
    static unsigned long timer_lastpress = millis();

    if(millis() - timer_update >= time_update_delay){
        timer_update = millis();

        current_menu->showOnDisplay1();
        current_menu->submenu_list[encoder->readEncoder()]->showOnDisplay2();
    }
    
    if (encoder->isEncoderButtonClicked() and (millis() - timer_lastpress > time_button_delay)){
        timer_lastpress = millis();
        if(current_menu->submenu_list[encoder->readEncoder()] == exitt)
        {
            exitt->function_setup();
            current_menu->function_exit();
            current_menu->deactivate_node();
        }
        else (current_menu->submenu_list[encoder->readEncoder()])->activate_node();
        current_menu = find_offspring(primitive);
        encoder->setBoundaries(0, current_menu->get_num_valid_submenus() - 1, 1);
        encoder->reset(0); // reset the encoder to prevent a[4] = 5 for (a[2])
        current_menu->function_setup();
    }

    current_menu->function_loop();
    
}



/*
void MYMENU::add_submenu(MYMENU *menu){
    submenu_list[num_valid_submenus] = menu;
    num_valid_submenus += 1;
}
*/

/*
MYMENU* NAVI::find_offspring(MYMENU *amenu){
    bool the_end = 0;
    uint8_t count = 0;
    while(the_end == 0){
        for(int16_t i=0; i<amenu->get_num_valid_submenus(); i++){
            if(amenu->submenu_list[i] != NULL) if(amenu->submenu_list[i]->is_activated() == 1){
                amenu = amenu->submenu_list[i];
                count = 0;
            }else count += 1;
        }
        if(count == amenu->get_num_valid_submenus()) break;
    }
    return amenu;
}
*/