#include "tyeon_v4l2.h"
#include "tyeon_v4l2_common.h"


char *input_type_str[INPUT_TYPE_MAX + 1] = {
	"ADC",
	"DMB",
	"UNKNOWN",
};

char *video_dev_path[INPUT_TYPE_MAX + 1] = {
	"/dev/video0",
	"/dev/video1",
	"",
};

char *test_mode_str[TEST_MODE_MAX + 1] = {
	"READ",
	"CAPTURE_IMG",
	"CAPTURE_TO_FB0",
	"PREVIEW_BY_CAP",
	"PREVIEW",
	"UNKNOWN",
};

static inline void _v4l2_yuv2rgb(int *Y, int *U, int *V, int *R, int *G, int *B)
{
	*B = 1.164*(*Y - 16) + 2.018*(*U - 128);
	*G = 1.164*(*Y - 16) - 0.813*(*V - 128) - 0.391*(*U - 128);
	*R = 1.164*(*Y - 16) + 1.596*(*V - 128);

	if (*R > 255) *R = 255;
	if (*R < 0) *R = 0;
	if (*G > 255) *G = 255;
	if (*G < 0) *G = 0;
	if (*B > 255) *B = 255;
	if (*B < 0) *B = 0;
}

int tyeon_v4l2_yuv2rgb(struct v4l2_pix_format *fmt, char *buff, int len,
		struct buffer *rgb_buff)
{
	int i = 0, j = 0;
	int ret = -1;
	char *rgb_buff_alias = NULL;

	if (!fmt || !buff || len <= 0 || !rgb_buff) 
		goto _yuv2rgb_err;

	if (fmt->width <= 0 ||
			fmt->height <= 0)
		goto _yuv2rgb_err;

	//Now, only UYVY / YUYV
	if (!(fmt->pixelformat == V4L2_PIX_FMT_UYVY ||
		fmt->pixelformat == V4L2_PIX_FMT_UYVY)) 
		goto _yuv2rgb_err;

	rgb_buff->length = fmt->width * fmt->height * 3;
	rgb_buff->start = malloc(rgb_buff->length);
	memset(rgb_buff->start, 0, rgb_buff->length);
	rgb_buff_alias = (char*)rgb_buff->start;

	for (i = 0; i < len && j < rgb_buff->length; i += 4, j += 6) {
		int R, G, B;
		int Y, U, V;

		if (fmt->pixelformat == V4L2_PIX_FMT_UYVY) {
			Y = (int)(*(buff + i + 1));
			U = (int)(*(buff + i));
			V = (int)(*(buff + i + 2));
			_v4l2_yuv2rgb(&Y, &U, &V, &R, &G, &B);
			rgb_buff_alias[j] = (char)R;	
			rgb_buff_alias[j+1] = (char)G;
			rgb_buff_alias[j+2] = (char)B;
			Y = (int)(*(buff + i + 3));
			_v4l2_yuv2rgb(&Y, &U, &V, &R, &G, &B);
			rgb_buff_alias[j+3] = (char)R;	
			rgb_buff_alias[j+4] = (char)G;
			rgb_buff_alias[j+5] = (char)B;
		} else if (fmt->pixelformat == V4L2_PIX_FMT_YUYV) {
			Y = (int)(*(buff + i));
			U = (int)(*(buff + i + 1));
			V = (int)(*(buff + i + 3));
			_v4l2_yuv2rgb(&Y, &U, &V, &R, &G, &B);
			rgb_buff_alias[j] = (char)R;	
			rgb_buff_alias[j+1] = (char)G;
			rgb_buff_alias[j+2] = (char)B;
			Y = (int)(*(buff + i + 2));
			_v4l2_yuv2rgb(&Y, &U, &V, &R, &G, &B);
			rgb_buff_alias[j+3] = (char)R;	
			rgb_buff_alias[j+4] = (char)G;
			rgb_buff_alias[j+5] = (char)B;
		}
	}
	ret = 0;

_yuv2rgb_err:
	return ret;
}

/* ************* code history ***************
 *  1. 2011.05.24 : create code.
 *
 * ******************************************/
