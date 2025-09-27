#include "EffDebugLog.h"
#include "GMMemory.h"

void WriteDebug(char *strErr) {
	char *strDebugFile = "EffectErr.txt";
	FILE *fp = fopen(strDebugFile,"at+");
	fprintf(fp,"File Not Found : %s\n",strErr);
	fclose(fp);
}
