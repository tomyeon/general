#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "tyeon_v4l2.h"
#include "tyeon_v4l2_common.h"

e_input_type input_type = INPUT_TYPE_UNKNOWN;
e_test_mode test_mode = TEST_MODE_UNKNOWN;
int rotation_test = 0;
int csi_path = READ_MODE_CSI_IC_MEM;

static void print_help(void);
static void parse_type(char *arg, e_input_type *type);
static void parse_mode(char *arg, e_test_mode *mode);
static int parse_opt( int argc, char **argv);

static void print_help(void)
{
	fprintf (stderr, "How to Use----\n");
	fprintf (stderr, "[--type|-t] [ADC/DMB]\n");
	fprintf (stderr, "[--mode|-m]\n");
	fprintf (stderr, "\t READ : read raw-image\n");
	fprintf (stderr, "\t CAPTURE : capture\n");
	fprintf (stderr, "\t CAP2FB0 : capture to fb0\n");
	fprintf (stderr, "\t others : TBD\n");
	fprintf (stderr, "--ic [0:CSI_MEM/1:CSI_IC_MEM]\n");
	fprintf (stderr, "--rot_test : rotation test<CAP2FB0,>\n");
}

static void parse_type(char *arg, e_input_type *type)
{
	if (NULL == type) return;

	if (arg) {
		int i = 0;
		for (; i < INPUT_TYPE_MAX; i++) { 
			if (!strcmp(arg, input_type_str[i]))
				*type = (e_input_type)i;
		}
	}
}

static void parse_mode(char *arg, e_test_mode *mode)
{
	if (NULL == mode) return;

	if (arg) {
		int i = 0;
		for (; i < TEST_MODE_MAX; i++) { 
			if (!strcmp(arg, test_mode_str[i]))
				*mode = (e_test_mode)i;
		}
	}
}

static int parse_opt( int argc, char **argv)
{
    struct option options[] = {
        {"type", 1, NULL, 0},
        {"mode", 1, NULL, 0},
        {"ic", 1, NULL, 0},
        {"rot_test", 0, &rotation_test, 1},
        {0, 0, 0, 0}
    };

    int opt, ret = 0;
    int opt_index = 0;
    int show_help = 0;

    while(1) {

        opt = getopt_long(argc, argv, 
			"t:m:h", options, &opt_index);

        if (-1 == opt)  break;  // 모든 옵션을 확인했으면 루프 종료

        switch(opt) {

        case 0: 
		switch (opt_index) {
		case 0:
			parse_type(optarg, &input_type);
			break;
		case 1:
			parse_mode(optarg, &test_mode);
			break;
		case 2:
			if (optarg)
				csi_path = atoi(optarg);
			break;
		}
		break;
	case 't': 
		parse_type(optarg, &input_type);
		break;
	case 'm': 
		parse_mode(optarg, &test_mode);
		break;
	case 'h': 
		show_help = 1;
		break;
        case '?': 
		show_help = 1;
		break;
        }
    }

    if (!CHECK_TYPE(input_type)) { 
	    fprintf(stderr, "invalid input type\n");
	    show_help = 1;
    }

    if (!CHECK_MODE(test_mode)) {
	    fprintf(stderr, "invalid mode type\n");
	    show_help = 1;
    } 

    if (show_help) {
	    ret = -1;
	    print_help();
    }

    return ret;
}

int main( int argc, char **argv)
{
	int ret;

	ret = parse_opt(argc, argv);

	if (ret < 0) {
		return ret;
	}

	switch (test_mode) {
	case TEST_MODE_READ:
		ret = tyeon_v4l2_read(input_type, csi_path);
		break;
	case TEST_MODE_CAPTURE_IMG:
		//ret = tyeon_v4l2_capture(input_type, csi_path);
		//break;
	case TEST_MODE_CAPTURE_TO_FB0:
	case TEST_MODE_PREVIEW_BY_CAPTURE:
	case TEST_MODE_PREVIEW:
	default:
		fprintf (stderr, "Reserved.. wait\n");
		break;
	}

	return ret;
}

/* ************* code history ***************
 *  1. 2011.05.23 : create code.
 *  2. 2011.05.24 : create read function
 *  3. 2011.05.24 : add csi-ic-mem mode for read function
 *  3. 2011.05.25 : add rotation_test option
 *
 * ******************************************/
