/*---------------------------------------------------------------------------*

  NintendoDS VoiceChat library.
  
   Copyright (c) 2005-2006 Abiosso Networks, Inc. ALL RIGHTS RESERVED.

  These coded instructions, statements, and computer programs contain
  proprietary information of Abiosso Networks, Inc., and are protected
  by Federal copyright law.  They may not be disclosed to third parties
  or copied or duplicated in any form, in whole or in part,  without
  the prior written consent of Abiosso.

 *---------------------------------------------------------------------------*/

#ifndef VCT_VOICECHAT_H_
#define VCT_VOICECHAT_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- VoiceChat common error code.
#define VCT_ERROR_NONE                  0
#define VCT_ERROR_BAD_PARAM            -1
#define VCT_ERROR_BAD_MODE             -2
#define VCT_ERROR_BAD_REQUEST          -3
#define VCT_ERROR_SEND_FAIL            -4
#define VCT_ERROR_TRANSCEIVER_BUSY     -5
#define VCT_ERROR_EXHAUST_CLIENTS      -6

// session, clients limitation    
#define VCT_MAX_SESSION             8
#define VCT_MAX_TRANSCEIVER_CLIENT  8
#define VCT_MAX_CONFERENCE_CLIENT   4

#define VCT_MAX_AUDIO_STREAM (VCT_MAX_CONFERENCE_CLIENT - 1)

#define VCT_MAGIC_PACKET_HEADER '_VCT'
    

typedef struct _VCTSession VCTSession;

// --- Available audio codecs.
typedef enum {
    VCT_CODEC_8BIT_RAW,   // 8Bit 8KHz raw audio
    VCT_CODEC_G711_ULAW,  // G711 u-Law
    VCT_CODEC_2BIT_ADPCM, // 2bit IMA-ADPCM (Not standard format)
    VCT_CODEC_3BIT_ADPCM, // 3bit IMA-ADPCM
    VCT_CODEC_4BIT_ADPCM, // 4bit IMA-ADPCM
    VCT_CODEC_END
} VCTCodec;

#define VCT_AUDIO_FRAME_LENGTH  68   // VoiceChat audio frame length (68ms)
#define VCT_AUDIO_FRAME_RATE    8000 // VoiceChat audio frame rate (8KHz)

// --- Audio streaming information
typedef struct VCTAudioInfo
{
    u32         internalLatency;    // Library internal latency.
    u32         bufferLatency;      // Jitter buffer latency.
    u32         bufferCount;        // Number of buffer in jitter buffer.
    int         clockSkew;          // Clock skew (ticks).
    u32         sequence;           // Now audio packet sequence number.
    u32         dropCount;          // Dropped packet count.
    u32         jitter;             // Maxmum jitter (ticks).
    u32         jamCount;           // Jammed packet count.
    u32         recoverCount;       // Number of recovery packet.
    VCTCodec    codec;              // Remote audio codec.
} VCTAudioInfo;

// --- VoiceChat mode
typedef enum {
    VCT_MODE_NULL,
    VCT_MODE_PHONE,
    VCT_MODE_TRANSCEIVER,
    VCT_MODE_CONFERENCE
} VCTMode;

// --- VoiceChat library event code.
typedef enum {
    VCT_EVENT_NONE,
    
    VCT_EVENT_INCOMING,         // Incoming call
    VCT_EVENT_REJECT,           // Call was rejected.
    VCT_EVENT_BUSY,             // Remote client is busy.
    VCT_EVENT_CANCEL,           // Cancel call.
    VCT_EVENT_NOTIFY_FREE,      // Received 'NOTIFY_FREE'
    VCT_EVENT_NOTIFY_BUSY,      // Received 'NOTIFY BUSY'
    VCT_EVENT_CONNECTED,        // Connected to remote client.
    VCT_EVENT_RESPONDBYE,       // Received 'BYE' request.
    VCT_EVENT_DISCONNECTED,     // Connection closed.
    VCT_EVENT_CONTACT,          // Received 'CONTACT' request.
    VCT_EVENT_TIMEOUT,          // Session timeout.
    VCT_EVENT_ABORT             // Session abort.
} VCTEvent;

// --- Callback function prototype.
typedef void (*VCTEventCallback)(u8 aid, VCTEvent result, VCTSession *session, void *param);

// --- Receive buffer size.
#define VCT_AUDIO_BUFFER_SIZE          1144
#define VCT_DEFAULT_AUDIO_BUFFER_COUNT  8
#define VCT_MIN_AUDIO_BUFFER_COUNT      4
#define VCT_MAX_AUDIO_BUFFER_COUNT     (24 * VCT_MAX_AUDIO_STREAM)
    
// --- Initialize VoiceChat library.
typedef struct
{
    VCTSession* session;
    u32         numSession;
    VCTMode     mode;
    u8          aid;
    u8          padding__[3];

    void*       audioBuffer;
    u32         audioBufferSize;
    
    VCTEventCallback    callback;
    void*               userData;

} VCTConfig;

// --- SSP request code
typedef enum {
    VCT_REQUEST_INVITE = 0,
    VCT_REQUEST_BYE,
    VCT_REQUEST_CANCEL,
    VCT_REQUEST_CONTACT,
    VCT_REQUEST_NOTIFY,
    VCT_REQUEST_END
} VCTRequestCode;
        
// --- SSP response code
typedef enum {
    VCT_RESPONSE_OK,
    VCT_RESPONSE_BAD_REQUEST,
    VCT_RESPONSE_NOT_ACCEPTABLE,
    VCT_RESPONSE_BUSY_HERE,
    VCT_RESPONSE_TERMINATED,
    VCT_RESPONSE_DECLINE,
    VCT_RESPONSE_END
} VCTResponseCode;

// --- State code
typedef enum {
    VCT_STATE_INIT,
    VCT_STATE_OUTGOING,
    VCT_STATE_TALKING,
    VCT_STATE_CONNECTED,
    VCT_STATE_INCOMING,
    VCT_STATE_DISCONNECTING
} VCTState;

// --- SSP session information
struct _VCTSession 
{
    VCTMode         mode;     		// mode of this session
    u8              aid;            // remote aid
    u8              talking;        // talking aid for tranceiver mode
    u16             padding__;
    u32             aidBitmap;      // aid bitmap for transceiver/conference clients

    VCTState        state;    		// state of this session
    void*           userData;       // user private data

    // private area. don't touch.
    //
    VCTSession*     _next;
};

// --- Information for VAD
typedef struct _VCTVADInfo
{
    BOOL        activity;       // TRUE if VAD detect sound, FALSE if VAD detect noise
    int         scale;          // scale for input audio (0-127)
    int         releaseCount;   // VAD release count
    int         releaseTime;    // VAD release time
} VCTVADInfo;

/*===========================================================================*
 *
 * Initialize / Finalize functions
 *
 *===========================================================================*/
    
/*---------------------------------------------------------------------------*
  Name:         VCT_Init

  Description:  Initialize libVCT

  Arguments:    config   pointer of VCTConfig structure.

  Returns:      if initialize is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL VCT_Init(VCTConfig *config);
    

/*---------------------------------------------------------------------------*
  Name:         VCT_Cleanup

  Description:  Cleanup libVCT.
 *---------------------------------------------------------------------------*/
void VCT_Cleanup(void);


/*---------------------------------------------------------------------------*
  Name:         VCT_Main

  Description:  Main loop of libVCT.
 *---------------------------------------------------------------------------*/
void VCT_Main();


/*---------------------------------------------------------------------------*
  Name:         VCT_HandleData

  Description:  Handle received data.

  Arguments:    aid     AID of transmitting client.
                buffer  Received data.
                size    Size of receied data.

  Returns:      if received data is for libVCT, TRUE.
 *---------------------------------------------------------------------------*/
BOOL VCT_HandleData(u8 aid, u8* buffer, int size);



/*===========================================================================*
 *
 * SSP functions
 *
 *===========================================================================*/
    

/*---------------------------------------------------------------------------*
  Name:         VCT_Request

  Description:  Send SSP request.

  Arguments:    session     A session to issue a request.
                request     request code.

  Returns:      if the request operation is successful, zero.
                otherwise error code.
 *---------------------------------------------------------------------------*/
int VCT_Request(VCTSession *session, VCTRequestCode request);


/*---------------------------------------------------------------------------*
  Name:         VCT_Response

  Description:  Send SSP response.

  Arguments:    session     A session to issue a response.
                request     response code.

  Returns:      if the response operation is successful, zero.
                otherwise error code.
 *---------------------------------------------------------------------------*/
int VCT_Response(VCTSession *session, VCTResponseCode response);


/*---------------------------------------------------------------------------*
  Name:         VCT_Contact

  Description:  Send transceier initiation request.

  Arguments:    session     pointer of session structure.

  Returns:      if the request operation is successful, zero.
                otherwise error code.
 *---------------------------------------------------------------------------*/
int VCT_Contact(VCTSession **outSession);


/*---------------------------------------------------------------------------*
  Name:         VCT_Release

  Description:  Send transceiver closure request.

  Arguments:    session     pointer of session structure.

  Returns:      if the request operation is successful, zero.
                otherwise error code.
 *---------------------------------------------------------------------------*/
int VCT_Release(VCTSession *session);


/*---------------------------------------------------------------------------*
  Name:         VCT_SetTransceiverMode

  Description:  Set transceiver mode.

  Arguments:    true_or_false   TRUE for set server, FALSE for set client.

  Returns:      NONE
 *---------------------------------------------------------------------------*/
void VCT_SetTransceiverMode(BOOL true_or_false);


/*---------------------------------------------------------------------------*
  Name:         VCT_SetTransceiverServer

  Description:  Set transceiver server's aid.

  Arguments:    aid     AID of transceiver server.

  Returns:      if operation is successful, zero. otherwise error code.
 *---------------------------------------------------------------------------*/
int VCT_SetTransceiverServer(u8 aid);


/*---------------------------------------------------------------------------*
  Name:         VCT_SetTransceiverClients

  Description:  Set transceiver client's aid.

  Arguments:    aidList     AID of transceiver clients.
                num_of_aid  number of clients.

  Returns:      if operation is successful, zero. otherwise error code.
 *---------------------------------------------------------------------------*/
int VCT_SetTransceiverClients(u8 aidList[], int num_of_aid);


/*---------------------------------------------------------------------------*
  Name:         VCT_SetTransceiverTimeout

  Description:  Set transceiver timeout.

  Arguments:    sec     timeout seconds.
 *---------------------------------------------------------------------------*/
void VCT_SetTransceiverTimeout(u32 sec);


/*---------------------------------------------------------------------------*
  Name:         VCT_AddConferenceClient

  Description:  Add client to conference.

  Arguments:    aid     client AID.

  Returns:      if operation is successful, zero. otherwise error code.
 *---------------------------------------------------------------------------*/
int VCT_AddConferenceClient(u8 aid);


/*---------------------------------------------------------------------------*
  Name:         VCT_RemoveConferenceClient

  Description:  Remove client from conference.

  Arguments:    aid     client AID.

  Returns:      if operation is successful, zero. otherwise error code.
 *---------------------------------------------------------------------------*/
int VCT_RemoveConferenceClient(u8 aid);


/*---------------------------------------------------------------------------*
  Name:         VCT_CreateSession

  Description:  Create new session.

  Arguments:    aid     client AID.

  Returns:      if operation is successful, VCTSession structure,
                otherwize error code.
 *---------------------------------------------------------------------------*/
VCTSession * VCT_CreateSession(u8 aid);


/*---------------------------------------------------------------------------*
  Name:         VCT_DeleteSession

  Description:  Delete exist session.

  Arguments:    session     Pointer of session structure.

  Returns:      if operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL VCT_DeleteSession(VCTSession *session);


/*===========================================================================*
 *
 * Audio streaming functions
 *
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         VCT_StartStreaming

  Description:  Start audio streaming.

  Arguments:    session     A session to start streaming.

  Returns:      if operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL VCT_StartStreaming(VCTSession *session);


/*---------------------------------------------------------------------------*
  Name:         VCT_StopStreaming

  Description:  Stop audio streaming.

  Arguments:    session     A session to stop streaming.

  Returns:      NONE
 *---------------------------------------------------------------------------*/
void VCT_StopStreaming(VCTSession *session);


/*---------------------------------------------------------------------------*
  Name:         VCT_SendAudio

  Description:  Queue audio data to buffer.

  Arguments:    audio_data      audio data.
                length          size of audio data (in bytes).

  Returns:      if operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL VCT_SendAudio(void* audio_data, u32 length);


/*---------------------------------------------------------------------------*
  Name:         VCT_ReceiveAudio

  Description:  Retrieve audio data from buffer.

  Arguments:    audio_data      pointer of audio data buffer
                length          size of audio buffer
                outAIDBitmap    bitmap of sender AIDs

  Returns:      if operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL VCT_ReceiveAudio(void* audio_data, u32 length, u32 *outAIDBitmap);

/*---------------------------------------------------------------------------*
  Name:         VCT_SetCodec

  Description:  Set audio codec.

  Arguments:    codec       codec type.
 *---------------------------------------------------------------------------*/
BOOL VCT_SetCodec(VCTCodec codec);


/*---------------------------------------------------------------------------*
  Name:         VCT_GetCodec

  Description:  Get now audio codec.
 *---------------------------------------------------------------------------*/
VCTCodec VCT_GetCodec(void);


/*---------------------------------------------------------------------------*
  Name:         VCT_GetAudioInfo

  Description:  Get audio streaming information.

  Arguments:    channel     channel number of be retrieved.
  Arguments:    info        pointer of VCTAudioInfo structure.
 *---------------------------------------------------------------------------*/
void VCT_GetAudioInfo(u32 channel, VCTAudioInfo *info);



/*===========================================================================*
 *
 * VAD functions
 *
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         VCT_EnableVAD

  Description:  Turn on/off VAD.
 *---------------------------------------------------------------------------*/
void VCT_EnableVAD(BOOL flag);

/*---------------------------------------------------------------------------*
  Name:         VCT_SetVADReleaseTime

  Description:  Set VAD release time.

  Arguments:    count       release time (numbe of packets)
 *---------------------------------------------------------------------------*/
void VCT_SetVADReleaseTime(int count);


/*---------------------------------------------------------------------------*
  Name:         VCT_ResetVAD

  Description:  Reset internal VAD parameters.

  Returns:      None
 *---------------------------------------------------------------------------*/
void VCT_ResetVAD(void);


/*---------------------------------------------------------------------------*
  Name:         VCT_GetVADInfo

  Description:  Get internal VAD parameters.

  Arguments:    outInfo		pointer of VCTVADInfo structure.

  Returns:      None
 *---------------------------------------------------------------------------*/
void VCT_GetVADInfo(VCTVADInfo *outInfo);

/*===========================================================================*
 *
 * Echo Canceling
 *
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         VCT_EnableEchoCancel

  Description:  Turn on/off Echo cancel processor.

  Arguments:    flag		enable/disable echo canceling.

  Returns:      None
 *---------------------------------------------------------------------------*/
void VCT_EnableEchoCancel(BOOL flag);


/*===========================================================================*
 *
 * Debugging
 *
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         VCTi_SetReportLevel

  Description:  Set debug report level
  
  Arguments:    level		Set reporting level.

  Returns:      None
 *---------------------------------------------------------------------------*/
typedef enum {
    VCT_REPORTLEVEL_NONE,       // Don't output any message
    VCT_REPORTLEVEL_ALL         // Report all message
} VCTReportLevel;

#ifndef SDK_FINALROM
void    VCT_SetReportLevel(VCTReportLevel level);
#else
#define VCT_SetReportLevel(level) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif // VCT_VOICECHAT_H_
