#include "SD.h"

FATFS sd_init(void);

void sd_deinit(fs);

// FRESULT scan_files (
//     char* path        /* Start node to be scanned (***also used as work area***) */
// )
// {
//     FRESULT res;
//     DIR dir;
//     UINT i;
//     static FILINFO fno;

//     // WriteText("_");
//     res = f_opendir(&dir, path);                       /* Open the directory */
//     if (res == FR_OK) {
//         for (;;) {
//             // WriteText(".");
//             res = f_readdir(&dir, &fno);                   /* Read a directory item */
//             if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
//             if (fno.fattrib & AM_DIR) {                    /* It is a directory */
//                 i = strlen(path);
//                 sprintf(&path[i], "/%s", fno.fname);
//                 res = scan_files(path);                    /* Enter the directory */
//                 if (res != FR_OK) break;
//                 path[i] = 0;
//             } else {
//                 int n;
//                 char buffer [250];                                      /* It is a file. */
//                 n = sprintf(buffer,"%s/%s\n\r", path, fno.fname);
//                 WriteText(buffer);
//             }
//         }
//         f_closedir(&dir);
//     } else {
//         char err[20];
//         sprintf(err, "%lu\n\r", FR_OK);
//         WriteText(err);
//     }

//     return res;
// }

void PrintFresult(FRESULT fr) {
    if (fr == FR_OK) {
        WriteText("OK\n\r");
    } else if (fr == FR_NO_FILESYSTEM) {
        WriteText("SD FILESYSTEM CORRUPTED\n\r");
    } else if (fr == FR_NOT_READY) {
        WriteText("SD NOT PLUGGED IN\n\r");
    } else { // unrecognised error, just print the code
        char err[30];
        sprintf(err, "Error code: %d\n\r", fr);
        WriteText("SD NOT PLUGGED IN\n\r");
    }
}

// crash while card is mounted corrupts filesystem
// card is mounted and unmounted for each operation to minimise risk
FATFS sd_init(void) {
    FATFS fs;
    FRESULT fr;
    fr = f_mount(&fs, "", 1);
    #if DEBUG == 1
    PrintFresult(fr);
    #endif
    return fs;
}
void sd_deinit(fs) {
    f_mount(0, "", 0);
}

#define MAX_FILE_COUNT 32

char** MallocFilenames() {
    char** filenames = (char**)malloc(32*sizeof(char*)); // max 128 filenames for now
    int i = 0;
    for (i = 0; i < MAX_FILE_COUNT; i ++ ) {
        filenames[i] = (char*)malloc(14 * sizeof(char));
    }
    return filenames;
}
void FreeFilenames(char** filenames) {
    int i = 0;
    for (i = 0; i < MAX_FILE_COUNT; i ++ ) {
        free(filenames[i]);
    }
    free(filenames);
}

int SDGetFiles(char** result) { // right now eagerly preallocates, might change later
    sd_init();
    
    

    DIR dir;
    char path[2] = "/";
    char buff[64];
    FRESULT res = f_opendir(&dir, path);
    #if DEBUG == 1
    WriteText("Reading Files: ");
    PrintFresult(res);
    #endif
    int i = 0;
    FILINFO fno;
    if (res == FR_OK) {
        res = f_readdir(&dir, &fno);
        PrintFresult(res);
        
        while(fno.fname[0] != 0 && i < MAX_FILE_COUNT) {
            strcpy(result[i], (char*)&fno.fname);
            // WriteText(result[i]);
            f_readdir(&dir, &fno);
            i += 1;
        }
        #if DEBUG == 1
        if (i >= 128) {
            sprintf(buff, "Too many files in directory; only first %d were returned.", MAX_FILE_COUNT);
            WriteText(buff);
        }
        #endif
    }
    // sprintf(buff, "\n\r%d\n\r", i);
    // WriteText(buff);
    sd_deinit();
    return i;
}



