#include "CC1310_LAUNCHXL.h"
#include "sk6812.h"
#include <ti/devices/cc13x0/driverlib/gpio.h>
#include <ti/devices/cc13x0/driverlib/ioc.h>
#include <ti/devices/cc13x0/driverlib/cpu.h>
#include <ti/sysbios/hal/Hwi.h>

#define SK_PIN  NODE_SK6812_DIN

static inline void dly(uint32_t cycles) { CPUdelay(cycles); }

// 48MHz 기준 대략치(필요 시 숫자만 미세조정)
static inline void send0(void){
  GPIO_writeDio(SK_PIN, 1); dly(16);
  GPIO_writeDio(SK_PIN, 0); dly(44);
}
static inline void send1(void){
  GPIO_writeDio(SK_PIN, 1); dly(34);
  GPIO_writeDio(SK_PIN, 0); dly(30);
}

static void sendByte(uint8_t b){
    int i;
    for (i = 7; i >= 0; i--) {
        (b & (1U << i)) ? send1() : send0();
    }
}

void sk6812_init(void){
  IOCIOPortPullSet(SK_PIN, IOC_NO_IOPULL);
  IOCIOPortIdSet(SK_PIN, IOC_PORT_GPIO);
  GPIO_setOutputEnableDio(SK_PIN, GPIO_OUTPUT_ENABLE);
  GPIO_writeDio(SK_PIN, 0);
}

void sk6812_show_rgb(uint8_t r, uint8_t g, uint8_t b){
    UInt key = Hwi_disable();     // 인터럽트 off

    sendByte(g);
    sendByte(r);
    sendByte(b);

    Hwi_restore(key);             // 인터럽트 원복
    dly(48 * 100);                // latch > 80us
}
