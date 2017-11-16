#ifndef _TYEON_V4L2_COMMON_
#define _TYEON_V4L2_COMMON_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include "../../lib/include/libv4l2.h"

/*! UYVY filxelformat bug */
//#define BUG_PIXEL_FORMAT_ADC
#ifdef BUG_PIXEL_FORMAT_ADC
#define PIXEL_SHIFT_FOR_ADC_UYVY	3
#define PIXEL_SHIFT_FOR_ADC_YUYV	1
#endif

struct buffer {
	void   *start;
	size_t  length;
};

enum {
	READ_MODE_CSI_MEM = 0,
	READ_MODE_CSI_IC_MEM 
};

/*! \brief read raw data 
 * \param input_type input type
 * \param mode mem path in IPU
 *	READ_MODE_CSI_MEM : CSI -> MEM
 *	READ_MODE_CSI_IC_MEM : CSI -> IC -> MEM 
 *
 * IPU 내 CSI 입력을 MEM로 보낸 것을 바로 memory에 저장하는 기능
 * yuv422 저장은 out_yuv422.ppm으로 저장되고,
 * rgb24 저장은 out_rgb24.ppm으로 저장된다.
 *
 * 현재, DMB는 정상이나, CAMERA(tw9900)의 경우 pixelformat이 IPU와
 * TW9900이 달라 pixel shift를 적용해야 rgb24가 정상으로 나온다.
 * 
 * return 0 if sucess, 1 otherwise
 * */
int tyeon_v4l2_read(e_input_type input_type, int mode);


/*! \brief capture data
 *
 * \param input_type input type
 * \param mode mem path in IPU
 *	READ_MODE_CSI_MEM : CSI -> MEM
 *	READ_MODE_CSI_IC_MEM : CSI -> IC -> MEM 
 * \param overaly 
 *	TRUE : use overlay
 *	FALSE : use dma
 *
 * see tyeon_v4l2_read for detail.
 *
 * It saved 30 frames..
 */
int tyeon_v4l2_capture(e_input_type input_type, int mode, int overlay);

/*! \brief capture to fb0 test
 *
 * \param input_type input type
 * \param mode mem path in IPU
 *	READ_MODE_CSI_MEM : CSI -> MEM
 *	READ_MODE_CSI_IC_MEM : CSI -> IC -> MEM 
 * \param rot_test
 *	TRUE : rotation
 *	FALSE ; no rotation
 */
int tyeon_v4l2_capture_to_fb0(e_input_type input_type, int mode, int rot_test);

/*! YUV422 to rgb24 */
int tyeon_v4l2_yuv2rgb(struct v4l2_pix_format *fmt, char *buff, int len,
		struct buffer *rgb_buff);

/* ************* code history *****************************************
 *  1. 2011.05.24 : create code.
 *  2. 2011.05.24 : add <\mode\> parameter for tyeon_v4l2_read function
 *  3. 2011.05.25 : add tyeon_v4l2_capture function
 *  3. 2011.05.25 : add tyeon_v4l2_capture_to_fb0 function
 *
 * ********************************************************************/

#endif
