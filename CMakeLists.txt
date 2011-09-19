PROJECT(eScape)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE(CheckCXXCompilerFlag)

IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)


###########################
# Compilation options
###########################
# Dev stuff
OPTION(BUILD_TEST	"Enable shell frontend (test utilities)" ON)
OPTION(WANT_MALLOC_CHK	"Enable checked malloc (GuardMalloc, Mudflap, Etc)" OFF)
OPTION(DISABLE_SSL	"Disable SSL support (this is far-reaching, use caution)" OFF)

# Libraries
OPTION(BUILD_INK	"Build local copy of glue (libink)" ON)
OPTION(BUILD_AMY	"Enable HTTP/FTP library (libAmy)" ON)
OPTION(BUILD_GWEN	"Enable social media library (libGwen)" ON)
OPTION(BUILD_KYLIE	"Enable streaming media library (libKylie)" ON)
OPTION(BUILD_CHLOE	"Enable chat library (libChloe)" ON)

# Depends
OPTION(SYS_MOWGLI	"Use system libmowgli (NOT RECOMMEND)" OFF)
OPTION(SYS_LIBJSON	"Use system libjson" OFF)

# Client options
OPTION(BUILD_DESKTOP	"Build the desktop clients" ON)
OPTION(BUILD_IPHONE	"Build the iOS clients (Requires Xcode)" OFF)
OPTION(BUILD_ANDROID	"Build Android clients (requires ADK)" OFF)


###########################
# Set all our options
###########################
ADD_DEFINITIONS(-DSQLITE_THREADSAFE=0)

IF(DISABLE_SSL)
	ADD_DEFINITIONS(-DNO_SSL)
ENDIF(DISABLE_SSL)

IF(BUILD_INK)
	SET(LIBINK_SRCS libink/WTDictionary.cpp)
	ADD_LIBRARY(ink SHARED ${LIBINK_SRCS})
ENDIF(BUILD_INK)

IF(BUILD_AMY)
	ADD_DEFINITIONS(-DHAVE_AMY)
	SET(LIBAMY_SRCS libAmy/amy_init.c libAmy/connect.cpp libAmy/connect_http.cpp)
	ADD_LIBRARY(amy SHARED ${LIBAMY_SRCS})
	IF(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
		TARGET_LINK_LIBRARIES(amy ssl crypto socket ink)
	ELSE(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
		IF(WIN32)
			TARGET_LINK_LIBRARIES(amy libeay32 ws2_32 ssleay32 libink)
		ELSE(WIN32)
			TARGET_LINK_LIBRARIES(amy ssl crypto ink)
		ENDIF(WIN32)
	ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
ENDIF(BUILD_AMY)

IF(BUILD_GWEN)
	ADD_DEFINITIONS(-DHAVE_GWEN)
	SET(LIBGWEN_SRCS libGwen/gwen_init.cpp libGwen/TwitterCore/twit_init.cpp)
	ADD_LIBRARY(gwen SHARED ${LIBGWEN_SRCS})
	TARGET_LINK_LIBRARIES(gwen ink amy)
ENDIF(BUILD_GWEN)
