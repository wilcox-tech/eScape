/*
 * WTConnDelegate.h - interface of delegate class for WTConnection
 * libAmy, the Web as seen by
 * eScape
 * Wilcox Technologies, LLC
 *
 * Copyright (c) 2011 Wilcox Technologies, LLC. All rights reserved.
 * License: NCSA-WT
 */

#ifndef __LIBAMY_WT_CONNDELEGATE_H__
#define __LIBAMY_WT_CONNDELEGATE_H__

/*! The status of the connection as reported to update_status */
enum WTHTTPConnectionStatus
{
	WTHTTP_Error = -1,	/*! An error occurred during the last step. */
	WTHTTP_Resolving = 1,	/*! Resolving the domain name */
	WTHTTP_Connecting,	/*! Connecting to server */
	WTHTTP_Connected,	/*! Connected to the server */
	WTHTTP_Transferring,	/*! Transferring data to/from the server */
	WTHTTP_Finished,	/*! Connection has finished transferring. */
	WTHTTP_Closed,		/*! Connection closed.  It may be reopened. */
	WTHTTP_Cancelled	/*! Connection cancelled by user/API. */
};

class WTConnection;

/*!
	@class		WTConnDelegate
	@brief		The delegate to the WTConnection class.
 */
class WTConnDelegate
{
public:
	/*!
	@brief		Update the status of the connection.
	@details	This function provides the delegate object with a
			notification that the status of the connection is
			changing.
	@param		connection	The connection object.
	@param		status		The new status of <connection>.
					(See WTHTTPConnectionStatus.)
	 */
	virtual void update_status(WTConnection *connection, char status) = 0;
};

#endif /*!__LIBAMY_WT_CONNDELEGATE_H__*/
