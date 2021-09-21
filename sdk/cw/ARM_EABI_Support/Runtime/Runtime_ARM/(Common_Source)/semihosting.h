#ifdef __thumb
	#define	SEMIHOSTING_SWI		0xAB
#else
	#define	SEMIHOSTING_SWI		0x123456
#endif

#define SYS_OPEN	0x01
#define SYS_CLOSE	0x02
#define	SYS_WRITEC	0x03
#define	SYS_WRITE0	0x04
#define SYS_WRITE	0x05
#define SYS_READ	0x06
#define	SYS_READC	0x07
#define SYS_ISERROR 0x08
#define SYS_ISTTY	0x09
#define SYS_SEEK	0x0a
// not used			0x0b
#define SYS_FLEN	0x0c
#define SYS_TMPNAM	0x0d
#define SYS_REMOVE	0x0e
#define SYS_RENAME	0x0f
#define	SYS_CLOCK	0x10
#define SYS_TIME	0x11
#define SYS_SYSTEM	0x12
#define SYS_ERRNO	0x13
// not used			0x14
#define SYS_GET_CMDLINE 0x15
#define SYS_HEAPINFO 0x16
#define SYS_ELAPSED	0x30
#define SYS_TICKFREQ 0x31

#define SYS_EXIT	0x18

#define SEMIHOST_CMDLINE_MAX 256

// Struct for fopen passed in r1 to debug agent
typedef struct {
	char * name;			// pointer to filename, null terminated
	int semihost_mode;		// file mode in semihost format
	int name_len;			// filename length excluding null character
} semihost_open_parms;

// Struct for __write_file, __read_file info passed in r1 to debug agent
typedef struct {
	int handle;				// file handle previously opened
	unsigned char * data;	// pointer to memory of data to be written
	int count;				// count of number of bytes to be written
} semihost_readwrite_parms;

// Struct for __position_file info passed in r1 to debug agent
typedef struct  {
	int handle;				// file handle previously opened
	unsigned long pos;		// absolute file position to seek to
} semihost_seek_parms;

// Struct for __temp_file_name info passed in r1 to debug agent
typedef struct {
	char *name;				// storage to host returned tmp name
	int targetid;			// unique target id for each tmp name 
	int len;				// size of storage, usually L_tmpnam
} semihost_tmpnam_parms;

// Struct for __rename_file info passed in r1 to debug agent
typedef struct {
	char *oldname;			// pointer to old file name
	int olen;				// length of old file name
	char *newname;			// pointer to new file name
	int nlen;				// length of new file name
} semihost_rename_parms;

// Struct for get cmd line info passed in r1 to debug agent
typedef struct {
	char *cmdline;			// storage for cmd line
	int len;				// max length of buffer (256)

} semihost_cmdline_parms;

// Struct for remove info passed in r1 to debug agent
typedef struct {
	char *name;				// storage for cmd line
	int len;				// max length of buffer (256)
} semihost_remove_parms;

// Struct for system() info passed in r1 to debug agent
typedef struct {
	char *cmd;				// system command to execute
	int len;				// length of command
} semihost_system_parms;

extern void sys_exit();