#include "SD.h"

void sd_init(void);
void sd_deinit();
void SDPrintFresult(FRESULT fr) {
  char buff[32];
  switch (fr) {
    case FR_OK:
    WriteText("OK\n\r");
    break;
    case FR_DISK_ERR:
    WriteText("FR_DISK_ERR\n\r");
    break;
    case FR_INT_ERR:
    WriteText("FR_INT_ERR\n\r");
    break;
    case FR_NOT_READY:
    WriteText("FR_NOT_READY (SD NOT PLUGGED IN?)\n\r");
    break;
    case FR_NO_FILE:
    WriteText("FR_NO_FILE\n\r");
    break;
    case FR_NO_PATH:
    WriteText("FR_NO_PATH\n\r");
    break;
    case FR_INVALID_NAME:
    WriteText("FR_INVALID_NAME\n\r");
    break;
    case FR_EXIST:
    WriteText("FR_EXIST\n\r");
    break;
    case FR_INVALID_OBJECT:
    WriteText("FR_INVALID_OBJECT\n\r");
    break;
    case FR_WRITE_PROTECTED:
    WriteText("FR_WRITE_PROTECTED\n\r");
    break;
    case FR_INVALID_DRIVE:
    WriteText("FR_INVALID_DRIVE\n\r");
    break;
    case FR_NOT_ENABLED:
    WriteText("FR_NOT_ENABLED\n\r");
    break;
    case FR_NO_FILESYSTEM:
    WriteText("FR_NO_FILESYSTEM\n\r");
    break;
    case FR_MKFS_ABORTED:
    WriteText("FR_MKFS_ABORTED\n\r");
    break;
    case FR_TIMEOUT:
    WriteText("FR_TIMEOUT\n\r");
    break;
    case FR_LOCKED:
    WriteText("FR_NOT_ENOUGH_CORE\n\r");
    break;
    case FR_NOT_ENOUGH_CORE:
    WriteText("FR_NOT_ENOUGH_CORE\n\r");
    break;
    case FR_TOO_MANY_OPEN_FILES:
    WriteText("FR_TOO_MANY_OPEN_FILES\n\r");
    break;
    default:
    sprintf(buff, "Unknown Error Code: %i", fr);
    WriteText(buff);
  }
}

void indemalloc(char** arr, uint8_t idx, uint8_t len) {
  if (arr[idx] == 0x0) {
    arr[idx] = (char*)malloc(len* sizeof(char));
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
void sd_init(void) {
    FRESULT fr;
    fr = f_mount(&fs, "", 0);
    #if SD_DEBUG == 1
    SDPrintFresult(fr);
    #endif
}

void sd_deinit() {
    f_mount(0, "", 0);
}

#define MAX_FILE_COUNT 12

char** SDMallocFilenames() {
    char** filenames = (char**)calloc(MAX_FILE_COUNT,sizeof(char*)); // max 128 filenames for now
    return filenames;
}
void SDFreeFilenames(char** filenames) {
  int i = 0;
  for (i = 0; i < MAX_FILE_COUNT; i ++ ) {
    // if (filenames[i] == 0x00) break;
    free(filenames[i]);
    filenames[i] = 0x00;
  }
  free(filenames);
}


uint8_t SDGetAllFiles(char** result) {
  int stack_top = 0;
  char **allDirs = SDMallocFilenames();
  uint8_t allDirsCount = SDGetDirectories("/", allDirs);
  char output[30];
  uint8_t i = 0, newFilesCount = 0, j = 0;
  char **thisDir = SDMallocFilenames();
  for (i = 0; i < allDirsCount; i += 1) {
    newFilesCount = SDGetFiles(allDirs[i], thisDir);
    for (j = 0; j < newFilesCount; j += 1) {
      if (thisDir[j][0] == '-') {
        indemalloc(result, stack_top, 16);
        sprintf(result[stack_top], "%s/%s", allDirs[i], thisDir[j] + 2);
        SDCleanPath(result[stack_top]);
        stack_top += 1;
      }
    }
  }
  SDFreeFilenames(allDirs);
  SDFreeFilenames(thisDir);
  return stack_top;
}

uint8_t SDGetAllFilesandDirs(char** result,char**allDirs) {
  int stack_top = 0;
  uint8_t allDirsCount = SDGetDirectories("/", allDirs);
  char output[30];
  uint8_t i = 0, newFilesCount = 0, j = 0;
  char **thisDir = SDMallocFilenames();
  for (i = 0; i < allDirsCount; i += 1) {
    newFilesCount = SDGetFiles(allDirs[i], thisDir);
    for (j = 0; j < newFilesCount; j += 1) {
      if (thisDir[j][0] == '-') {
        indemalloc(result, stack_top, 16);
        sprintf(result[stack_top], "%s/%s", allDirs[i], thisDir[j] + 2);
        SDCleanPath(result[stack_top]);
        stack_top += 1;
      }
    }
  }
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
        SDCleanPath(buff);
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
    ff_DIR dir;
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
          if (fno.fattrib & AM_DIR && (
              (fno.fname[0] == '.' && fno.fname[1] == '\0') ||
              (fno.fname[0] == '.' && fno.fname[1] == '.' && fno.fname[2] == '\0')
            ) ) {
            f_readdir(&dir, &fno);
            continue;
        }
          indemalloc(result, i, 16);
          strcpy(result[i] + 2, (char*)&fno.fname);
          if (fno.fattrib & AM_DIR) {
              result[i][0] = 'd';
          } else {
              result[i][0] = '-';
          }
          result[i][1] = ' ';
          SDCleanPath(result[i]);

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

// Pete's SD module does not like multiple slashes (eg //path)
// this replaces all multiple slashes with single ones
// also makes everything lowercase for no good reason
void SDCleanPath(char *path) {
  int i = 0, j = 0;
  char buff[64];
  while(path[i] != '\0') {
    if (path[i] == '/') {
      buff[j] = '/';
      while (path[i] == '/') i++;
      j++;
      continue;
    }
    else if (path[i] >= 'A' && path[i] <= 'Z') {
      buff[j] = (char)((uint8_t)path[i] + 32);
    }
    else {
      buff[j] = path[i];
    }
    j++;
    i++;
  }
  buff[j] = '\0';

  strcpy(path, buff);
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
  sd_deinit();
  return nRead;
}

uint32_t SDGetFileSize(char *path) {
  sd_init();
  FIL fil;
  SDPrintFresult(f_open(&fil, path, FA_READ));
  DWORD fsize = f_size(&fil);
  f_close(&fil);
  sd_deinit();
  return (uint32_t) fsize;

}


unsigned int SD_READ(FIL* fil,uint32_t* buf,uint32_t bufSize)
{
  unsigned int count;
  // NVIC_DisableIRQ(I2S_IRQn);//Disable i2s interrupts
  // CS_LOW();//Enable chip select
  f_read(fil,buf,bufSize, &count);
  // CS_HIGH();//Disable chip select
  // NVIC_EnableIRQ(I2S_IRQn);
  //enable i2s interrupts
  return count;
}

//FRESULT f_write (FIL*, const void*, UINT, UINT*);   /* Write data to a file */

unsigned int SD_WRITE(FIL* fil,uint32_t* buf,uint32_t bufSize)
{
  unsigned int count;
  // NVIC_DisableIRQ(I2S_IRQn);//Disable i2s interrupts
  // CS_LOW();//Enable chip select
  f_write(fil,buf,bufSize, &count);
  // CS_HIGH();//Disable chip select
  // NVIC_EnableIRQ(I2S_IRQn);
  //enable i2s interrupts
  return count;
}
