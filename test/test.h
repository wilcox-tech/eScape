#ifndef __ESCAPE_TEST_H__
#define __ESCAPE_TEST_H__

#include <stdio.h>	// printf
#include <string.h>	// strdup
#include <stdlib.h>	// free
#ifdef __MWERKS__
#	include <Utility.h>
#	include <utsname.h>
#else
#	include <sys/utsname.h>
#endif

static int test_num = 0;
static int passed = 0;
static int failed = 0;

#define DO_TEST(name, function, onpass, onfail) testhead(name); if(function) { TEST_PASSED(onpass) } else { TEST_FAILED(onfail) }
#define DO_TEST_FAIL(name, function, onpass, onfail) testhead(name); if(!function) { TEST_PASSED(onpass) } else { TEST_FAILED(onfail) }
#define NOTHING do { } while(false)
#define testhead(test_name) printf("\n\n====================\nTest %d: %s\n====================\n", ++test_num, test_name)
#define TEST_FAILED(x) x; printf(">>>> TEST FAILED <<<<\n"); ++failed;
#define TEST_PASSED(x) x; printf(">>>> TEST PASSED <<<<\n"); ++passed;

void print_header(const char *product)
{
	struct utsname name_info;
	char *os_info;
	if(uname(&name_info) == -1) os_info = strdup("Unknown OS/Unknown Arch");
	else asprintf(&os_info, "%s %s/%s", name_info.sysname, name_info.release, name_info.machine);
	printf( "Wilcox Tech C++ Unit Test Utility for %s on %s\n"\
		"Copyright (c) 2011-2012 Wilcox Technologies, LLC\n"\
		"Confidential and proprietary. Internal use only.\n", product, os_info);
	free(os_info);
};

#define PRINT_STATS printf("\n\nTESTING STATISTICS:\nPassed:\t%d\nFailed:\t%d\nTotal:\t%d\n", passed, failed, passed + failed);

#endif /*!__ESCAPE_TEST_H__*/

