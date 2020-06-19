/*
  Linux userspace test code, simple and mose code directy from the doco.
  compile like this: gcc linux_userspace.c ../bme280.c -I ../ -o bme280
  tested: Raspberry Pi.
  Use like: ./bme280 /dev/i2c-0
*/
#include "app_bme280.h"
#include "spi.h"
#include "bsp_mcu_delay.h"

uint32_t tickTask1start;	

int fd;

struct bme280_dev bme280_dev1;	
struct bme280_data bme280_data1;
	
void user_delay_ms(uint32_t period)
{
  delay_ms(period);
}
int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
//  write(fd, &reg_addr,1);
//  read(fd, data, len);
  return 0;
}

int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
//  int8_t *buf;
//  buf = malloc(len +1);
//  buf[0] = reg_addr;
//  memcpy(buf +1, data, len);
//  write(fd, buf, len +1);
//  free(buf);
  return 0;
}

void print_sensor_data(struct bme280_data *comp_data)
{
#ifdef BME280_FLOAT_ENABLE
  printf("temp %0.2f, p %0.2f, hum %0.2f\r\n",comp_data->temperature, comp_data->pressure, comp_data->humidity);
#else
  printf("temp %ld, p %ld, hum %ld\r\n",comp_data->temperature, comp_data->pressure, comp_data->humidity);
#endif
}

int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev)
{
  int8_t rslt;
  uint8_t settings_sel;
  struct bme280_data comp_data;

  /* Recommended mode of operation: Indoor navigation */
  dev->settings.osr_h = BME280_OVERSAMPLING_1X;
  dev->settings.osr_p = BME280_OVERSAMPLING_16X;
  dev->settings.osr_t = BME280_OVERSAMPLING_2X;
  dev->settings.filter = BME280_FILTER_COEFF_16;

  settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

  rslt = bme280_set_sensor_settings(settings_sel, dev);

  printf("Temperature, Pressure, Humidity\r\n");
  /* Continuously stream sensor data */
  while (1) {
    rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, dev);
    /* Wait for the measurement to complete and print data @25Hz */
    dev->delay_ms(40);
    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
    print_sensor_data(&comp_data);
  }
  return rslt;
}

int app_bme280_active(int argc, char* argv[])
{
  struct bme280_dev dev;
  int8_t rslt = BME280_OK;

//  if ((fd = open(argv[1], O_RDWR)) < 0) {
//    printf("Failed to open the i2c bus %s", argv[1]);
//    exit(1);
//  }
//  if (ioctl(fd, I2C_SLAVE, 0x76) < 0) {
//    printf("Failed to acquire bus access and/or talk to slave.\n");
//    exit(1);
//  }
  dev.dev_id = BME280_I2C_ADDR_PRIM;
  dev.intf = BME280_I2C_INTF;
  dev.read = user_i2c_read;
  dev.write = user_i2c_write;
  dev.delay_ms = user_delay_ms;

  rslt = bme280_init(&dev);
  stream_sensor_data_forced_mode(&dev);
	return rslt;
}


//
int8_t stream_sensor_data_normal_mode(struct bme280_dev *dev)
{
	int8_t rslt;
	uint8_t settings_sel;
	struct bme280_data comp_data;

	/* Recommended mode of operation: Indoor navigation */
	dev->settings.osr_h = BME280_OVERSAMPLING_1X;
	dev->settings.osr_p = BME280_OVERSAMPLING_16X;
	dev->settings.osr_t = BME280_OVERSAMPLING_2X;
	dev->settings.filter = BME280_FILTER_COEFF_16;
	dev->settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;
	rslt = bme280_set_sensor_settings(settings_sel, dev);
	rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, dev);

	printf("Temperature, Pressure, Humidity\r\n");
	while (1) {
		/* Delay while the sensor completes a measurement */
		dev->delay_ms(70);
		rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
		print_sensor_data(&comp_data);
	}

	return rslt;
}
//
int8_t user_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
	
		BME280_CS_PIN_0();
		HAL_SPI_Transmit(&hspi1,&reg_addr,1,0xFFFF);
	  HAL_SPI_Receive(&hspi1,reg_data,len,0xFFFF);
		BME280_CS_PIN_1();
    /*
     * The parameter dev_id can be used as a variable to select which Chip Select pin has
     * to be set low to activate the relevant device on the SPI bus
     */

    /*
     * Data on the bus should be like
     * |----------------+---------------------+-------------|
     * | MOSI           | MISO                | Chip Select |
     * |----------------+---------------------|-------------|
     * | (don't care)   | (don't care)        | HIGH        |
     * | (reg_addr)     | (don't care)        | LOW         |
     * | (don't care)   | (reg_data[0])       | LOW         |
     * | (....)         | (....)              | LOW         |
     * | (don't care)   | (reg_data[len - 1]) | LOW         |
     * | (don't care)   | (don't care)        | HIGH        |
     * |----------------+---------------------|-------------|
     */

    return rslt;
}

int8_t user_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
		BME280_CS_PIN_0();
		HAL_SPI_Transmit(&hspi1,&reg_addr,1,0xFFFF);
	  HAL_SPI_Transmit(&hspi1,reg_data,len,0xFFFF);
		BME280_CS_PIN_1();
    /*
     * The parameter dev_id can be used as a variable to select which Chip Select pin has
     * to be set low to activate the relevant device on the SPI bus
     */

    /*
     * Data on the bus should be like
     * |---------------------+--------------+-------------|
     * | MOSI                | MISO         | Chip Select |
     * |---------------------+--------------|-------------|
     * | (don't care)        | (don't care) | HIGH        |
     * | (reg_addr)          | (don't care) | LOW         |
     * | (reg_data[0])       | (don't care) | LOW         |
     * | (....)              | (....)       | LOW         |
     * | (reg_data[len - 1]) | (don't care) | LOW         |
     * | (don't care)        | (don't care) | HIGH        |
     * |---------------------+--------------|-------------|
     */

    return rslt;
}


int8_t Bme280InitViaSpi(struct bme280_dev *dev)
{
	int8_t rslt = BME280_OK;
	uint8_t settings_sel;

	dev->dev_id = 0;
	dev->intf = BME280_SPI_INTF;
	dev->read = user_spi_read;
	dev->write = user_spi_write;
	dev->delay_ms = user_delay_ms;
	rslt = bme280_init(dev);

	/* Recommended mode of operation: Indoor navigation */
	dev->settings.osr_h = BME280_OVERSAMPLING_1X;
	dev->settings.osr_p = BME280_OVERSAMPLING_16X;
	dev->settings.osr_t = BME280_OVERSAMPLING_2X;
	dev->settings.filter = BME280_FILTER_COEFF_16;
	dev->settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;
	rslt = bme280_set_sensor_settings(settings_sel, dev);
	rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, dev);

	printf("Temperature, Pressure, Humidity\r\n");
	
	tickTask1start=HAL_GetTick();
	
	return rslt;
}
void msTaskNoneRTOS_bme280(uint16_t msTime)
{
	if(abs((int)(HAL_GetTick()-tickTask1start))>msTime)
	{
		bme280_get_sensor_data(BME280_ALL, &bme280_data1, &bme280_dev1);
		print_sensor_data(&bme280_data1);
		tickTask1start=HAL_GetTick();
	}	
}
