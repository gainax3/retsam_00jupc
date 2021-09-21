/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CPS
  File:     cps.h

  Copyright 2001-2005 Ubiquitous Corporation.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Ubiquitous Corporation, and are protected
  by Federal copyright law.  They may not be disclosed to third parties
  or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Ubiquitous Corporation.

  $Revision: 1.17 $ $Date: 2006/02/22 11:23:37 $

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITROWIFI_CPS_H_
#define NITROWIFI_CPS_H_

#ifdef __cplusplus
extern "C" {
#endif

//
//	Basic typedef's
//
typedef u32 CPSInAddr;
typedef u8 CPSMacAddress[6];

//
//	Buffer definition
//
typedef struct {
	u32 size;
	u8 *data;
} CPSSocBuf;

//
//	Socket definition
//
typedef struct _CPSSoc {
	OSThread *thread;			// Owner of this socket
	u32 block_type;
	u8 state;					// socket state
	u8 ssl;						// TCP if 0, SSL otherwise
	u16 local_port;				// my port number
	void *con;					// SSL connection information
	u32 when;					// when this was created (SYN sent/recvd)
	u32 local_ip_real;			// real IP that tha packet is sent to
	u16 remote_port;			// his port number
	u16 remote_port_bound;		// port number specified by CPS_SocBind()
	CPSInAddr remote_ip;		// his IP address
	CPSInAddr remote_ip_bound;	// IP address specified by CPS_SocBind()
	u32 ackno;					// my ACK number (i.e., received so far)
	u32 seqno;					// my SEQ number (i.e., sent so far)
	u16 remote_win;				// his window size
	u16 remote_mss;				// his MSS
	u32 remote_ackno;			// his ACK number (i.e., acked so far)
	u32 ackrcvd;				// # of ACKs received so far (for slow start)
	int (*udpread_callback)(u8 *, u32, struct _CPSSoc *);
	CPSSocBuf rcvbuf;			// Low level receive buffer
								// (filled by TCP state machine or UDP)
	u32 rcvbufp;				// index to rcvbuf.data[] (0 based)
	CPSSocBuf sndbuf;			// Low level send buffer (used by packet sender)
	CPSSocBuf linbuf;			// High level line input buffer
	CPSSocBuf outbuf;			// High level output buffer for buffered-output
	u32 outbufp;				// index to outbuf.data[]
} CPSSoc;

//
//	TCP state
//
enum {
	CPS_STT_CLOSED = 0,			// so that initial state will be STT_CLOSED
	CPS_STT_LISTEN,
	CPS_STT_SYN_SENT,
	CPS_STT_SYN_RCVD,
	CPS_STT_ESTABLISHED,
	CPS_STT_CLOSE_WAIT,
	CPS_STT_LAST_ACK,
	CPS_STT_FIN_WAIT1,
	CPS_STT_FIN_WAIT2,
	CPS_STT_CLOSING,
	CPS_STT_DATAGRAM,
	CPS_STT_PING
};

//
//	Block type
//
enum {
	CPS_BLOCK_NONE = 0,
	CPS_BLOCK_TCPCON,
	CPS_BLOCK_TCPREAD,
	CPS_BLOCK_UDPREAD
};

//
//	Reason codes when CPSMyIp == 0
//
enum {
	CPS_NOIP_REASON_NONE = 0,
	CPS_NOIP_REASON_LINKOFF,
	CPS_NOIP_REASON_DHCPDISCOVERY,
	CPS_NOIP_REASON_LEASETIMEOUT,
	CPS_NOIP_REASON_COLLISION
};

//
//	ARP cache definition
//
typedef struct {
	CPSInAddr ip;				// IP address
	CPSMacAddress mac;			// MAC address
	u16 when;					// when this entry was updated (max approx 2hr)
} CPSArpCache;

//
//	IP Fragment table definition
//
#define CPS_MAX_IPSIZE 4096
#define CPS_MIN_MTU    576
#define CPS_MAX_IPFRAG (CPS_MAX_IPSIZE/CPS_MIN_MTU + 1)

typedef struct {
	CPSInAddr ipfrom;			// IP of the sender of IP packet
	u16	frags;					// # of fragments received (also used as a flag)
	u16 id;						// packet ID in IP header
	u16 last;					// chunk # (in 8 byte units) of the last chunk
	u16 size;					// byte size of the final IP data
	u16 from[CPS_MAX_IPFRAG];	// chunk # of the start of the fragment
	u16 to[CPS_MAX_IPFRAG];		// chunk # of the last (+1) of the fragment
	u32 when;					// when the first fragment arrived
	u8 *ofs0;					// pointer to the IP data
	u8 *buf;					// alloc'ed buffer (including Ether and IP headers)
} CPSFragTable;

//
//	Initialization
//
typedef struct {
	u32 mode;
	void *(*alloc)(u32);
	void (*free)(void *);
	void (*dhcp_callback)(void);
	BOOL (*link_is_on)(void);
	unsigned long long random_seed;
	u8 *lan_buf;
	u32 lan_buflen;
	u32 mymss;
	CPSInAddr requested_ip;
	u32 yield_wait;
} CPSConfig;

//
//	CPSConfig.mode definition
//
#define CPS_DONOTUSE_DHCP 0x0001

//
//	for Endian conversion
//
#if	defined(BIG_ENDIAN)
#define CPS_htons(data) (data)
#define CPS_htonl(data) (data)
#else
static inline u16
CPS_htons(u16 data)
{
	return (u16)((data >> 8) | (data << 8));
}

static inline u32
CPS_htonl(u32 data)
{
	u32 tmp;

	tmp = ((data >> 8) & 0x00ff00ff) | ((data << 8) & 0xff00ff00);
	return (tmp >> 16) | (tmp << 16);
}
#endif

////////////////////////////////////////////////////////////////
//
//	Network data field accessors
//
////////////////////////////////////////////////////////////////
//
//	Host u16 accessor (assuming 2byte aligned)
//
#define CPS_USHORT2_HOST(p) (*(u16 *)(p))

//
//	Network accessors (suffix shows the assumption on alignment)
//
#define CPS_GETUSHORT1(p) ((u16)(((u8 *)(p))[0] << 8 | ((u8 *)(p))[1]))
#define CPS_GETULONG1(p) (((u32)CPS_GETUSHORT1(p) << 16) | CPS_GETUSHORT1((p) + 2))

#define CPS_GETUSHORT2(p) (CPS_htons(CPS_USHORT2_HOST(p)))
#define CPS_GETULONG2(p) (((u32)CPS_GETUSHORT2(p) << 16) | CPS_GETUSHORT2((p) + 2))

#define CPS_SETUSHORT1(p, v) do {((u8 *)(p))[0] = (u8)((v) >> 8); ((u8 *)(p))[1] = (u8)(v);} while (0)
#define CPS_SETULONG1(p, v) do {CPS_SETUSHORT1(p, (u16)((v) >> 16)); CPS_SETUSHORT1((p) + 2, (u16)(v));} while (0)

#define CPS_SETUSHORT2(p, v) CPS_USHORT2_HOST(p) = CPS_htons((u16)(v))
#define CPS_SETULONG2(p, v) do {CPS_SETUSHORT2(p, (u32)(v) >> 16); CPS_SETUSHORT2((p) + 2, v);} while (0)

//
//	Throughout this library, LTick (Local Tick) is used to control timeouts.
//	It is a u32 value, equivalent to Tick/65536 (approximately 0.125 seconds),
//	and exacly equal to OSi_TickCounter...
//
//	BE EXTREMELY CAREFUL WHEN PLAYING WITH TICKS USING SYSTEM MACRO!!!!!!!
//	THE LONGEST INTERVAL CALCULATED BY THE SYSTEM MACRO IS 64.077 SECONDS!!!!!!
//	OTHERWISE, IT WILL OVERFLOW DURING THE PROCESS WITH NO WARNINGS!!!!
//
#if 0
#define CPS_MilliSecondsToLTicks(msec) (OS_MilliSecondsToTicks(msec) >> 16)
#else
#define CPS_MilliSecondsToLTicks(msec) (((OS_SYSTEM_CLOCK/64/1000)*(msec)) >> 16)
#endif

#ifndef SDK_THREAD_INFINITY
#define CPS_CURSOC (CPSSocTab[OS_GetCurrentThread()->id])
#define CPS_SET_CURSOC(p) CPS_CURSOC = p
#else
#define CPS_CURSOC ((CPSSoc *)OSi_GetSpecificData(OS_GetCurrentThread(), OSi_SPECIFIC_CPS))
#define CPS_SET_CURSOC(p) OSi_SetSpecificData(OS_GetCurrentThread(), OSi_SPECIFIC_CPS, (void *)(p))
#endif

static inline u32 CPSi_GetTick()
{
	return (u32)(OS_GetTick() >> 16);	// approx. 0.125sec
}

extern void *(*CPSiAlloc)(u32);
extern void (*CPSiFree)(void *);
#define CPSi_Alloc(n) (*CPSiAlloc)(n)
#define CPSi_Free(p) (*CPSiFree)(p)

//
//	Global variables
//
extern CPSInAddr CPSMyIp;
extern CPSInAddr CPSNetMask;
extern CPSInAddr CPSGatewayIp;
extern CPSInAddr CPSDnsIp[2];
extern CPSMacAddress CPSMyMac;
extern CPSInAddr CPSDhcpServerIp;

extern int CPSNoIpReason;

extern MATHRandContext32 CPSiRand32ctx;
#ifndef SDK_THREAD_INFINITY
extern CPSSoc *CPSSocTab[];
#endif

//
//	Interface routines between TCP and SSL
//
extern void CPSi_SslListen(CPSSoc *);
extern u32 CPSi_SslConnect(CPSSoc *);
extern u8 *CPSi_SslRead(u32 *, CPSSoc *);
extern void CPSi_SslConsume(u32, CPSSoc *);
extern s32 CPSi_SslGetLength(CPSSoc *);
extern u32 CPSi_SslWrite2(u8 *, u32, u8 *, u32, CPSSoc *);
extern void CPSi_SslShutdown(CPSSoc *);
extern void CPSi_SslClose(CPSSoc *);
extern void CPSi_SslPeriodical(u32 now);
extern void CPSi_SslCleanup(void);
extern void CPSi_SocConsumeRaw(u32 len, CPSSoc *soc);
extern u32 CPSi_TcpWrite2Raw(u8 *buf, u32 len, u8 *buf2, u32 len2, CPSSoc *soc);
extern u32 CPSi_TcpConnectRaw(CPSSoc *soc);
extern void CPSi_TcpShutdownRaw(CPSSoc *soc);
extern void CPSi_TcpListenRaw(CPSSoc *soc);
extern u8 *CPSi_TcpReadRaw(u32 *len, CPSSoc *soc);

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
//	SOCKET APIs
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

//
//	Management functions
//
extern void CPS_Startup(CPSConfig *);
extern void CPS_Cleanup(void);
extern u32 CPS_GetThreadPriority(void);
extern void CPS_SetThreadPriority(u32);
extern void CPS_SocRegister(CPSSoc *);
extern void CPS_SocUnRegister(void);
extern void CPS_SocUse(void);
extern void CPS_SocRelease(void);
extern void CPS_SocDup(OSThread *);
extern void CPS_SetUdpCallback(int (*)(u8 *, u32, CPSSoc *));
extern int CPS_CalmDown(void);
extern void CPS_SetScavengerCallback(void (*f)(void));

//
//	Base socket I/O's
//
extern void CPS_SocBind(u16 local_port, u16 remote_port, CPSInAddr remote_ip);
extern void CPS_SocDatagramMode(void);
extern void CPS_SocPingMode(void);
extern u8 *CPS_SocRead(u32 *len);
extern void CPS_SocConsume(u32 len);
extern s32 CPS_SocGetLength(void);
extern u32 CPS_SocWrite(u8 *buf, u32 len);
extern u32 CPSi_SocWrite2(u8 *buf, u32 len, u8 *buf2, u32 len2);
extern u32 CPS_TcpConnect(void);
extern void CPS_TcpListen(void);
extern CPSInAddr CPS_SocWho(u16 *remote_port, CPSInAddr *local_ip);
extern void CPS_TcpShutdown(void);
extern void CPS_TcpClose(void);
extern void CPS_TcpAck(void);
extern u16 CPS_SocGetEport(void);

//
//	I/O support functions
//
extern int CPS_SocGetChar(void);
extern u8 *CPS_SocGets(void);
extern void CPS_SocPutChar(char);
extern void CPS_SocPuts(char *);
extern void CPS_SocPrintf(const char *format, ...);
extern void CPS_SocFlush(void);
extern s32 CPS_GetProperSize(void);

extern void CPS_SetSsl(int);
extern u32 CPS_GetSslHandshakePriority(void);
extern void CPS_SetSslHandshakePriority(u32);
#define CPS_SetSslLowThreadPriority CPS_SetSslHandshakePriority
#define CPS_GetSslLowThreadPriority CPS_GetSslHandshakePriority

//
//	Resolver functions
//
extern CPSInAddr CPS_Resolve(const char *);
extern int CPS_RevResolve(CPSInAddr, char *, u32);
extern CPSInAddr CPS_NbResolve(const char *);
extern void CPS_EncodeNbName(u8 *d, u8 *s);

extern void CPSi_RecvCallbackFunc();

#define CPS_MK_IPv4(a,b,c,d) (((u32)(a)<<24)+((u32)(b)<<16)+((u32)(c)<<8)+(u32)(d))
#define CPS_CV_IPv4(ip) (u8)((ip) >> 24), (u8)((ip) >> 16), (u8)((ip) >> 8), (u8)(ip)

#define OS_YieldThread_() OS_YieldThread()

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITROWIFI_CPS_H_ */
#endif
