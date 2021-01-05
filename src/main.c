/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "main.h"

/* Private defines -----------------------------------------------------------*/
#define BRIGHTNESS (20)

typedef struct
{
    uint8_t green;
    uint8_t red;
    uint8_t blue;

} ws2812b_led_t;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void init(void)
{
    /* init clock to 16 MHz */

    // HSIDIV = 1, CPUDIV = 1
    CLK->CKDIVR = 0;

    /* init GPIO */

    // OUT OD
    LED_PORT->DDR |= LED_PIN;

    // OUT PP Fast
    WS2812B_PORT->ODR &= ~WS2812B_PIN;
    WS2812B_PORT->DDR |= WS2812B_PIN;
    WS2812B_PORT->CR1 |= WS2812B_PIN;
    WS2812B_PORT->CR2 |= WS2812B_PIN;

}


static void delay_us(uint32_t us)
{
    // very very rough estimation
    for(uint32_t i = 0; i < us; i++)
    {
        __asm__("nop\n nop\n nop\n");
        __asm__("nop\n nop\n nop\n");
    }
}

static void ws2812b_send_reset(void)
{
    // 50+ us low
    //WS2812B_PORT->ODR &= ~WS2812B_PIN;
    for(uint8_t i = 0; i < 254; i++)
    {
        __asm__("nop\n nop\n nop\n nop\n nop\n nop\n");
        __asm__("nop\n nop\n nop\n nop\n nop\n nop\n");
        __asm__("nop\n nop\n nop\n nop\n nop\n nop\n");
        __asm__("nop\n nop\n nop\n nop\n nop\n nop\n");
        __asm__("nop\n nop\n nop\n nop\n nop\n nop\n");
    }
    return;
}

void ws2812b_send_byte(uint8_t byte)
{
    int8_t i = 8;
    while(i > 0)
    {
        if (byte & (1 << (i-1)))
        {
            // send 1: 0.8 us high, 0.45 us low
            WS2812B_PORT->ODR |= WS2812B_PIN;
            __asm__("nop\n nop\n nop\n");
            __asm__("nop\n nop\n nop\n");
            __asm__("nop\n nop\n nop\n");
            WS2812B_PORT->ODR &= ~WS2812B_PIN;
        } else {
            // send 0: 0.4 us high, 0.85 us low
            WS2812B_PORT->ODR |= WS2812B_PIN;
            __asm__("nop\n nop\n nop\n");
            WS2812B_PORT->ODR &= ~WS2812B_PIN;
            __asm__("nop\n nop\n nop\n");
        }
        //__asm__("nop\n nop\n nop\n");
        i--;
    }

}

void ws2812b_send_leds(ws2812b_led_t *leds, uint8_t count)
{
    for(uint8_t i = 0; i < count; i++)
    {
        // G7..G0 R7..R0 B7..B0, MSB first
        ws2812b_send_byte(leds[i].green);
        ws2812b_send_byte(leds[i].red);
        ws2812b_send_byte(leds[i].blue);
    }
    ws2812b_send_reset();
    return;
}

void set_color(ws2812b_led_t *led, uint8_t bits)
{
    led->red   = (bits & 1) ? BRIGHTNESS : 0;
    led->green = (bits & 2) ? BRIGHTNESS : 0;
    led->blue  = (bits & 4) ? BRIGHTNESS : 0;
}


void main(void)
{
    init();

    ws2812b_led_t led;
    uint8_t color = 0;

    while(1) {
        set_color(&led, color);
        ws2812b_send_leds(&led, 1);

        color++;
        if (color >= 8) color = 0;

        delay_us(1000000);
    }

}



/***********************************************************************/

/* Interrupt example
void tim4_isr(void) __interrupt(ITC_IRQ_TIM4_OVF) {

}
*/


/* Debug output

int putchar (int c)
{
    // this implementation is SDCC 3.8 specific
    // see sdcc stdio.h for details
    UART1->DR = (uint8_t) c;
    while (!(UART1->SR & UART1_FLAG_TXE));
    return c;
}
*/

// This is called by some of the SPL files on error.
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  (void)file;
  (void)line;

  /* User can add their own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
