#include "SD.h"

FATFS sd_init(void);

void sd_deinit();

#if SD_DEBUG == 1
void SDPrintFresult(FRESULT fr) {
    if (fr == FR_OK) {
        WriteText("OK\n\r");
    } else if (fr == FR_NO_FILESYSTEM) {
        WriteText("SD FILESYSTEM CORRUPTED\n\r");
    } else if (fr == FR_NOT_READY) {
        WriteText("SD NOT PLUGGED IN\n\r");
    } else { // unrecognised error, just print the code
        char err[30];
        sprintf(err, "Error code: %d\n\r", fr);
        WriteText(err);
    }
}
#endif
// crash while card is mounted corrupts filesystem
// card is mounted and unmounted for each operation to minimise risk
FATFS sd_init() {
    FRESULT fr;
    fr = f_mount(&fs, "", 1);
    #if SD_DEBUG == 1
    SDPrintFresult(fr);
    #endif
    return fs;
}
void sd_deinit() {
    f_mount(0, "", 0);
}

#define MAX_FILE_COUNT 16

char** SDMallocFilenames() {
    char** filenames = (char**)malloc(MAX_FILE_COUNT*sizeof(char*)); // max 128 filenames for now
    int i = 0;
    for (i = 0; i < MAX_FILE_COUNT; i ++ ) {
        filenames[i] = (char*)malloc(16 * sizeof(char));
    }
    return filenames;
}
void SDFreeFilenames(char** filenames) {
    int i = 0;
    for (i = 0; i < MAX_FILE_COUNT; i ++ ) {
        free(filenames[i]);
    }
    free(filenames);
}

uint8_t SDGetDirectories(char *path, char** result) {
	int stack_begin = 0;
	int stack_len = 1;
	result[0] = path;

	int i;
	char buff[64];
	char** thisDir = SDMallocFilenames();
	while(stack_begin < stack_len) {
		int thisDirLen = SDGetFiles(result[stack_begin], thisDir);

		for (i=0; i<thisDirLen; i++) {
			if (thisDir[i][0] == 'd') {
				sprintf(buff, "%s/%s", result[stack_begin], thisDir[i] + 2);
				strcpy(result[stack_len], buff);
				stack_len++;
			}
		}

		stack_begin ++;
	}

	SDFreeFilenames(thisDir);

	return stack_len;
}

uint8_t SDGetFiles(char* path, char** result) {
    sd_init();
    DIR dir;
    char buff[64];
    FRESULT res = f_opendir(&dir, path);
    #if SD_DEBUG == 1
    WriteText("Reading Files: ");
    SDPrintFresult(res);
    #endif
    int i = 0;
    FILINFO fno;
    if (res == FR_OK) {
        res = f_readdir(&dir, &fno);
        SDPrintFresult(res);
        
        while(fno.fname[0] != 0 && i < MAX_FILE_COUNT) {
            strcpy(result[i] + 2, (char*)&fno.fname);
            if (fno.fattrib & AM_DIR) {
                result[i][0] = 'd';
            } else {
                result[i][0] = '-';
            }
            result[i][1] = ' ';
            
            f_readdir(&dir, &fno);
            i += 1;
        }
        #if SD_DEBUG == 1
        if (i >= MAX_FILE_COUNT) {
            sprintf(buff, "Too many files in directory; only first %d were returned.", MAX_FILE_COUNT);
            WriteText(buff);
        }
        #endif
    }
    sd_deinit();
    return i;
}

unsigned int SD_READ(FIL* fil,uint32_t* buf,uint32_t bufSize)
{
  unsigned int count;
  NVIC_DisableIRQ(I2S_IRQn);//Disable i2s interrupts
  CS_LOW();//Enable chip select
  f_read(fil,buf,bufSize, &count);
  CS_HIGH();//Disable chip select
  NVIC_EnableIRQ(I2S_IRQn);
  //enable i2s interrupts
  return count;
}

unsigned int SD_WRITE(FIL* fil,uint32_t* buf,uint32_t bufSize)
{
  unsigned int count;
  NVIC_DisableIRQ(I2S_IRQn);//Disable i2s interrupts
  CS_LOW();//Enable chip select
  f_write(fil,buf,bufSize, &count);
  CS_HIGH();//Disable chip select
  NVIC_EnableIRQ(I2S_IRQn);
  //enable i2s interrupts
  return count;
}
