#ifndef ISL29125_H
#define ISL29125_H

#include <stdint.h>
#include "color.h"
#include "bsp_defines.h"
#include "bsp_mcu_softi2c.h"

#ifdef __cplusplus
extern "C" {
#endif

	#define ISL29125_I2C_ADDRESS 0x88

/**
 * @name ISL29125 constants
 * @{
 */
#define ISL29125_ID             0x7D
/** @} */

/**
 * @name ISL29125 register map
 * @{
 */
/* main register */
#define ISL29125_REG_ID         0x0000//0x00
#define ISL29125_REG_RESET      0x0000//0x00
/* configuration registers */
#define ISL29125_REG_CONF1      0x0001//0x01
#define ISL29125_REG_CONF2      0x0002//0x02
#define ISL29125_REG_CONF3      0x0003//0x03
/* interrupt mode threshold registers */
#define ISL29125_REG_LTHLB      0x0004//0x04
#define ISL29125_REG_LTHHB      0x0005//0x05
#define ISL29125_REG_HTHLB      0x0006//0x06
#define ISL29125_REG_HTHHB      0x0007//0x07
/* status register */
#define ISL29125_REG_STATUS     0x0008//0x08
/* sensor readout registers (double buffered) */
#define ISL29125_REG_GDLB       0x0009//0x09
#define ISL29125_REG_GDHB       0x000A//0x0A
#define ISL29125_REG_RDLB       0x000B//0x0B
#define ISL29125_REG_RDHB       0x000C//0x0C
#define ISL29125_REG_BDLB       0x000D//0x0D
#define ISL29125_REG_BDHB       0x000E//0x0E
/** @} */

/**
 * @name ISL29125 commands
 * @{
 */
#define ISL29125_CMD_RESET      0x46
/** @} */

/**
 * @name ISL29125 configuration masks and bits
 * @{
 */
/* ISL29125_REG_CONF1 B2:B0 */
#define ISL29125_CON1_MASK_MODE 0x07
/* ISL29125_REG_CONF1 B3 */
#define ISL29125_CON1_MASK_RANGE 0x08
/* ISL29125_REG_CONF1 B4 */
#define ISL29125_CON1_MASK_RES  0x10
/* ISL29125_REG_CONF1 B5 */
#define ISL29125_CON1_MASK_SYNC 0x20
#define ISL29125_CON1_SYNCOFF   0x00
#define ISL29125_CON1_SYNCON    0x20

/**
 * @brief data type for storing lux RGB sensor readings
 */
typedef struct {
    float red;              /**< red lux value */
    float green;            /**< green lux value */
    float blue;             /**< blue lux value */
} isl29125_rgb_t;

/**
 * @brief supported operation modes of the ISL29125 sensor's AD
 * conversion
 */
typedef enum {
    ISL29125_MODE_DOWN = 0x00,      /**< ADC powered down */
    ISL29125_MODE_STANDBY = 0x04,   /**< AD conversion not performed */
    ISL29125_MODE_RGB = 0x05,       /**< RGB conversion */
    ISL29125_MODE_R = 0x02,         /**< red conversion only */
    ISL29125_MODE_G = 0x01,         /**< green conversion only */
    ISL29125_MODE_B = 0x03,         /**< blue conversion only */
    ISL29125_MODE_RG = 0x06,        /**< red and green conversion only */
    ISL29125_MODE_GB = 0x07         /**< green and blue conversion only */
} isl29125_mode_t;

/**
 * @brief supported RGB sensing range values of the ISL29125 sensor
 */
typedef enum {
    ISL29125_RANGE_375 = 0x00,      /**< range: 5.7m - 375 lux */
    ISL29125_RANGE_10K = 0x08       /**< range: 0.152 - 10,000 lux */
} isl29125_range_t;

/**
 * @brief supported color resolutions of the ISL29125 sensor's AD
 * conversion
 */
typedef enum {
    ISL29125_RESOLUTION_12 = 0x10,  /**< resolution: 12 bit */
    ISL29125_RESOLUTION_16 = 0x00   /**< resolution: 16 bit */
} isl29125_resolution_t;


/**
 * @brief Configuration-3 Register 0x03 B1:0
 */
typedef enum {
    ISL29125_INTERRUPT_STATUS_NONE = 0x00,      /**< No interrupt */
    ISL29125_INTERRUPT_STATUS_GREEN = 0x01,     /**< GREEN interrupt */
    ISL29125_INTERRUPT_STATUS_RED = 0x02,       /**< RED interrupt */
    ISL29125_INTERRUPT_STATUS_BLUE = 0x03       /**< BLUE interrupt */
} isl29125_interrupt_status_t;

/**
 * @brief Configuration-3 Register 0x03 B3:2
 */
typedef enum {
    ISL29125_INTERRUPT_PERSIST_1 = (0x00 << 2), /**< Int. Persist: Number of integration cycle 1 */
    ISL29125_INTERRUPT_PERSIST_2 = (0x01 << 2), /**< Int. Persist: Number of integration cycle 2 */
    ISL29125_INTERRUPT_PERSIST_4 = (0x02 << 2), /**< Int. Persist: Number of integration cycle 4 */
    ISL29125_INTERRUPT_PERSIST_8 = (0x03 << 2)  /**< Int. Persist: Number of integration cycle 8 */
} isl29125_interrupt_persist_t;

/**
 * @brief Configuration-3 Register 0x03 B4
 */
typedef enum {
    ISL29125_INTERRUPT_CONV_DIS = (0x0 << 4),   /**< RGB Conversion done to ~INT Control disable */
    ISL29125_INTERRUPT_CONV_EN = (0x1 << 4),    /**< RGB Conversion done to ~INT Control enable */
} isl29125_interrupt_conven_t;



/**
 * @brief Device descriptor for ISL29125 sensors
 */
typedef struct
{
//    Gpio *		scl_pin;             /**< I2C pin */
//		Gpio *		sda_pin;
	IIC_GPIO	pin;
	uint8_t   int_pin;							/**< GPIO pin for interrupt/sync mode */
	
	isl29125_mode_t mode;
	isl29125_range_t range;         /**< sensor range */
	isl29125_resolution_t res;      /**< sensor resolution */
	
	isl29125_interrupt_status_t	interrupt_status;
	isl29125_interrupt_persist_t interrupt_persist;
	isl29125_interrupt_conven_t interrupt_conven;
	uint16_t lower_threshold;
	uint16_t higher_threshold;
//		gpio_cb_t cb;
//		void (*arg)void;
			
} isl29125_t;

/**
 * @brief initialize a new ISL29125 device
 *
 * @param[in] dev           device descriptor of an ISL29125 device
 * @param[in] i2c           I2C device the sensor is connected to
 * @param[in] gpio          GPIO pin for interrupt/sync mode (currently unused)
 * @param[in] mode          operation mode
 * @param[in] range         measurement range
 * @param[in] resolution    AD conversion resolution
 *
 * @return              0 on success
 * @return              -1 on error
 */
uint8_t isl29125_init(isl29125_t *dev);
/**
 * @brief initialize interrupts
 *
 * @param[in] dev                   device descriptor of an ISL29125 device
 * @param[in] interrupt_status      Interrupt status
 * @param[in] interrupt_persist     Interrupt persistency
 * @param[in] interrupt_conven      RGB conversion done to interrupt control, enable
 * @param[in] lower_threshold       Lower interrupt threshold
 * @param[in] higher_threshold      Higher interrupt threshold
 * @param[in] cb                    Callback function on interrupts
 * @param[in] arg                   Argument passed to the callback function
 *
 * @return                          0 on success
 * @return                          -1 on error
 */
int isl29125_init_int(isl29125_t *dev);
/**
 * @brief read RGB values from device
 *
 * @param[in] dev       device descriptor of an ISL29125 device
 * @param[in] dest      pointer to lux RGB color object data is written to
 */
void isl29125_read_rgb_lux(const isl29125_t *dev, isl29125_rgb_t *dest);

 /**
 * @brief read color values from device
 *
 * @param[in] dev       device descriptor of an ISL29125 device
 * @param[in] dest      pointer to RGB color object data is written to
 */
void isl29125_read_rgb_color(const isl29125_t *dev, color_rgb_t *dest);

/**
 * @brief set the device's operation mode
 *
 * @param[in] dev       device descriptor of an ISL29125 device
 * @param[in] mode      operation mode
 */
void isl29125_set_mode(const isl29125_t *dev, isl29125_mode_t mode);

/**
 * @brief read isl29125 interrupt status
 *
 * @param[in] dev       device descriptor of an ISL29125 device
 *
 * @return              interrupt status
 */
int isl29125_read_irq_status(const isl29125_t *dev);

int isl29125_test(void);

/*		*/
int isl29125_devs_hw_init(void);
int light_sensosr_thread_init(void);
void lightSensor(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* ISL29125_H */
/** @} */
