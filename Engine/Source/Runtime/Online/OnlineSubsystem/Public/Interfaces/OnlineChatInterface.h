// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "OnlineDelegateMacros.h"
#include "OnlineSubsystemTypes.h"

/**
 * Id of a chat room
 */
typedef FString FChatRoomId;

/**
 * Info for a joined/created chat room
 */
class FChatRoomInfo
{
public:
	virtual ~FChatRoomInfo() {}

	virtual const FChatRoomId& GetRoomId() const = 0;
	virtual const TSharedRef<FUniqueNetId>& GetOwnerId() const = 0;
	virtual const FString& GetSubject() const = 0;
	virtual bool IsPrivate() const = 0;
	virtual bool IsJoined() const = 0;
};

/**
 * Member of a chat room
 */
class FChatRoomMember
{
public:
	virtual ~FChatRoomMember() {}

	virtual const TSharedRef<FUniqueNetId>& GetUserId() const = 0;
	virtual const FString& GetNickname() const = 0;
};

/**
* Chat message received from user/room
*/
class FChatMessage
{
public:
	virtual ~FChatMessage() {}

	virtual const TSharedRef<FUniqueNetId>& GetUserId() const = 0;
	virtual const FString& GetNickname() const = 0;
	virtual const FString& GetBody() const = 0;
	virtual const FDateTime& GetTimestamp() const = 0;
};

/**
 * Delegate used when joining a public chat room
 *
 * @param UserId the user that made the request
 * @param RoomId room that was requested
 * @param bWasSuccessful true if the async action completed without error, false if there was an error
 * @param Error string representing the error condition
 */
DECLARE_MULTICAST_DELEGATE_FourParams(FOnChatRoomJoinPublic, const FUniqueNetId& /*UserId*/, const FChatRoomId& /*RoomId*/, bool /*bWasSuccessful*/, const FString& /*Error*/);
typedef FOnChatRoomJoinPublic::FDelegate FOnChatRoomJoinPublicDelegate;

/**
 * Delegate used when exiting a chat room
 *
 * @param UserId the user that made the request
 * @param RoomId room that was requested
 * @param bWasSuccessful true if the async action completed without error, false if there was an error
 * @param Error string representing the error condition
 */
DECLARE_MULTICAST_DELEGATE_FourParams(FOnChatRoomExit, const FUniqueNetId& /*UserId*/, const FChatRoomId& /*RoomId*/, bool /*bWasSuccessful*/, const FString& /*Error*/);
typedef FOnChatRoomExit::FDelegate FOnChatRoomExitDelegate;

/**
 * Delegate used when another chat room member enters/joins
 *
 * @param UserId user currently in the room
 * @param RoomId room that member is in
 * @param MemberId the user that entered
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChatRoomMemberJoin, const FUniqueNetId& /*UserId*/, const FChatRoomId& /*RoomId*/, const FUniqueNetId& /*MemberId*/);
typedef FOnChatRoomMemberJoin::FDelegate FOnChatRoomMemberJoinDelegate;

/**
 * Delegate used when another chat room member exits
 *
 * @param UserId user currently in the room
 * @param RoomId room that member is in
 * @param MemberId the user that exited
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChatRoomMemberExit, const FUniqueNetId& /*UserId*/, const FChatRoomId& /*RoomId*/, const FUniqueNetId& /*MemberId*/);
typedef FOnChatRoomMemberExit::FDelegate FOnChatRoomMemberExitDelegate;

/**
 * Delegate used when another chat room member is updated
 *
 * @param UserId user currently in the room
 * @param RoomId room that member is in
 * @param MemberId the user that updated
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChatRoomMemberUpdate, const FUniqueNetId& /*UserId*/, const FChatRoomId& /*RoomId*/, const FUniqueNetId& /*MemberId*/);
typedef FOnChatRoomMemberUpdate::FDelegate FOnChatRoomMemberUpdateDelegate;

/**
 * Delegate used when a chat message is received from a chat room
 *
 * @param UserId user currently in the room
 * @param RoomId room that member is in
 * @param ChatMessage the message that was received
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChatRoomMessageReceived, const FUniqueNetId& /*UserId*/, const FChatRoomId& /*RoomId*/, const TSharedRef<FChatMessage>& /*ChatMessage*/);
typedef FOnChatRoomMessageReceived::FDelegate FOnChatRoomMessageReceivedDelegate;

/**
 * Delegate used when a private chat message is received from another user
 *
 * @param UserId user that received the message
 * @param ChatMessage the message that was received
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChatPrivateMessageReceived, const FUniqueNetId& /*UserId*/, const TSharedRef<FChatMessage>& /*ChatMessage*/);
typedef FOnChatPrivateMessageReceived::FDelegate FOnChatPrivateMessageReceivedDelegate;

/**
 * Interface class for user-user and user-room chat
 */
class IOnlineChat
{
public:
	
	/**
	 * Kick off request for joining a public chat room
	 * 
	 * @param UserId id of user that is joining
	 * @param RoomId name of room to join
	 * @param Nickname display name for the chat room. Name must be unique and is reserved for duration of join
	 *
	 * @return if successfully started the async operation
	 */
	virtual bool JoinPublicRoom(const FUniqueNetId& UserId, const FChatRoomId& RoomId, const FString& Nickname) = 0;

	/**
	 * Kick off request for exiting a previously joined chat room
	 * 
	 * @param UserId id of user that is exiting
	 * @param RoomId name of room to exit
	 *
	 * @return if successfully started the async operation
	 */
	virtual bool ExitRoom(const FUniqueNetId& UserId, const FChatRoomId& RoomId) = 0;

	/**
	 * Kick off request for sending a chat message to a joined chat room
	 * 
	 * @param UserId id of user that is sending the message
	 * @param RoomId name of room to send message to
	 * @param MsgBody plain text of message body
	 *
	 * @return if successfully started the async operation
	 */
	virtual bool SendRoomChat(const FUniqueNetId& UserId, const FChatRoomId& RoomId, const FString& MsgBody) = 0;

	/**
	 * Kick off request for sending a chat message privately between users
	 * 
	 * @param UserId id of user that is sending the message
	 * @param RecipientId id of user to send the chat to
	 * @param MsgBody plain text of message body
	 *
	 * @return if successfully started the async operation
	 */
	virtual bool SendPrivateChat(const FUniqueNetId& UserId, const FUniqueNetId& RecipientId, const FString& MsgBody) = 0;

	/**
	 * Get cached list of rooms that have been joined
	 * 
	 * @param UserId id of user to find
	 * @param OutRooms [out] list of room ids or empty if not found
	 */
	virtual void GetJoinedRooms(const FUniqueNetId& UserId, TArray<FChatRoomId>& OutRooms) = 0;

	/**
	 * Get cached room info for a room 
	 * 
	 * @param UserId id of user to find
	 * @param RoomId id of room to find
	 *
	 * @return RoomInfo information about a chat room or NULL if not found
	 */
	virtual TSharedPtr<FChatRoomInfo> GetRoomInfo(const FUniqueNetId& UserId, const FChatRoomId& RoomId) = 0;

	/**
	 * Get cached list of members currently joined in a chat room 
	 * 
	 * @param UserId id of user to find
	 * @param RoomId id of room to find
	 * @param OutRoomMembers [out] list of current members in the chat room
	 *
	 * @return true if found
	 */
	virtual bool GetMembers(const FUniqueNetId& UserId, const FChatRoomId& RoomId, TArray< TSharedRef<FChatRoomMember> >& OutMembers) = 0;

	/**
	 * Get cached member currently joined in a chat room 
	 * 
	 * @param UserId id of user to find
	 * @param RoomId id of room to find
	 * @param MemberId id of member to find
	 *
	 * @return member in room or NULL if not found
	 */
	virtual TSharedPtr<FChatRoomMember> GetMember(const FUniqueNetId& UserId, const FChatRoomId& RoomId, const FUniqueNetId& MemberId) = 0;

	/**
	 * Get cached list of chat messages for a currently joined chat room 
	 * 
	 * @param UserId id of user to find
	 * @param RoomId id of room to find
	 * @param NumMessages max number of messages to fetch from history (-1 for all cached messages)
	 * @param OutMessages [out] list of messages from the chat room history
	 *
	 * @return true if found
	 */
	virtual bool GetLastMessages(const FUniqueNetId& UserId, const FChatRoomId& RoomId, int32 NumMessages, TArray< TSharedRef<FChatMessage> >& OutMessages) = 0;

	// delegate callbacks (see declarations above)
	DEFINE_ONLINE_DELEGATE_FOUR_PARAM(OnChatRoomJoinPublic, const FUniqueNetId&, const FChatRoomId&, bool, const FString&);
	DEFINE_ONLINE_DELEGATE_FOUR_PARAM(OnChatRoomExit, const FUniqueNetId&, const FChatRoomId&, bool, const FString&);
	DEFINE_ONLINE_DELEGATE_THREE_PARAM(OnChatRoomMemberJoin, const FUniqueNetId&, const FChatRoomId&, const FUniqueNetId&);
	DEFINE_ONLINE_DELEGATE_THREE_PARAM(OnChatRoomMemberExit, const FUniqueNetId&, const FChatRoomId&, const FUniqueNetId&);
	DEFINE_ONLINE_DELEGATE_THREE_PARAM(OnChatRoomMemberUpdate, const FUniqueNetId&, const FChatRoomId&, const FUniqueNetId&);
	DEFINE_ONLINE_DELEGATE_THREE_PARAM(OnChatRoomMessageReceived, const FUniqueNetId&, const FChatRoomId&, const TSharedRef<FChatMessage>&);
	DEFINE_ONLINE_DELEGATE_TWO_PARAM(OnChatPrivateMessageReceived, const FUniqueNetId&, const TSharedRef<FChatMessage>&);
};