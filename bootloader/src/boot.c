/* Code based on https://github.com/rogerclarkmelbourne/STM32duino-bootloader/blob/master/hardware.c */

#include <inttypes.h>
#include <stm32f1xx.h>

#include "config.h"

#define SET_REG(addr,val) do { *(volatile uint32_t*)(addr)=val; } while(0)
#define GET_REG(addr)     (*(volatile uint32_t*)(addr))
#define GPIO_BSRR(port) ((uint32_t)port+0x10)
#define GPIO_IDR(port)  ((uint32_t)port+0x08)
#define CR_INPUT_PU_PD      0x08
#define CR_OUTPUT_PP        0x01
#define CR_SHITF(pin) ((pin - 8*(pin>7))<<2)
#define GPIO_CR(port,pin) ((uint32_t)port + (0x04*(pin>7)))

// Used to create the control register masking pattern, when setting control register mode.
static unsigned int crMask(int pin)
{
    unsigned int mask;
    if (pin>=8)
    {
        pin-=8;
    }
    mask = 0x0F << (pin<<2);
    return ~mask;
}

int checkUserCode(void) {
    uint32_t sp = *(volatile uint32_t *) USER_PROGRAM;

    if ((sp & 0x2FFE0000) == 0x20000000) {
        return 0;
    } else {
        return 1;
    }
}

int checkAndClearBootloaderFlag(void) {
    int flag = 0;

    // Enable clocks for the backup domain registers
    RCC->APB1ENR |= (RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN);

    if (BKP->DR10 == RTC_BOOTLOADER_FLAG) {
        flag = 1;
    }

    // Disable backup register write protection
    PWR->CR |= PWR_CR_DBP;
    // store value in pBK DR10
    BKP->DR10 = 0;
    // Re-enable backup register write protection
    PWR->CR &=~ PWR_CR_DBP;

    // Disable clocks
    RCC->APB1ENR &= ~(RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN);

    return flag;
}

static void gpio_write_bit(uint32_t bank, uint8_t pin, uint8_t val) {
    val = !val;          // "set" bits are lower than "reset" bits
    SET_REG(GPIO_BSRR(bank), (1U << pin) << (16 * val));
}

static int readPin(GPIO_TypeDef *bank, uint8_t pin) {
    if (bank->IDR & (0x01 << pin)) {
        return 1;
    } else {
        return 0;
    }
}

int checkKbMatrix(void) {
    gpio_write_bit(BL_OUTPUT_BANK, BL_OUTPUT_PIN, 1);
    for (volatile int delay = 0; delay < 1000; ++delay) {}
    return readPin(BL_INPUT_BANK, BL_INPUT_PIN);
}

void setupGPIO(void) {
    RCC->APB2ENR |= 0b111111100;// Enable All GPIO channels (A to G)

    // setup our two gpios for input and output
    SET_REG(GPIO_CR(BL_INPUT_BANK,BL_INPUT_PIN),(GET_REG(GPIO_CR(BL_INPUT_BANK,BL_INPUT_PIN)) & crMask(BL_INPUT_PIN)) | CR_INPUT_PU_PD << CR_SHITF(BL_INPUT_PIN));
    // set input pulldown
    gpio_write_bit(BL_INPUT_BANK, BL_INPUT_PIN, 0);
    SET_REG(GPIO_CR(BL_OUTPUT_BANK,BL_OUTPUT_PIN),(GET_REG(GPIO_CR(BL_OUTPUT_BANK,BL_OUTPUT_PIN)) & crMask(BL_OUTPUT_PIN)) | CR_OUTPUT_PP << CR_SHITF(BL_OUTPUT_PIN));
}
