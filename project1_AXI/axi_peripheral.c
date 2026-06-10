#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

// MMIO 주소 (기존 유지)
#define I2C_BASE    0x44a00000
#define GPIO_BASE   0x40000000

// I2C 레지스터 오프셋
#define REG_CONTROL 0x00
#define REG_TXDATA  0x04
#define REG_RXDATA  0x08
#define REG_STATUS  0x0C

// GPIO 레지스터 오프셋
#define GPIO_DATA   0x00
#define GPIO_DIR    0x04

#define SLAVE_ADDR  0x48
#define I2C_WRITE   0
#define I2C_READ    1

volatile uint32_t *i2c_ctrl  = (volatile uint32_t *)(I2C_BASE + REG_CONTROL);
volatile uint32_t *i2c_tx    = (volatile uint32_t *)(I2C_BASE + REG_TXDATA);
volatile uint32_t *i2c_rx    = (volatile uint32_t *)(I2C_BASE + REG_RXDATA);
volatile uint32_t *i2c_stat  = (volatile uint32_t *)(I2C_BASE + REG_STATUS);

volatile uint32_t *gpio_dir  = (volatile uint32_t *)(GPIO_BASE + GPIO_DIR);
volatile uint32_t *gpio_data = (volatile uint32_t *)(GPIO_BASE + GPIO_DATA);

int main() {
    uint32_t current_val = 0x55; // 초기 전송 값
    uint32_t rx_val;
    setvbuf(stdout, NULL, _IONBF, 0);

    // GPIO 방향 설정 (출력 모드)
    *gpio_dir = 0x00000000;

    printf("===== I2C Rx-to-Tx & GPIO Loopback Start =====\n");

    while (1) {
        printf("\n--- Starting I2C Write [Data: 0x%02X] ---\n", current_val);

        // 1. WRITE 사이클: 현재 값을 슬레이브에 전송
        *i2c_tx = current_val;
        *i2c_ctrl = (SLAVE_ADDR << 2) | (I2C_WRITE << 1) | 0x1;

        while(!(*i2c_stat & 0x1)); // Busy 대기
        *i2c_ctrl = 0x0;           // Start Pulse Clear
        while(*i2c_stat & 0x1);    // 완료 대기
        printf("Step 1: Write Complete.\n");

        // 2. READ 사이클: 슬레이브로부터 새로운 데이터 수신
        printf("Step 2: Starting I2C Read... ");
        *i2c_ctrl = (SLAVE_ADDR << 2) | (I2C_READ << 1) | 0x1;

        while(!(*i2c_stat & 0x1)); // Busy 대기
        *i2c_ctrl = 0x0;           // Start Pulse Clear
        while(*i2c_stat & 0x1);    // 완료 대기

        rx_val = (*i2c_rx) & 0xFF; // 하위 8비트 데이터 추출
        printf("Received: 0x%02X\n", rx_val);

        // 3. 수신된 데이터를 다음 전송 값과 GPIO로 전달
        current_val = rx_val;      // 다음 루프의 TX 데이터가 됨
        *gpio_data = rx_val;       // GPIO(LED 등)로 출력
        printf("Step 3: GPIO & Next TX updated with 0x%02X\n", current_val);
    }
}
