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
ELSE(DISABLE_SSL)
	FIND_PACKAGE(openssl REQUIRED)
	INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
	
ENDIF(DISABLE_SSL)

SET(LIBTYPE SHARED)

IF(UNIX)
	IF(APPLE)
		IF(BUILD_IPHONE)
			ADD_DEFINITIONS(-DOSNAME="iPhone OS")
			SET(LIBTYPE STATIC)
		ELSE(BUILD_IPHONE)
			ADD_DEFINITIONS(-DOSNAME="Mac OS X")
		ENDIF(BUILD_IPHONE)
	ELSE(APPLE)
		ADD_DEFINITIONS(-DOSNAME="Unix")
	ENDIF(APPLE)
ELSE(UNIX)
	IF(WIN32)
		ADD_DEFINITIONS(-DOSNAME="Windows")
	ELSE(WIN32)
		ADD_DEFINITIONS(-DOSNAME="Unknown OS")
	ENDIF(WIN32)
ENDIF(UNIX)

IF(WIN32)
	SET(INCLUDE_PLAT Win32Deps/include)
ELSE(WIN32)
	SET(INCLUDE_PLAT /usr/local/include)
ENDIF(WIN32)

IF(EXTRA_UA)
	ADD_DEFINITIONS(-DEXTRA_UA="${EXTRA_UA}")
ENDIF(EXTRA_UA)

IF(SYS_MOWGLI)
	INCLUDE_DIRECTORIES(. contrib/libb64/include contrib/uriparser/include ${INCLUDE_PLAT})
ELSE(SYS_MOWGLI)
	INCLUDE_DIRECTORIES(. contrib/libb64/include contrib/uriparser/include contrib/libmowgli/src contrib/libmowgli/src/libmowgli ${INCLUDE_PLAT})
	IF(WIN32)
		SET(LIBMOWGLI_SRC_PLAT contrib/libmowgli/src/libmowgli/mowgli_module_win32.c contrib/libmowgli/src/libmowgli/win32_support.c)
	ELSE(WIN32)
		SET(LIBMOWGLI_SRC_PLAT contrib/libmowgli/src/libmowgli/mowgli_module_posix.c)
	ENDIF(WIN32)
	SET(LIBMOWGLI_SRCS contrib/libmowgli/src/libmowgli/mowgli_alloc.c contrib/libmowgli/src/libmowgli/mowgli_allocation_policy.c contrib/libmowgli/src/libmowgli/mowgli_allocator.c contrib/libmowgli/src/libmowgli/mowgli_argstack.c contrib/libmowgli/src/libmowgli/mowgli_bitvector.c contrib/libmowgli/src/libmowgli/mowgli_dictionary.c contrib/libmowgli/src/libmowgli/mowgli_error_backtrace.c contrib/libmowgli/src/libmowgli/mowgli_formatter.c contrib/libmowgli/src/libmowgli/mowgli_global_storage.c contrib/libmowgli/src/libmowgli/mowgli_hash.c contrib/libmowgli/src/libmowgli/mowgli_heap.c contrib/libmowgli/src/libmowgli/mowgli_hook.c contrib/libmowgli/src/libmowgli/mowgli_index.c contrib/libmowgli/src/libmowgli/mowgli_init.c contrib/libmowgli/src/libmowgli/mowgli_ioevent.c contrib/libmowgli/src/libmowgli/mowgli_list.c contrib/libmowgli/src/libmowgli/mowgli_logger.c contrib/libmowgli/src/libmowgli/mowgli_mempool.c contrib/libmowgli/src/libmowgli/mowgli_object.c contrib/libmowgli/src/libmowgli/mowgli_object_class.c contrib/libmowgli/src/libmowgli/mowgli_object_messaging.c contrib/libmowgli/src/libmowgli/mowgli_object_metadata.c contrib/libmowgli/src/libmowgli/mowgli_patricia.c contrib/libmowgli/src/libmowgli/mowgli_queue.c contrib/libmowgli/src/libmowgli/mowgli_random.c contrib/libmowgli/src/libmowgli/mowgli_signal.c contrib/libmowgli/src/libmowgli/mowgli_spinlock.c contrib/libmowgli/src/libmowgli/mowgli_string.c contrib/libmowgli/src/libmowgli/mowgli_thread.c ${LIBMOWGLI_SRC_PLAT}) 
	ADD_LIBRARY(mowgli STATIC ${LIBMOWGLI_SRCS})
ENDIF(SYS_MOWGLI)

SET(LIBB64_SRCS contrib/libb64/src/cdecode.c contrib/libb64/src/cencode.c)
ADD_LIBRARY(b64 STATIC ${LIBB64_SRCS})

SET(LIBURIPARSER_SRCS contrib/uriparser/src/UriCommon.c contrib/uriparser/src/UriCommon.h contrib/uriparser/src/UriCompare.c contrib/uriparser/src/UriEscape.c contrib/uriparser/src/UriFile.c contrib/uriparser/src/UriIp4.c contrib/uriparser/src/UriIp4Base.c contrib/uriparser/src/UriIp4Base.h contrib/uriparser/src/UriNormalize.c contrib/uriparser/src/UriNormalizeBase.c contrib/uriparser/src/UriNormalizeBase.h contrib/uriparser/src/UriParse.c contrib/uriparser/src/UriParseBase.c contrib/uriparser/src/UriParseBase.h contrib/uriparser/src/UriQuery.c contrib/uriparser/src/UriRecompose.c contrib/uriparser/src/UriResolve.c contrib/uriparser/src/UriShorten.c)
ADD_LIBRARY(uriparser STATIC ${LIBURIPARSER_SRCS})

IF(BUILD_INK)
	SET(LIBINK_SRCS libink/WTDictionary.cpp libink/WTDictionary.h
			libink/WTFormParser.cpp libink/WTFormParser.h)
	ADD_LIBRARY(ink ${LIBTYPE} ${LIBINK_SRCS})
	TARGET_LINK_LIBRARIES(ink mowgli uriparser)
ENDIF(BUILD_INK)

IF(BUILD_AMY)
	ADD_DEFINITIONS(-DHAVE_AMY)
	SET(LIBAMY_SRCS libAmy/amy_init.cpp libAmy/connect.cpp libAmy/connect_http.cpp libAmy/OAuth.cpp)
	ADD_LIBRARY(amy ${LIBTYPE} ${LIBAMY_SRCS})
	IF(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
		TARGET_LINK_LIBRARIES(amy ssl crypto socket ink b64)
	ELSE(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
		IF(WIN32)
			TARGET_LINK_LIBRARIES(amy ${OPENSSL_CRYPTO_LIBRARIES} ${OPENSSL_SSL_LIBRARIES} ws2_32 ink b64)
		ELSE(WIN32)
			TARGET_LINK_LIBRARIES(amy ${OPENSSL_CRYPTO_LIBRARIES} ${OPENSSL_SSL_LIBRARIES} ink b64)
		ENDIF(WIN32)
	ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
ENDIF(BUILD_AMY)

IF(BUILD_GWEN)
	ADD_DEFINITIONS(-DHAVE_GWEN)
	SET(LIBGWEN_SRCS libGwen/gwen_init.cpp
			libGwen/MIME.h
			libGwen/WTMIMEEncoder.cpp libGwen/WTMIMEEncoder.h)
	ADD_LIBRARY(gwen ${LIBTYPE} ${LIBGWEN_SRCS})
	TARGET_LINK_LIBRARIES(gwen ink amy b64)
ENDIF(BUILD_GWEN)



FILE(GLOB amy_head "libAmy/*.h")
FILE(GLOB gwen_head "libGwen/*.h")
FILE(GLOB ink_head "libink/*.h")
INSTALL(FILES ${amy_head} DESTINATION include/libAmy)
INSTALL(FILES ${gwen_head} DESTINATION include/libGwen)
INSTALL(FILES ${ink_head} DESTINATION include/libink)
INSTALL(TARGETS amy b64 gwen ink uriparser LIBRARY DESTINATION lib  ARCHIVE DESTINATION lib)
IF(SYS_MOWGLI)
ELSE(SYS_MOWGLI)
	FILE(GLOB mowgli_head "contrib/libmowgli/src/libmowgli/*.h")
	INSTALL(FILES ${mowgli_heade} DESTINATION include/libmowgli)
	INSTALL(TARGETS mowgli LIBRARY DESTINATION lib  ARCHIVE DESTINATION lib)
ENDIF(SYS_MOWGLI)
