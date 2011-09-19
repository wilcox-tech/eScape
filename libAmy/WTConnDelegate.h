/*
 * WTConnDelegate.h - delegate class for WTConnection
 * Auctions
 * Wilcox Technologies
 *
 * Copyright (c) 2011 Wilcox Technologies. All rights reserved.
 * License: internal use only
 */

#ifndef __WT_CONNECTION_DELEGATE_H_
#define __WT_CONNECTION_DELEGATE_H_

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

#endif /*!__WT_CONNECTION_DELEGATE_H_*/
