#pragma once

//
typedef char CiString20Type[21];
typedef char CiString25Type[26];

struct RequestBootNotificatation {
   CiString20Type chargePointModel;
   CiString20Type chargePointVendor;
};

// Messages
#define ACTION_BOOT_NOTIFICATION 0

const char *getActionString(int action);
