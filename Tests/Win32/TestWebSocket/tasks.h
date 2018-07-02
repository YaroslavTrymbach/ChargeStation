// Tags for tasks

#define TASK_TAG_RFID   1
#define TASK_TAG_SERVER 2

typedef struct __GeneralMessageType
{
	uint8_t sourceTag;
	uint8_t messageId;
	uint32_t param1;
	char uniqueId[40];
} GeneralMessage;

