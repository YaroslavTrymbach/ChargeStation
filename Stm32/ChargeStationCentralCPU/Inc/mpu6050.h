#include "stm32f3xx_hal.h"
//#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define MPU6050_ACCEL_SCALE_2G 0
#define MPU6050_ACCEL_SCALE_4G 1
#define MPU6050_ACCEL_SCALE_8G 2
#define MPU6050_ACCEL_SCALE_16G 3

#define MPU6050_SELF_TEST_FLAG_ALL 0
#define MPU6050_SELF_TEST_FLAG_X   1
#define MPU6050_SELF_TEST_FLAG_Y   2
#define MPU6050_SELF_TEST_FLAG_Z   3

typedef struct _mpu6050AccelRowData{
	int16_t x;
	int16_t y;
	int16_t z;
}mpu6050AccelRowData;

typedef struct _mpu6050AccelData{
	float x;
	float y;
	float z;
}mpu6050AccelData;

typedef struct _mpu6050GyroRowData{
	int16_t x;
	int16_t y;
	int16_t z;
}mpu6050GyroRowData;

typedef struct _mpu6050GyroData{
	float x;
	float y;
	float z;
}mpu6050GyroData;


bool mpu6050_init(I2C_HandleTypeDef *i2c);

bool mpu6050_getAccelRowData(mpu6050AccelRowData *rowData);

bool mpu6050_setAccelScale(uint8_t scale);

bool mpu6050_getAccelScale(uint8_t *scale);

bool mpu6050_getAccelDataFromRow(mpu6050AccelData *data, mpu6050AccelRowData *rowData, uint8_t scale);

bool mpu6050_getGyroRowData(mpu6050GyroRowData *rowData);

bool mpu6050_turnOnAccelSelfTest(uint8_t flag);
bool mpu6050_turnOffAccelSelfTest(void);

bool mpu6050_getAccelOffset(mpu6050AccelRowData *rowData);
bool mpu6050_setAccelOffset(mpu6050AccelRowData *rowData);
