#include "tyeon_v4l2.h"
#include "tyeon_v4l2_common.h"

static int _tyeon_v4l2_read_csi2ic2mem(e_input_type input_type)
{
	int fd = -1;
	int ret = -1, mode;
	int input = 1;
	int width, height;
	char *video_dev = NULL;
	struct buffer buff;
	struct v4l2_format fmt;
	struct v4l2_streamparm s_parm;
	struct v4l2_crop crop;

	fprintf(stdout, "start to read (CIS->IC->MEM)\n");

	/*! check type */ 
	if (!CHECK_TYPE(input_type)) { 
		fprintf(stderr, "invalid type\n");
		return -1;
	}

	video_dev = video_dev_path[input_type];

	fd = open(video_dev, O_RDWR, 0);
	if (fd < 0) {
		fprintf(stderr, "can't open device[%s]\n"), video_dev;
		return ret;
	}

	/*! VIDIOC_S_INPUT ==> 0
	 * It means CSI ==> IC ==> MEM */
	input = 0;
	ret = ioctl(fd, VIDIOC_S_INPUT, &input);
	if (ret < 0) {
		fprintf(stderr, "failed to set ioctl[VIDIOC_S_INPUT]\n");
		goto _read_err0;
	}

	/*! check input standard */
	if (input_type == INPUT_TYPE_FROM_ADC) {
		/*! ADC의 경우 이것을 호출해 주어야 현재 입력되는 standard를
		 * v4l2 driver에 적용시킨다.
		 */
		v4l2_std_id id;
		ret = ioctl(fd, VIDIOC_G_STD, &id);
		if (ret < 0) {
			fprintf(stderr, "failed to set ioctl[VIDIOC_G_STD]\n");
			goto _read_err0;
		}

		ret = ioctl(fd, VIDIOC_S_STD, &id);
		if (ret < 0) {
			fprintf(stderr, "failed to set ioctl[VIDIOC_S_STD]\n");
			goto _read_err0;
		}
	}

	/*! set param */
	memset(&s_parm, 0, sizeof(struct v4l2_streamparm));

	s_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_S_PARM, &s_parm);
	if (ret < 0) {
		fprintf(stderr, "Failed to VIDIOC_S_PARM\n");
		goto _read_err0;
	}

	/*! get crop */
	memset(&crop, 0, sizeof(struct v4l2_crop));
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_CROP, &crop);
	if (ret < 0) {
		fprintf(stderr, "Failed to VIDIOC_G_CROP\n");
		goto _read_err0;
	}
	fprintf(stdout, "w:%d, h:%d\n", crop.c.width, crop.c.height);

	if (input_type == INPUT_TYPE_FROM_ADC) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c.height /= 2;
		ret = ioctl(fd, VIDIOC_S_CROP, &crop);
		if (ret < 0) {
			fprintf(stderr, "Failed to VIDIOC_S_CROP\n");
			goto _read_err0;
		}
	}

	/*! set s_fmt */
	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
	fmt.fmt.pix.width = crop.c.width;
	fmt.fmt.pix.height = crop.c.height;

	ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		fprintf(stderr, "Failed to S_FMT\n");
		goto _read_err0;
	}

	/*! get fmt (for cheking) */
	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		fprintf(stderr, "Failed to G_FMT\n");
		goto _read_err0;
	}

	fprintf(stdout, "width:%d, height:%d, sizeimage:%d\n",
			fmt.fmt.pix.width,
			fmt.fmt.pix.height,
			fmt.fmt.pix.sizeimage);

	/*! read */
	memset(&buff, 0, sizeof (struct buffer));
	buff.length = fmt.fmt.pix.sizeimage;
	buff.start = malloc(buff.length);

	ret = read(fd, buff.start, buff.length);

	if (-1 == ret) {
		fprintf(stderr, "failed to read\n");
		goto _read_err1;
	} else {
		FILE *fout = NULL;
		char out_name[256] = {0, };
		struct buffer rgb_buff;

		sprintf(out_name, "/tmp/out_rgb24.ppm");
		fout = fopen(out_name, "w");
		if (!fout) {
			fprintf(stderr, "can't open image\n");
			goto _read_err1;
		}
		fprintf(fout, "P6\n%d %d 255\n",
			fmt.fmt.pix.width, fmt.fmt.pix.height);
		fwrite(buff.start, buff.length, 1, fout);
		fclose(fout);
	} 

	ret = 0;
_read_err1:
	free(buff.start);

_read_err0:
	close(fd);

	if (ret < -1) {
		fprintf(stderr, "failed the read-operation\n");
	}

	return ret;

}

static int _tyeon_v4l2_read_csi2mem(e_input_type input_type)
{
	int fd = -1;
	int ret = -1, mode;
	int input = 1;
	int width, height;
	char *video_dev = NULL;
	struct buffer buff;
	struct v4l2_format fmt;
	struct v4l2_streamparm s_parm;
	struct v4l2_crop crop;

	fprintf(stdout, "start to read (CIS->MEM)\n");
	/*! check type */ 
	if (!CHECK_TYPE(input_type)) { 
		fprintf(stderr, "invalid type\n");
		return -1;
	}

	video_dev = video_dev_path[input_type];

	fd = open(video_dev, O_RDWR, 0);
	if (fd < 0) {
		fprintf(stderr, "can't open device[%s]\n", video_dev);
		return ret;
	}

	/*! VIDIOC_S_INPUT ==> 1
	 * It means CSI ==> MEM */
	input = 1;
	ret = ioctl(fd, VIDIOC_S_INPUT, &input);
	if (ret < 0) {
		fprintf(stderr, "failed to set ioctl[VIDIOC_S_INPUT]\n");
		goto _read_err0;
	}

	/*! check input standard */
	if (input_type == INPUT_TYPE_FROM_ADC) {
		/*! ADC의 경우 이것을 호출해 주어야 현재 입력되는 standard를
		 * v4l2 driver에 적용시킨다.
		 */
		v4l2_std_id id;
		ret = ioctl(fd, VIDIOC_G_STD, &id);
		if (ret < 0) {
			fprintf(stderr, "failed to set ioctl[VIDIOC_G_STD]\n");
			goto _read_err0;
		}

		ret = ioctl(fd, VIDIOC_S_STD, &id);
		if (ret < 0) {
			fprintf(stderr, "failed to set ioctl[VIDIOC_S_STD]\n");
			goto _read_err0;
		}
	}

	/*! set param */
	memset(&s_parm, 0, sizeof(struct v4l2_streamparm));

	s_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_S_PARM, &s_parm);
	if (ret < 0) {
		fprintf(stderr, "failed to VIDIOC_S_PARM\n");
		goto _read_err0;
	}

	/*! get crop */
	memset(&crop, 0, sizeof(struct v4l2_crop));
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_CROP, &crop);
	if (ret < 0) {
		fprintf(stderr, "failed to VIDIOC_G_CROP\n");
		goto _read_err0;
	}
	fprintf(stdout, "w:%d, h:%d\n", crop.c.width, crop.c.height);

	if (input_type == INPUT_TYPE_FROM_ADC) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c.height /= 2;
		ret = ioctl(fd, VIDIOC_S_CROP, &crop);
		if (ret < 0) {
			fprintf(stderr, "failed to VIDIOC_S_CROP\n");
			goto _read_err0;
		}
	}

	/*! set s_fmt */
	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	fmt.fmt.pix.width = crop.c.width;
	fmt.fmt.pix.height = crop.c.height;

	ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		fprintf(stderr, "failed to S_FMT\n");
		goto _read_err0;
	}

	/*! get fmt (for cheking) */
	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		fprintf(stderr, "failed to G_FMT\n");
		goto _read_err0;
	}

	fprintf(stdout, "width:%d, height:%d, sizeimage:%d\n",
			fmt.fmt.pix.width,
			fmt.fmt.pix.height,
			fmt.fmt.pix.sizeimage);

	/*! read */
	memset(&buff, 0, sizeof (struct buffer));
	buff.length = fmt.fmt.pix.sizeimage;
	buff.start = malloc(buff.length);

	ret = read(fd, buff.start, buff.length);

	if (-1 == ret) {
		fprintf(stderr, "failed to read\n");
		goto _read_err1;
	} else {
		int pixel_shift = 0;
		FILE *fout = NULL;
		char out_name[256] = {0, };
		struct buffer rgb_buff;

#ifdef BUG_PIXEL_FORMAT_ADC
		if (input_type == INPUT_TYPE_FROM_ADC) {
			if (V4L2_PIX_FMT_UYVY == fmt.fmt.pix.pixelformat) 
				pixel_shift = PIXEL_SHIFT_FOR_ADC_UYVY;
			else if (V4L2_PIX_FMT_YUYV == fmt.fmt.pix.pixelformat) 
				pixel_shift = PIXEL_SHIFT_FOR_ADC_YUYV;
		}
#endif

		sprintf(out_name, "/tmp/out_yuv422.ppm");
		fout = fopen(out_name, "w");
		if (!fout) {
			fprintf(stderr, "can't open image\n");
			goto _read_err1;
		}
		fprintf(fout, "P6\n%d %d 255\n",
			fmt.fmt.pix.width, fmt.fmt.pix.height);
		fwrite(buff.start, buff.length, 1, fout);
		fclose(fout);

		memset(&rgb_buff, 0, sizeof(struct buffer));
		ret = tyeon_v4l2_yuv2rgb(&fmt.fmt.pix, 
				(char*)buff.start+pixel_shift, 
				buff.length-pixel_shift,
				&rgb_buff);
		if (ret == 0) {
			sprintf(out_name, "/tmp/out_rgb24.ppm");
			fout = fopen(out_name, "w");
			if (!fout) {
				fprintf(stderr, "can't open image(rgb)\n");
				ret = -1;
				goto _read_err2;
			}

			fprintf(fout, "P6\n%d %d 255\n",
				fmt.fmt.pix.width, fmt.fmt.pix.height);
			fwrite(rgb_buff.start, rgb_buff.length, 1, fout);
			fclose(fout);
_read_err2:
			free(rgb_buff.start);
		} else {
			fprintf(stderr, "failed to convert yuv to rgb\n");
			goto _read_err1;
		}
	} 

	ret = 0;
_read_err1:
	free(buff.start);

_read_err0:
	close(fd);

	if (ret < -1) {
		fprintf(stderr, "failed the read-operation\n");
	}

	return ret;
}


int tyeon_v4l2_read(e_input_type input_type, int mode)
{
	int ret = -1;
	if (mode == READ_MODE_CSI_MEM)
		ret = _tyeon_v4l2_read_csi2mem(input_type);
	else if (mode == READ_MODE_CSI_IC_MEM)
		ret = _tyeon_v4l2_read_csi2ic2mem(input_type);

	return ret;
}

/* ************* code history ***************
 *  1. 2011.05.24 : create code.
 *	- check DMB
 *	- check TW9900
 *  2. 2011.05.24 : create CSI->IC->MEM path
 *	- check DMB
 *	- TW9900 has a problem(No CSC operation)
 *
 * ******************************************/
