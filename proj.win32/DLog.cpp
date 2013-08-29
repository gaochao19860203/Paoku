#include "DLog.h"

static bool isOpen = true;

void DLog::showLog(const char* pszFormat, ...){
	if( !isOpen ){
		return;
	}

	printf("Cesc: ");
	char szBuf[kMaxLogLen];

	va_list ap;
	va_start(ap, pszFormat);
	vsnprintf(szBuf, kMaxLogLen, pszFormat, ap);
	va_end(ap);
	printf("%s", szBuf);
	printf("\n");
}
