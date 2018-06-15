#include "RFID.h"
#include "RC522.h"

bool RFID_init(SPI_HandleTypeDef *hspi){
	RC522InitType rc522Init;
	rc522Init.hspi = hspi;
	rc522Init.isHardwareChipSelect = true;
	//rc522Init.chipSelectPort = SPI_CS_GPIO_Port;
	//rc522Init.chipSelectPin = SPI_CS_Pin;
	
	MFRC522_Init(&rc522Init);
	
	return false;
}
