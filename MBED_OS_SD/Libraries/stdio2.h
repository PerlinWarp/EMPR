#ifndef __STDIO_H
#define __STDIO_H

#include "config.h"					// Base system config flags, as setup with 'make menu'

/****************************************************************/
/*					  			*/
/* Base definitions for the OS and user code			*/
/*					  			*/
/****************************************************************/
//#define		NULL		'\0'
#define		EOF		0x7f			// Not -1, as we are using 7-bit ascii at the mo
#define 	true		 1			// Base def of TRUE
#define 	false   	 0			// Base def of FALSE
#define 	TRUE		 true
#define 	FALSE		 false
#define 	NL		 0x0a			// Basic ASCII New Line
#define		CR		 0x0d			// Basic ASCII Caridge-return
#define		BS		 0x08			// Basic ASCII back space
#define		ESC		 0x1b			// Send an escape char
/****************************************************************/
/*					  			*/
/* Basic file open modes flags we have				*/
/*					  			*/
/****************************************************************/
#define		O_RDONLY	0x01			// Read only
#define		O_WRONLY	0x02			// Write only
#define		O_RDWR		O_RDONLY | O_WRONLY	// Read and Write, Read and write flags macro....
#define		O_APPWR		0x04			// Append flag, Probally only used on the file systems

#define		O_FILE		0x10			// Additional flags to base create, for a file
#define		O_DIR		0x20			// Additional flags to base create, for a directory
#define		O_MNT		0x40			// Additional flags to base create, for a mount point

#define putchar(c) putc(c,stdout)			// Simple string macro to help early doors

/****************************************************************/
/*					  			*/
/* user side FILE pointer used by the STDIO system, links	*/
/* down in to all the kernel device drivers, serial can I2c etc	*/
/*					  			*/
/****************************************************************/
typedef struct __iobuf {
	int		fd;				// Base file descriptor, used to map data in kernel and user space
	int		flags;				// Flags holding driver state for the user and open mode, ets
	char		name[CONFIG_FILE_NAME_SIZE];	// File Name passed to driver
	int		bufsiz;				// If we have buffers attached, in the kernel, this tells how big they are
	unsigned char	*buf;				// Kernel data buffer
	unsigned char	*ptr;				// pointer into kernel data buffer
} FILE;
FILE	*FILES;						// Basic file pointer array, currently set as a config variable
extern FILE	*stdin,*stdout,*stderr;			// Primary file descriptors, hooked on to the console
/****************************************************************/
/*					  			*/
/* Real time clock data structure, passed through the IO	*/
/* system, defined here						*/
/*					  			*/
/****************************************************************/
// control block for the RTC I/O commands
typedef struct _rtc_reg {
	int		year;				// 1..4095
	unsigned char	month;				// 1..12 
	unsigned char	mday;				// 1.. 31
	unsigned char	wday;				// 1..7 
	unsigned char	hour;				// 0..23 
	unsigned char	min;				// 0..59 
	unsigned char	sec;				// 0..59 
} RTC;
extern RTC 	*rtc_data_block;			// Once setup this pointer should be the transfer buffer

/****************************************************************/
/*					  			*/
/* File stat data, Note the flags should be an extended version */
/* of what we have defined currently as of Oct 2013, so we can 	*/
/* have flags for all the filesystems represented by the 32 	*/
/* bits of the data						*/
/*					  			*/
/****************************************************************/

typedef struct __stat {
	int		st_dev;     			// ID of device containing file 
	int		st_rdev;    			// device ID (if special file) 
	int		st_blk;    			// Block Number
	int		st_dir;    			// Directory Number/Block
	int		st_lbo;				// Last but one block, used so create will work
	int		st_elements;			// Breakup elements
	char		st_file[15];			// File name the stat call has looked at
	int		st_size;    			// total size, in bytes 
	int		st_flags;			// Files flags, 
}STAT;
/****************************************************************/
/*					  			*/
/* Types used with the above stat data				*/
/*					  			*/
/****************************************************************/
enum {
	S_IFREG = 1,					// Regular File
	S_IFDIR,					// Directory	
	S_IFBDIR,					// Roots Bin Directory	(Extra to MbedOS)
	S_IFDDIR,					// Roots Dev Directory	(Extra to MbedOS)
	S_IFMDIR,					// Mount PointDirectory	(Extra to MbedOS)
	S_IFCHR,					// Charactor Device
	S_IFBLK,					// Block Device
	S_IFPRT,					// GPIO Port Device	(Extra to MbedOS)
	S_IFI2C,					// I2C Bus Device	(Extra to MbedOS)
	S_IFCMD,					// Command Found	(Extra to MbedOS)
	S_IFNON,					// Nothing there
	S_IFRNON,					// Flag for more than one level out
}STAT_TYPE;
/****************************************************************/
/*					  			*/
/* Data types and defs for the Schedular code			*/
/*					  			*/
/****************************************************************/
#ifdef CONFIG_SHED
typedef struct _SHED_TASK {
	char		task_no;			// Used for process listing only
	char		name[CONFIG_SHED_TASK_NAME_SIZE];	// Task Name
	int		flags;				// Task Flags
	long		tick;				// Ticks between task runs
	void		(*task)(void);			// Task routine pointer
	char	 	path[CONFIG_MAX_PATH];		// exec path if the schedualed task is a script
}shed_task;
extern shed_task 	*shed_task_data_block;

#define		SHED_EMPTY	-1			// No task setup on this slot
#define		SHED_RUN	0x0001			// Just keep running task
#define		SHED_RUN_ONCE	0x0002			// Run task once, then pause
#define		SHED_PAUSED	0x0004			// Task Paused
#define		SHED_SCRIPT	0x0100			// Tell the code its a script file to run

#endif
/****************************************************************/
/*					  			*/
/* Global file flags, local drivers should munge there file 	*/
/* types back onto these					*/
/*					  			*/
/****************************************************************/
#define	FILE_READABLE		0x0001			// We can read this file,
#define	FILE_WRITEABLE		0x0002			// We can write to this file
#define	FILE_EXICUTABLE		0x0004			// We can exicute data in this file
#define	FILE_DIRECTORY		0x0008			// this is a directory
#define	FILE_TYPE_SH		0x0010			// This is a script type file
#define	FILE_TYPE_BAS		0x0020			// This is a file that can be passed to the basic interpriter
#define	FILE_TYPE_TXT		0x0040			// This is a script type file
#define FILE_TYPE_BIN		0x0100			// From the system bin dir
#define	FILE_TYPE_DEV		0x0200			// From the system device driver dir
#define	FILE_TYPE_MOUNT		0x0400			// Mount point
#define	FILE_TYPE_CHAR		0x1000			// Charactor device file
#define	FILE_TYPE_BLOCK		0x2000			// Block device file
#define	FILE_TYPE_PAR		0x4000			// port device file
/****************************************************************/
/*					  			*/
/* Data passed from kernel to user about a file system status	*/
/*					  			*/
/****************************************************************/
typedef struct _disk_stat {
	long	dsk_free;				// Free Bytes on volume
	long	dsk_used;				// Bytes used
	long	dsk_clusters;
	int	dsk_flags;
} DISK_STAT;
extern DISK_STAT *sys_disk_stat;			// returned disk data

/****************************************************************/
/*					  			*/
/* IOCTL's known to the system are all here at the mo		*/
/*					  			*/
/****************************************************************/
#define TTYCLS	LCD_CLEAR			// Mapping for a clear on the console line
enum {
	SET_TX_BlOCKING = 0,			// 0 IOCTL's used for the serial lines
	SET_RX_BLOCKING,			// 1
	RESET_TX_BlOCKING,			// 2
	RESET_RX_BLOCKING,			// 3

	SET_SERIAL_SPEED,			// 4 two ioctl's that can be used in I2C/rs232/CAN to 
	GET_SERIAL_SPEED,			// 5 set the speed of the device
	
	CAN_SET_BIT,				// 6 Set and Reset the can RS bit on the driver chip
	CAN_RESET_BIT,				// 7
	
	DEVICE_NONE_BLOCKING,			// 8 Basic call for blocking, or none blocking on
	DEVICE_BLOCKING,			// 9 serial devices
	
	LCD_CLEAR,				// 10 IOCTL's used for the Batron 2*16 I2C LCD display
	LCD_HOME,				// 11 and used for the MIDAS 2*16 I2C LCD display
	LCD_POS,				// 12 Set the X/Y position on the LCD
	LCD_TEST,				// 13 Send out some test string to the LCD
	
	SET_MEMORY_ADDRESS,			// 14 Set a memory address pointer in the memory driver
	
	GPIO_SETBIT,				// 15 Set the selected GPIO bit
	GPIO_RESETBIT,				// 16 Clear the selected GPIO bit	
	GPIO_IN,				// 17 PCA9534 registers, but can be used else where
	GPIO_OUT,				// 18 Input, Output reg
	GPIO_POL,				// 19 Invert Input Bits, Reg
	GPIO_CONF,				// 20 Device Config reg
	GPIO_ALL,				// 21 Read All of the setup
	GPIO_DISPLAY,				// 22 normally used for debugging I/O
	
	SHED_SET_TICKS,				// 23 Reset task ticks via IOCTL
	SHED_SET_FLAGS,				// 24 Reset task flags via IOCTL
	SHED_ADD_TASK,				// 25 Add a new task to the stack via IOCTL
	SHED_DEL_TASK,				// 26 Clear out a task via IOCTL
	SHED_GET_TASK,				// 27 Get the task data via IOCTL (PS should live of this)
	SHED_EXTRA1,				// 28 Scratch ioctl
	SHED_EXTRA2,				// 29 Scratch ioctl
	
	BLOCK_WRITE,				// 30 Block Read and write IOCTL
	BLOCK_READ,				// 31 used for the file system code
	
	FS_GET_SIZE,				// 32 Used if the file systems to get disk sizes
	GPIO_SET_KBD,				// 33, Used in the PCF8574 driver to flag device as keyboard
	
	FS_SET_MOUNT_DEVICE,			// 34, Used to set the minor device number on the root FS mount point
	FS_SET_FLAGS,				// 35, Used the set the flags on a file in the file system, value passed
						// 	in should be the global state of the flags, and the driver should 
						//	re-map them to the file systems format
	FS_DISPLAY_TABLES,			// 36, Used to display the data tables in file system device drivers
	FS_DISPLAY_TABLES_LONG,			// 37, Used to display the data tables in file system device drivers, 
						//     long format, long format, long format
	
	GPIO_RESET,				// 38 general reset call for the GPIO
	
	FS_FORMAT,				// 39, Pass the format command to the driver as an IOCTL
	FS_DISPLAY_SECTOR,			// 40, Display a sector of a file system back to user
	FS_DISPLAY_SECTOR_HEX,			// 41, same as above, but add a hex dump of the sector to dump
	FS_DUMP_SECTOR,				// 42, Dump the whole sector in a hex dump
	FS_DISPLAY_DRIVER_DATA,			// 43, Dump the driver details for a device
	
	GPIO_ADDBIT,				// 44, Add a new GPIO bit to the I/O driver
	GPIO_DELBIT,				// 45, Remove a GPIO bit from the I/O driver
	GPIO_GETBIT,				// 46, Get the pointer from the GPIO name the I/O driver
	GPIO_SHOWBIT,				// 47, Show the GPIO bits defined in the driver
	
	STEP_TIME,				// 48, Timming for the stepper motor
	STEP_CLOCK,				// 49, Turn the stepper clockwise
	STEP_ANTI,				// 50, Turn the stepper anti clockwise
	STEP_HOME,				// 51, Step the stepper to the home position
	STEP_DRIVER,				// 52, Link the right driver to the main stepper motor code
	
	GPIO_RESET_KBD,				// 53, Need to be able to reset the keyboard bit
	IOCTL_CHANNEL0,				// 54, Set channel 0 on device				
	IOCTL_CHANNEL1,				// 55, Set channel 1 on device	
	IOCTL_CHANNEL2,				// 56, Set channel 2 on device	
	IOCTL_CHANNEL3,				// 57, Set channel 3 on device
	IOCTL_CHANNEL4,				// 58, Set channel 4 on device	
	IOCTL_CHANNEL5,				// 59, Set channel 5 on device	
	IOCTL_CHANNEL6,				// 60, Set channel 6 on device	
	IOCTL_CHANNEL7,				// 61, Set channel 7 on device	
	IOCTL_CHANNEL8,				// 62, Set channel 8 on device
	IOCTL_CHANNEL9,				// 63, Set channel 9 on device
	IOCTL_CHANNEL10,			// 64, Set channel 10 on device
	
	GET_DEV_LOCAL,				// 65, Return the drivers local data value back to user
	
};
/****************************************************************/
/*					  			*/
/* Read line return values, used in the shell			*/
/*					  			*/
/****************************************************************/
enum {
	READ_LINE_NULL = 0,			// So the New Line is not 0
	READLINE_NEWLINE_FOUND,			// Default return from readline
	READLINE_EOF_FOUND,			// End of file found
	READLINE_BUFFER_OVERRUN,		// Oh dear we are about to over-run the end of the buffer
	READLINE_READ_ERROR			// Don't want this, but read returned an error
};
/****************************************************************/
/*					  			*/
/* Some basic system defaults, should move out in time		*/
/*					  			*/
/****************************************************************/
#define		FOPEN_MAX		CONFIG_NFILES_SIZE	// Max open files
#define 	SYSTEM_STATE_RUNNING	0x00000001		// State flags for the OS

/****************************************************************/
/*					  			*/
/* STDIO bits for the GPIO pins					*/
/*					  			*/
/****************************************************************/
#ifdef CONFIG_GPIO_NEW
typedef struct _gpio_pin {
	unsigned char	port;				// Port the pin is on
	unsigned char	pin;				// Pin on port
	unsigned char	mode;				// Mode the pin is set to
	char		name[CONFIG_GPIO_NAME_SIZE];	// Name of the pin
} GPIO_PIN;
GPIO_PIN	*GPIO_PINS;				// Array of GPIO pins
GPIO_PIN	*user_gpio;				// User side pin data
#endif
#endif /* _STDIO_H */
