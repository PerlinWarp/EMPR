#include "SD.h"

FATFS sd_init(void);

void sd_deinit();
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
    char buff[20];

void indemalloc(char** arr, uint8_t idx, uint8_t len) {
  if (arr[idx] == 0x0) {
    arr[idx] = (char*)NewMalloc(len* sizeof(char));
    #if SD_DEBUG == 1
      if (arr[idx] == 0x0) {
        WriteText("you ran out of heap my dude\n\r");
      }
    #endif
    arr[idx][0] = '\0';
  }
}

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

#define MAX_FILE_COUNT 12

char** SDMallocFilenames() {
    char** filenames = (char**)NewCalloc(MAX_FILE_COUNT,sizeof(char*)); // max 128 filenames for now
    return filenames;
}
void SDFreeFilenames(char** filenames) {
  int i = 0;
  for (i = 0; i < MAX_FILE_COUNT; i ++ ) {
    // if (filenames[i] == 0x00) break;
    NewFree(filenames[i]);
    filenames[i] = 0x00;
  }
  NewFree(filenames);
}


uint8_t SDGetAllFiles(char** result) {
  int stack_top = 0;
  char **allDirs = SDMallocFilenames();
  uint8_t allDirsCount = SDGetDirectories("/", allDirs);
  uint8_t i = 0, newFilesCount = 0, j = 0;
  char **thisDir = SDMallocFilenames();
  for (i = 0; i < allDirsCount; i += 1) {
    newFilesCount = SDGetFiles(allDirs[i], thisDir);
    for (j = 0; j < newFilesCount; j += 1) {
      if (thisDir[j][0] == '-') {
        indemalloc(result, stack_top, 16);
        sprintf(result[stack_top], "%s/%s", allDirs[i], thisDir[j] + 2);
        stack_top += 1;
      }
    }
  }
  SDFreeFilenames(allDirs);
  SDFreeFilenames(thisDir);
  return stack_top;
}


uint8_t SDGetDirectories(char *path, char** result) {
	int stack_begin = 0;
	int stack_len = 1;
  indemalloc(result, 0, 16);
  strcpy(result[0], path);

	int i;
	char buff[64];
	char** thisDir = SDMallocFilenames();
	while(stack_begin < stack_len) {
		int thisDirLen = SDGetFiles(result[stack_begin], thisDir);

		for (i=0; i<thisDirLen; i++) {
			if (thisDir[i][0] == 'd') {
				sprintf(buff, "%s/%s", result[stack_begin], thisDir[i] + 2);
        indemalloc(result, stack_len, 16);
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

        while(fno.fname[0] != 0 && i < MAX_FILE_COUNT) {
          indemalloc(result, i, 16);
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
// read at most n bytes from path and store them in result, returning how many were actually read
uint8_t SDReadBytes(char* path, BYTE* result, uint8_t n) {
    NVIC_DisableIRQ(I2S_IRQn);//Disable i2s interrupts
  sd_init();

  FIL fil;


  SDPrintFresult(f_open(&fil, path, FA_READ));

  UINT nRead = 0;
  SDPrintFresult(f_read(&fil, result, n, &nRead));

  f_close(&fil);
  sd_deinit(fs);
  return nRead;
}

uint32_t SDGetFileSize(char *path) {
  sd_init();
  FIL fil;
  SDPrintFresult(f_open(&fil, path, FA_READ));
  FSIZE_t fsize = f_size(&fil);
  f_close(&fil);
  sd_deinit(fs);
  return (uint32_t) fsize;

}

unsigned int SD_READ(FIL* fil,uint32_t* buf,uint32_t bufSize)
{
  unsigned int count;
  NVIC_DisableIRQ(I2S_IRQn);//Disable i2s interrupts
  // CS_LOW();//Enable chip select
  f_read(fil,buf,bufSize, &count);
  // CS_HIGH();//Disable chip select
  NVIC_EnableIRQ(I2S_IRQn);
  //enable i2s interrupts
  return count;
}

unsigned int SD_WRITE(FIL* fil,uint32_t* buf,uint32_t bufSize)
{
  unsigned int count;
  NVIC_DisableIRQ(I2S_IRQn);//Disable i2s interrupts
  // CS_LOW();//Enable chip select
  f_write(fil,buf,bufSize, &count);
  // CS_HIGH();//Disable chip select
  NVIC_EnableIRQ(I2S_IRQn);
  //enable i2s interrupts
  return count;
}
