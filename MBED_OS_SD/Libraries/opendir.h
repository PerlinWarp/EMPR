// Data type returned per file entery
typedef struct __DIRENT{
	char		 name[0x10];		// file/dir name
	int		 status;		// file/dir ststus, RO/RW etc
	int		 length;		// File length
	int		 local;			// some local data
}DIRENT;

// Type returned when a directory is sucsesfully opened by the opendir call
typedef struct __DIR{
	DIRENT		*entery;		// entery pointed at
	int		 flags;			// Flags to say what state the call is in
	int		 cnt;			// Needed now and then
}DIR;

#define	DIRENT_FREE	0x00000000		// Clear flags when call not in use
#define DIRENT_BUSY	0x00000001		// Mark system as in use
#define DIRENT_INIT	0x00000002		// Needs Setting up
#define DIRENT_READY	0x00000004		// Marked as setup

DIR *opendir(char *path);
DIRENT *readdir(DIR *dirp);
int closedir(DIR *dirp);
int opendir_init(void);
