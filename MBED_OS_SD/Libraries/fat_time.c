/****************************************************************/
/*					  			*/
/* Time support for the fat file system, 			*/
/*					  			*/
/****************************************************************/
#include "stdio.h"
#include "types.h"
#include "stdlib.h"
#include "ff.h"
#include "errno.h"			// System error numbers
//#include "kernel.h"

DWORD get_fattime (void)
{
	return(fat_time_get());
}

