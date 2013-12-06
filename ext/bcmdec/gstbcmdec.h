 /********************************************************************
 * Copyright(c) 2008 Broadcom Corporation.
 *
 *  Name: gstbcmdec.h
 *
 *  Description: Broadcom 70012 Decoder plugin header
 *
 *  AU
 *
 *  HISTORY:
 *
 *******************************************************************
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************/
#ifndef __GST_BCM_DEC_H__
#define __GST_BCM_DEC_H__


#define	GST_BCM_DEC_RANK	0xffff

#define CLOCK_BASE 9LL
#define CLOC_FREQ_CLOC_BASE * 10000

#define GST_BUF_LIST_POOL_SZ  100;

#define GST_RENDERER_BUF_POOL_SZ 20

#define MPEGTIME_TO_GSTTIME(time) ((time) * (GST_MSECOND/10)) / CLOCK_BASE)

#define GSTIME_TO_MPEGTIME(time) (((time) * CLOCK_BASE) / (GST_MSECOND)/10))

const gint64 UNITS = 1000000000;

#define BRCM_START_CODE_SIZE 4

//VC1 prefix 000001
#define VC1_FRM_SUFFIX	0x0D
#define VC1_SEQ_SUFFIX	0x0F

//VC1 SM Profile prefix 000001
#define VC1_SM_FRM_SUFFIX	0xE0

//Check WMV SP/MP PES Payload for PTS Info
//#define VC1_SM_MAGIC_WORD     0x5A5A5A5A
//#define VC1_SM_PTS_INFO_START_CODE    0xBD

//MPEG2 prefix 000001
#define	MPEG2_FRM_SUFFIX 0x00
#define	MPEG2_SEQ_SUFFIX 0xB3

#define PAUSE_THRESHOLD 16
#define RESUME_THRESHOLD 8
#define	SPS_PPS_SIZE 1000

#define BCM_GST_SHMEM_KEY 0xDEADBEEF
#define	THUMBNAIL_FRAMES 60

typedef enum
{
  H264 = 0,
  MPEG2,
  VC1
} VIDFOMATS;

typedef enum
{
  NV12 = 0,
  YUY2,
  UYVY,
  YV12
} CLRSPACE;

typedef struct
{
  guint width;
  guint height;
  guint8 clr_space;
  gdouble framerate;
  guint8 aspectratio_x;
  guint8 aspectratio_y;
  guint32 y_size;
  guint32 uv_size;
  guint8 stride;

} OUTPARAMS;

typedef struct
{
  guint8 *sps_pps_buf;
  guint32 pps_size;
  gboolean inside_buffer;
  guint32 consumed_offset;
  guint32 strtcode_offset;
  guint32 nal_sz;
  guint8 nal_size_bytes;
} CODEC_PARAMS;



typedef struct _GSTBUF_LIST
{
  GstBuffer *gstbuf;
  struct _GSTBUF_LIST *next;
} GSTBUF_LIST;

#define MAX_ADV_PROF_SEQ_HDR_SZ  		50

typedef enum
{
  UNKNOWN = 0,
  THUMBNAIL = 1,
  PLAYBACK = 2,
} CURDECODE;

typedef struct
{
  guint rendered_frames;
  gboolean waiting;
  CURDECODE cur_decode;
  sem_t inst_ctrl_event;
} GLB_INST_STS;


G_BEGIN_DECLS
#define GST_TYPE_BCM_DEC \
  (gst_bcm_dec_get_type())
#define GST_BCM_DEC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_BCM_DEC,GstBcmDec))
#define GST_BCM_DEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_BCM_DEC,GstBcmDecClass))
#define GST_IS_BCM_DEC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_BCM_DEC))
#define GST_IS_BCM_DEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_BCM_DEC))
typedef struct _GstBcmDec GstBcmDec;
typedef struct _GstBcmDecClass GstBcmDecClass;


struct _GstBcmDec
{
  GstElement element;
  GstPad *sinkpad, *srcpad;
  gboolean silent;
  void *hdevice;
  gboolean dec_ready;
  gboolean streaming;
  gboolean feos;
  GMutex *mPlayLock;
  BC_MEDIA_SUBTYPE input_format;
  OUTPARAMS output_params;
  pthread_t recv_thread;
  sem_t play_event;
  sem_t quit_event;
  BcmDecIF decif;
  Parse parse;
  BC_PIC_INFO_BLOCK pic_info;
  gboolean format_reset;
  gboolean interlace;
  GstClockTime base_time;
  FILE *fhnd;
  gboolean play_pending;
  GstEvent *ev_eos;
  GSTBUF_LIST *gst_buf_que_hd;
  GSTBUF_LIST *gst_buf_que_tl;
  pthread_mutex_t gst_buf_que_lock;
  guint gst_que_cnt;
  pthread_t push_thread;
  gboolean last_picture_set;
  sem_t buf_event;
  guint gst_buf_que_sz;
  GSTBUF_LIST *gst_mem_buf_que_hd;

  gdouble input_framerate;
  guint32 prev_pic;
  gboolean paused;
  gboolean insert_start_code;

  CODEC_PARAMS codec_params;
  gboolean sec_field;
  guint8 input_par_x;
  guint8 input_par_y;

  gboolean flushing;
  sem_t push_stop_event;
  sem_t push_start_event;
  sem_t recv_stop_event;
  guint ses_nbr;
  gboolean insert_pps;
  gboolean ses_change;
  gboolean push_exit;
  pthread_mutex_t fn_lock;
  gboolean suspend_mode;
  GstClock *gst_clock;
  guint32 rpt_pic_cnt;

  gboolean enable_spes;
  guint8 *dest_buf;
  guint32 spes_frame_cnt;
  GstClockTime spes_frm_time;
  gboolean catchup_on;
  GstClockTime last_spes_time;
  GstClockTime last_output_spes_time;
  GstClockTime frame_time;
  GstClockTime base_clock_time;
  GstClockTime prev_clock_time;
  GstClockTime cur_stream_time;
  guint8 proc_in_flags;

  gint frame_width;             /*The value from Demux used for WMV9 or VC-1 SP/MP */
  gint frame_height;            /*The value from Demux used for WMV9 or VC-1 SP/MP */

  GSTBUF_LIST *gst_padbuf_que_hd;
  GSTBUF_LIST *gst_padbuf_que_tl;
  pthread_mutex_t gst_padbuf_que_lock;
  guint gst_padbuf_que_cnt;
  pthread_t get_rbuf_thread;
  sem_t rbuf_start_event;
  sem_t rbuf_stop_event;
  sem_t rbuf_ins_event;
  guint gst_padbuf_que_sz;
  GSTBUF_LIST *gst_mem_padbuf_que_hd;
  gboolean rbuf_thread_running;
};

struct _GstBcmDecClass
{
  GstElementClass parent_class;
};

GType gst_bcm_dec_get_type (void);

G_END_DECLS
#endif /* __GST_BCM_DEC_H__ */
