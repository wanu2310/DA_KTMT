#include "Logcat.h"

void LogI(const char *FuncName, String msg)
{
    Serial.println("[" + String(FuncName)+"] : " + msg);
}