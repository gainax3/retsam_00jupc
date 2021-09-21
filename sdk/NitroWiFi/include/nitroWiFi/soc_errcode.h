/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - include - soc
  File:     soc_errcode.h

  Copyright 2002,2003,2005,2006 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: soc_errcode.h,v $
  Revision 1.3  2006/03/10 09:20:22  kitase_hirotake
  INDENT SOURCE

  Revision 1.2  2005/08/17 04:10:39  yasu
  SOC_Poll の機能追加

  Revision 1.1  2005/07/25 10:15:01  yasu
  SOC ライブラリの追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_SOC_ERRCODE_H_
#define NITROWIFI_SOC_ERRCODE_H_

#ifdef __cplusplus

extern "C" {
#endif

//
// エラー番号
//
#define SOC_E2BIG           (-1)
#define SOC_EACCES          (-2)
#define SOC_EADDRINUSE      (-3)        // Address is already in use
#define SOC_EADDRNOTAVAIL   (-4)
#define SOC_EAFNOSUPPORT    (-5)        // Non-supported address family
#define SOC_EAGAIN          (-6)        // Posix.1
#define SOC_EALREADY        (-7)        // Already in progress
#define SOC_EBADF           (-8)        // Bad socket descriptor
#define SOC_EBADMSG         (-9)
#define SOC_EBUSY           (-10)
#define SOC_ECANCELED       (-11)
#define SOC_ECHILD          (-12)
#define SOC_ECONNABORTED    (-13)       // Connection aborted
#define SOC_ECONNREFUSED    (-14)       // Connection refused
#define SOC_ECONNRESET      (-15)       // Connection reset
#define SOC_EDEADLK         (-16)
#define SOC_EDESTADDRREQ    (-17)       // Not bound to a local address
#define SOC_EDOM            (-18)
#define SOC_EDQUOT          (-19)
#define SOC_EEXIST          (-20)
#define SOC_EFAULT          (-21)
#define SOC_EFBIG           (-22)
#define SOC_EHOSTUNREACH    (-23)
#define SOC_EIDRM           (-24)
#define SOC_EILSEQ          (-25)
#define SOC_EINPROGRESS     (-26)
#define SOC_EINTR           (-27)       // Canceled
#define SOC_EINVAL          (-28)       // Invalid operation
#define SOC_EIO             (-29)       // I/O error
#define SOC_EISCONN         (-30)       // Socket is already connected
#define SOC_EISDIR          (-31)
#define SOC_ELOOP           (-32)
#define SOC_EMFILE          (-33)       // No more socket descriptors
#define SOC_EMLINK          (-34)
#define SOC_EMSGSIZE        (-35)       // Too large to be sent
#define SOC_EMULTIHOP       (-36)
#define SOC_ENAMETOOLONG    (-37)
#define SOC_ENETDOWN        (-38)
#define SOC_ENETRESET       (-39)
#define SOC_ENETUNREACH     (-40)       // Unreachable
#define SOC_ENFILE          (-41)
#define SOC_ENOBUFS         (-42)       // Insufficient resources
#define SOC_ENODATA         (-43)
#define SOC_ENODEV          (-44)
#define SOC_ENOENT          (-45)
#define SOC_ENOEXEC         (-46)
#define SOC_ENOLCK          (-47)
#define SOC_ENOLINK         (-48)
#define SOC_ENOMEM          (-49)       // Insufficient memory
#define SOC_ENOMSG          (-50)
#define SOC_ENOPROTOOPT     (-51)       // Non-supported option
#define SOC_ENOSPC          (-52)
#define SOC_ENOSR           (-53)
#define SOC_ENOSTR          (-54)
#define SOC_ENOSYS          (-55)
#define SOC_ENOTCONN        (-56)       // Not connected
#define SOC_ENOTDIR         (-57)
#define SOC_ENOTEMPTY       (-58)
#define SOC_ENOTSOCK        (-59)       // Not a socket
#define SOC_ENOTSUP         (-60)
#define SOC_ENOTTY          (-61)
#define SOC_ENXIO           (-62)
#define SOC_EOPNOTSUPP      (-63)       // Non-supported operation
#define SOC_EOVERFLOW       (-64)
#define SOC_EPERM           (-65)
#define SOC_EPIPE           (-66)
#define SOC_EPROTO          (-67)
#define SOC_EPROTONOSUPPORT (-68)       // Non-supported protocol
#define SOC_EPROTOTYPE      (-69)       // Non-supported socket type
#define SOC_ERANGE          (-70)
#define SOC_EROFS           (-71)
#define SOC_ESPIPE          (-72)
#define SOC_ESRCH           (-73)
#define SOC_ESTALE          (-74)
#define SOC_ETIME           (-75)
#define SOC_ETIMEDOUT       (-76)       // Timed out
#define SOC_ETXTBSY         (-77)
#define SOC_EWOULDBLOCK     SOC_EAGAIN  // Posix.1g
#define SOC_EXDEV           (-78)

//
//  SO_Poll() に関するパラメータ
//
#define SOC_POLLRDNORM  0x0001          // Normal data read
#define SOC_POLLRDBAND  0x0002          // Priority data read
#define SOC_POLLPRI     0x0004          // High priority data read
#define SOC_POLLWRNORM  0x0008          // Normal data write
#define SOC_POLLWRBAND  0x0010          // Priority data write
#define SOC_POLLERR     0x0020          // Error (revents only)
#define SOC_POLLHUP     0x0040          // Disconnected (revents only)
#define SOC_POLLNVAL    0x0080          // Invalid fd (revents only)
#define SOC_POLLIN      (SOC_POLLRDNORM | SOC_POLLRDBAND)
#define SOC_POLLOUT     SOC_POLLWRNORM
#define SOC_INFTIM      (-1)

#ifdef __cplusplus

}
#endif

#endif // NITROWIFI_SOC_ERRCODE_H_
