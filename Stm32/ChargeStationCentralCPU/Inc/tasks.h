// Tags for tasks

#define TASK_TAG_RFID           1
#define TASK_TAG_SERVER         2
#define TASK_TAG_SERIAL_CONTROL 3
#define TASK_TAG_NET            4
#define TASK_TAG_USER_BUTTON    10

typedef struct __GeneralMessageType
{
	uint8_t sourceTag;
	uint8_t messageId;
	uint32_t param1;
	char uniqueId[40];
} GeneralMessage;

