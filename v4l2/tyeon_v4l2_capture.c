#include "tyeon_v4l2.h"
#include "tyeon_v4l2_common.h"

static int _tyeon_v4l2_capture_csi2ic2mem(e_input_type input_type)
{
	int ret = -1;

	return ret;
}

static int _tyeon_v4l2_capture_csi2ic2mem_overlay(e_input_type input_type)
{
	int ret = -1;

	return ret;
}

static int _tyeon_v4l2_capture_csi2mem(e_input_type input_type)
{
	int ret = -1;
	
	return ret;
}

static int _tyeon_v4l2_capture_csi2mem_overlay(e_input_type input_type)
{
	int ret = -1;
	
	return ret;
}

int tyeon_v4l2_capture(e_input_type input_type, int mode, int overlay)
{
	int ret = -1;
	if (mode == READ_MODE_CSI_MEM) {
		if (TRUE == overlay)
			ret = _tyeon_v4l2_capture_csi2mem_overlay(input_type);
		else
			ret = _tyeon_v4l2_capture_csi2mem(input_type);
	} else if (mode == READ_MODE_CSI_IC_MEM) {
		if (TRUE == overlay)
			ret = _tyeon_v4l2_capture_csi2ic2mem_overlay(input_type);
		else
			ret = _tyeon_v4l2_capture_csi2ic2mem(input_type);
	}

	return ret;
}

/* *********************** code history ************************
 *  1. 2011.05.25 : create code.
 *	, hold
 *
 * ************************************************************/
