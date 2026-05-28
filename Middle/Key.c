#include "Key.h"
#include "multi_button.h"

#define KEY_NUM 4
 
static Button key[KEY_NUM];  
static uint8_t key_num = 0;

static void SingleClickCallback(Button* handle, void* user_data)
{
    key_num = handle->button_id + 1;
}

static uint8_t read_key_level(uint8_t button_id)
{
    switch (button_id) {
        case 0: return (uint8_t)(DL_GPIO_readPins(BUTTON_BUTTON1_PORT, BUTTON_BUTTON1_PIN) != 0);
        case 1: return (uint8_t)(DL_GPIO_readPins(BUTTON_BUTTON2_PORT, BUTTON_BUTTON2_PIN) != 0);
        case 2: return (uint8_t)(DL_GPIO_readPins(BUTTON_BUTTON3_PORT, BUTTON_BUTTON3_PIN) != 0);
        case 3: return (uint8_t)(DL_GPIO_readPins(BUTTON_BUTTON4_PORT, BUTTON_BUTTON4_PIN) != 0);
        default: return 0;  
    }
}

void Key_Init(void)
{
    for(int i = 0; i < KEY_NUM; i++)
    {
        button_init(&key[i], read_key_level, 0, i);  
        button_attach(&key[i], BTN_SINGLE_CLICK, SingleClickCallback, NULL);
        button_start(&key[i]);
    }
}

uint8_t Key_Num_Get(void)
{
    uint8_t num = key_num;
    key_num = 0;
    return num;
}