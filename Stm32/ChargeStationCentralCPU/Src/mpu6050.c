#include "mpu6050.h"
#include <string.h>

#define I2C_ADDRESS 0xD0

// Registers
// Some registers is not described in official documentation 
// https://www.i2cdevlib.com/devices/mpu6050#registers

#define REG_ACCEL_XOFF 0x06 //[15:0] XA_OFFS
#define REG_ACCEL_YOFF 0x08 //[15:0] YA_OFFS
#define REG_ACCEL_ZOFF 0x0A //[15:0] ZA_OFFS

#define REG_CONFIG       0x1A
#define REG_GYRO_CONFIG  0x1B
#define REG_ACCEL_CONFIG 0x1C

#define REG_ACCEL_XOUT   0x3B
#define REG_ACCEL_YOUT   0x3C
#define REG_ACCEL_ZOUT   0x3F
#define REG_TEMP_OUT     0x41
#define REG_GYRO_XOUT    0x43
#define REG_GYRO_YOUT    0x45
#define REG_GYRO_ZOUT    0x47

#define REG_PWR_MGMT_1   0x6B

#define REG_WHO_AM_I     0x75

I2C_HandleTypeDef *hi2c;

bool mpu6050_writeData(uint8_t regAdr, uint8_t *sendData, uint8_t len){
	uint8_t data[16];
	data[0] = regAdr;
	memcpy(data + 1, sendData, len);
	if(HAL_I2C_Master_Transmit(hi2c, I2C_ADDRESS, data, len + 1, 10) != HAL_OK){
		return false;
	}
	
	return true;
}

bool mpu6050_readData(uint8_t regAdr, uint8_t *data, uint8_t len){
	if(HAL_I2C_Master_Transmit(hi2c, I2C_ADDRESS, &regAdr, 1, 10) != HAL_OK){
		return false;
	}
	
	if(HAL_I2C_Master_Receive(hi2c, I2C_ADDRESS, data, len, 10) != HAL_OK){
		return false;
	}
	
	return true;
}

bool mpu6050_readReg(uint8_t regAdr, uint8_t *value){
	if(HAL_I2C_Master_Transmit(hi2c, I2C_ADDRESS, &regAdr, 1, 10) != HAL_OK){
		return false;
	}
	
	if(HAL_I2C_Master_Receive(hi2c, I2C_ADDRESS, value, 1, 10) != HAL_OK){
		return false;
	}
	
	return true;
}

bool mpu6050_writeReg(uint8_t regAdr, uint8_t value){
	uint8_t data[2];
	data[0] = regAdr;
	data[1] = value;
	if(HAL_I2C_Master_Transmit(hi2c, I2C_ADDRESS, data, 2, 10) != HAL_OK){
		return false;
	}
	
	return true;
}

bool mpu6050_init(I2C_HandleTypeDef *i2c){
	uint8_t regValue;
	hi2c = i2c;
	
	//Try to read register "Who am I". It must return value 0x68
	if(!mpu6050_readReg(REG_WHO_AM_I, &regValue))
		return false;
	
	if(regValue != 0x68)
		return false;
	
	mpu6050_writeReg(REG_PWR_MGMT_1, 0);
	
	return true;
}

bool mpu6050_getAccelRowData(mpu6050AccelRowData *rowData){
	uint8_t data[6];
	
	if(!mpu6050_readData(REG_ACCEL_XOUT, data, 6))
		return false;
	
	rowData->x = (data[0] << 8) | data[1];
	rowData->y = (data[2] << 8) | data[3];
	rowData->z = (data[4] << 8) | data[5];
	
	return true;
}

bool mpu6050_getAccelOffset(mpu6050AccelRowData *rowData){
	uint8_t data[6];
	
	if(!mpu6050_readData(REG_ACCEL_XOFF, data, 6))
		return false;
	
	rowData->x = (data[0] << 8) | data[1];
	rowData->y = (data[2] << 8) | data[3];
	rowData->z = (data[4] << 8) | data[5];
	
	return true;
}

bool mpu6050_setAccelOffset(mpu6050AccelRowData *rowData){
	uint8_t data[6];
	
	data[0] = (rowData->x >> 8) & 0xFF;
	data[1] = rowData->x & 0xFF;
	data[2] = (rowData->y >> 8) & 0xFF;
	data[3] = rowData->y & 0xFF;
	data[4] = (rowData->z >> 8) & 0xFF;
	data[5] = rowData->z & 0xFF;
	
	if(!mpu6050_writeData(REG_ACCEL_XOFF, data, 6))
		return false;
	
	return true;
}

bool mpu6050_setAccelScale(uint8_t scale){
	uint8_t regValue;
	
	if(scale > MPU6050_ACCEL_SCALE_16G)
		return false;
	
	if(!mpu6050_readReg(REG_ACCEL_CONFIG, &regValue))
		return false;
	
	regValue &= 0xE7;
	regValue |= (scale << 3);
	
	if(!mpu6050_writeReg(REG_ACCEL_CONFIG, regValue))
		return false;
	
	return true;
}

bool mpu6050_getAccelScale(uint8_t *scale){
	uint8_t regValue;
	if(!mpu6050_readReg(REG_ACCEL_CONFIG, &regValue))
		return false;
	
	*scale = ((regValue >> 3) & 0x03);
	
	return true;
}


bool mpu6050_getAccelDataFromRow(mpu6050AccelData *data, mpu6050AccelRowData *rowData, uint8_t scale){
	float koef;
	if(scale > MPU6050_ACCEL_SCALE_16G)
		return false;
	
	switch(scale){
		case MPU6050_ACCEL_SCALE_2G:
			koef = 2;
			break;
		case MPU6050_ACCEL_SCALE_4G:
			koef = 4;
			break;
		case MPU6050_ACCEL_SCALE_8G:
			koef = 8;
			break;
		case MPU6050_ACCEL_SCALE_16G:
			koef = 16;
			break;
	}
	koef /= 0x7FFF;
	
	data->x = rowData->x*koef;
	data->y = rowData->y*koef;
	data->z = rowData->z*koef;
	
	return true;
}

bool mpu6050_getGyroRowData(mpu6050GyroRowData *rowData){
	uint8_t data[6];
	
	if(!mpu6050_readData(REG_GYRO_XOUT, data, 6))
		return false;
	
	rowData->x = (data[0] << 8) | data[1];
	rowData->y = (data[2] << 8) | data[3];
	rowData->z = (data[4] << 8) | data[5];
	
	return true;
}

bool mpu6050_turnOnAccelSelfTest(uint8_t flag){
	uint8_t regValue;
	uint8_t mask;
	
	if(!mpu6050_readReg(REG_ACCEL_CONFIG, &regValue))
		return false;
	
	switch(flag){
		case MPU6050_SELF_TEST_FLAG_X:
			mask = 0x01;
			break;
		case MPU6050_SELF_TEST_FLAG_Y:
			mask = 0x02;
			break;
		case MPU6050_SELF_TEST_FLAG_Z:
			mask = 0x04;
			break;
		default:
			mask = 0x07;
			break;
	}
	
	regValue |= (mask << 5);
	
	if(!mpu6050_writeReg(REG_ACCEL_CONFIG, regValue))
		return false;
	
	return true;
}

bool mpu6050_turnOffAccelSelfTest(void){
	uint8_t regValue;
	
	if(!mpu6050_readReg(REG_ACCEL_CONFIG, &regValue))
		return false;
	
	regValue &= 0x1F;
	
	if(!mpu6050_writeReg(REG_ACCEL_CONFIG, regValue))
		return false;
	
	return true;
}
