#ifndef _TYEON_V4L2_H_
#define _TYEON_V4L2_H_

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

typedef enum {
	INPUT_TYPE_MIN = 0,
	INPUT_TYPE_FROM_ADC = INPUT_TYPE_MIN,	/*! from ADC(TW9900) */
	INPUT_TYPE_DMB,				/*! from DMB Module */
	INPUT_TYPE_UNKNOWN,
	INPUT_TYPE_MAX = INPUT_TYPE_UNKNOWN,
} e_input_type;

extern char *input_type_str[INPUT_TYPE_MAX + 1];

extern char *video_dev_path[INPUT_TYPE_MAX + 1];

typedef enum {
	TEST_MODE_MIN = 0,	
	TEST_MODE_READ = TEST_MODE_MIN,	/*! capture raw-image [CSI -> MEM] */
	TEST_MODE_CAPTURE_IMG,		/*! capture image 
						DMB : CSC complete
						ADC : Non-CSC
					*/
	TEST_MODE_CAPTURE_TO_FB0,	/*! capture to fb0 */
	TEST_MODE_PREVIEW_BY_CAPTURE,	/*! capture image and preview */
	TEST_MODE_PREVIEW,		/*! preview mode */
	TEST_MODE_UNKNOWN,
	TEST_MODE_MAX = TEST_MODE_UNKNOWN,
} e_test_mode;

extern char *test_mode_str[TEST_MODE_MAX + 1];

#define CHECK_TYPE(type) (type >= INPUT_TYPE_MIN ||\
		    type <= INPUT_TYPE_MAX)

#define CHECK_MODE(mode) (test_mode >= TEST_MODE_MIN &&\
		test_mode <= TEST_MODE_MAX)

/* ************* code history ***************
 *  1. 2011.05.23 : create code.
 *  2. 2011.05.23 : add TEST_MODE_READ_IC mode
 *  3. 2011.05.25 : add TEST_MODE_CAPTURE_TO_FB0 mode
 *
 * ******************************************/

#endif
