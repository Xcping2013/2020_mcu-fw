#ifndef __MPM_H
#define __MPM_H


#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "bsp_mcu_delay.h"
#include "bsp_mcu_gpio.h"
#include "bsp_eeprom_24xx.h"


#define	RMII_REF_CLK_PIN			GET_PIN(A, 1)
#define	RMII_REF_MDIO_PIN			GET_PIN(A, 2)
#define	RMII_CRS_DV_PIN				GET_PIN(A, 7)
#define	RMII_MDC_PIN					GET_PIN(C, 1)
#define	RMII_TXD0_PIN					GET_PIN(G, 13)
#define	RMII_RXD0_PIN					GET_PIN(C, 4)
#define	RMII_TXD1_PIN					GET_PIN(B, 13)
#define	RMII_RXD1_PIN					GET_PIN(C, 5)
#define	RMII_TX_EN_PIN				GET_PIN(G, 11)

#define DUT_5V_EN_PIN					GET_PIN(G, 4)
#define	DUT_UART_SEL_PIN			GET_PIN(A, 5)
#define	DUT_TX_MCU_RX_PIN			GET_PIN(D, 2)
#define DUT_RX_MCU_TX_PIN     GET_PIN(C, 12)  //UART5


#define	BUF_DUT_GPIO_1_PIN		GET_PIN(A, 3)
#define	BUF_DUT_GPIO_2_PIN		GET_PIN(A, 3)
#define	BUF_MISC_GPIO_1_PIN		GET_PIN(C, 2)
#define	BUF_MISC_GPIO_2_PIN		GET_PIN(B, 1)



#define	DOOR_DETECT_PIN				GET_PIN(A, 6)

#define	VALVE_CTRL_PUFF1_PIN	GET_PIN(A, 12)
#define	VALVE_CTRL_PUFF2_PIN	GET_PIN(A, 11)
#define	VALVE_CTRL_PUFF3_PIN	GET_PIN(A, 8)

#define	VALVE_CTRL_PUFF4_PIN	GET_PIN(C, 9)
#define	VALVE_CTRL_PUFF5_PIN	GET_PIN(C, 8)

#define	VALVE_CTRL_PUFF6_PIN	GET_PIN(C, 7)
#define	VALVE_CTRL_PUFF7_PIN	GET_PIN(C, 6)

#define	PRG_RX_MCU_TX_PIN			GET_PIN(A, 9)
#define	PRG_TX_MCU_RX_PIN			GET_PIN(A, 10)

#define	DISPLAY_RX_MCU_TX_PIN		GET_PIN(D, 5)
#define DISPLAY_TX_MCU_RX_PIN   GET_PIN(D, 6)
#define DISPLAY_RESET_PIN       GET_PIN(D, 7)

#define	CLI_RX_MCU_TX_PIN				GET_PIN(D, 8)
#define CLI_TX_MCU_RX_PIN   		GET_PIN(D, 9)

#define	MISC_RX_MCU_TX_PIN			GET_PIN(E, 8)
#define MISC_TX_MCU_RX_PIN   		GET_PIN(E, 7)

#define	MFC_RX_MCU_TX_PIN				GET_PIN(G, 14)
#define MFC_TX_MCU_RX_PIN   		GET_PIN(G, 9)

#define SCALE_RX_MCU_TX_PIN     GET_PIN(C, 10)  //USART3
#define SCALE_TX_MCU_RX_PIN     GET_PIN(C, 11)

#define LED_RED_PIN             GET_PIN(D, 12)
#define LED_GRN_PIN           	GET_PIN(D, 13)
#define LED_BLUE_PIN            GET_PIN(D, 14)

#define SD_SCLK_PIN             GET_PIN(E, 2)   //SPI
#define SD_CS_PIN               GET_PIN(E, 4)
#define SD_MISO_PIN             GET_PIN(E, 5)
#define SD_MOSI_PIN             GET_PIN(E, 6)
#define SD_DETECT_PIN           GET_PIN(F, 2)

#define BOARD_I2C_SCL_PIN       GET_PIN(B, 10)
#define BOARD_I2C_SDA_PIN       GET_PIN(B, 11)

#define I2C0_SCL_PIN            GET_PIN(B, 8)
#define I2C0_SDA_PIN            GET_PIN(B, 9)

#define PB_SW1_PIN            GET_PIN(B, 6)
#define PB_SW2_PIN            GET_PIN(B, 7)

#define	PS_EOC_PIN							GET_PIN(D, 4)

#define EEPROM_WP_PIN           GET_PIN(E, 14)  


#if 1
#define SCALE_RX_MCU_TX_PIN     GET_PIN(C, 10)  //USART3
#define SCALE_TX_MCU_RX_PIN     GET_PIN(C, 11)

#define MFC_TX_MCU_RX_PIN       GET_PIN(G, 9)   //USART6
#define MFC_RX_MCU_TX_PIN       GET_PIN(G, 14)

#define DUT_RX_MCU_TX_PIN       GET_PIN(C, 12)  //UART5
#define DUT_TX_MCU_RX_PIN       GET_PIN(D, 2)

#define DUT_5V_EN_PIN						GET_PIN(G, 2)
#define DUT_UART_SEL_PIN				GET_PIN(A, 5)

#define TERM_TX_MCU_RX_PIN      GET_PIN(E, 0)   //UART8
#define TERM_RX_MCU_TX_PIN      GET_PIN(E, 1)

#define PRESSURE_SPI_CS_PIN 	  GET_PIN(A, 15)
#define PRESSURE_EOC_PIN 		    GET_PIN(D, 4)

#define DISPLAY_RX_MCU_TX_PIN   GET_PIN(D, 5)   //USART2
#define DISPLAY_TX_MCU_RX_PIN   GET_PIN(D, 6)
#define DISPLAY_RESET_PIN       GET_PIN(D, 7)

#define SD_SCLK_PIN             GET_PIN(E, 2)   //SPI4
#define SD_CS_PIN               GET_PIN(E, 4)
#define SD_MISO_PIN             GET_PIN(E, 5)
#define SD_MOSI_PIN             GET_PIN(E, 6)
#define SD_DETECT_PIN           GET_PIN(F, 2)

#define LED_RED_PIN             GET_PIN(F, 1)
#define LED_GREEN_PIN           GET_PIN(F, 0)
#define LED_BLUE_PIN            GET_PIN(D, 1)

#define I2C_IRQ1_PIN            GET_PIN(C, 9)   //I2C1    DLLR SENSOR     DLLR-L10D-E2BS-C-NAV6
#define I2C_SCL1_PIN            GET_PIN(B, 8)
#define I2C_SDA1_PIN            GET_PIN(B, 9)

#define BOARD_I2C_SCL_PIN       GET_PIN(B, 10)  //I2C2    EEPROM_AT24CXX  BME280
#define BOARD_I2C_SDA_PIN       GET_PIN(B, 11)

#define EEPROM_WP_PIN           GET_PIN(E, 14)    

#endif

extern uint8_t led_rgb_pin[3];
extern uint8_t valve_pin[7];
extern at24cxx_t at24c256;

int msh_exec(char *cmd, rt_size_t length);
int UserMsh_Commands(char *cmd, rt_size_t length);

void led_rgb_hw_init(void);
void valve_hw_init(void);

void mpm_outputs_test(void);

void dis_bme280_times(void);
void DLLR_readdata(void);
int app_bme280_active(void);
void sd_card_test(void);

void at24cxx_hw_init(void);
int savedata(int argc, char **argv);
int readdata(int argc, char **argv);

#endif
