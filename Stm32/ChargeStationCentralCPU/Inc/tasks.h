// Tags for tasks

#define TASK_TAG_RFID 1

typedef struct __GeneralMessageType
{
	uint8_t sourceTag;
	uint8_t messageId;
	uint32_t param1;
} GeneralMessage;

