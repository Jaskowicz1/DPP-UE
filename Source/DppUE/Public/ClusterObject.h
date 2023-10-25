// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "CoreMinimal.h"
#include "CoreUObject/Public/UObject/Object.h"

THIRD_PARTY_INCLUDES_START
#include <dpp/dpp.h>
THIRD_PARTY_INCLUDES_END

#include "ClusterObject.generated.h"

#pragma region Structs

/**
 * @brief A discord snowflake.
 */
USTRUCT(BlueprintType)
struct FDiscordSnowflake
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Discord|Snowflake")
	FString snowflake_id;

	FORCEINLINE dpp::snowflake string_id_to_snowflake() const
	{
		if(!snowflake_id.IsNumeric() || snowflake_id.Contains("-"))
		{
			UE_LOG(LogTemp, Error, TEXT("Can't get snowflake from id. Make sure the id is a number and isn't negative."));
			return dpp::snowflake(0);
		}

		const uint64 numerical_id = FCString::Strtoui64(*snowflake_id, nullptr, 10);
		
		return dpp::snowflake(numerical_id);
	}
};

/**
 * @brief A message component (buttons).
 */
USTRUCT(BlueprintType)
struct FMessageComponent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Discord|Component")
	FString component_label;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Component")
	FString component_id;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Component")
	bool component_enabled;

	std::string label_to_string() const
	{
		return std::string(TCHAR_TO_UTF8(*component_label));
	}

	std::string id_to_string() const
	{
		return std::string(TCHAR_TO_UTF8(*component_id));
	}
};

/**
 * @brief A row of message components.
 */
USTRUCT(BlueprintType)
struct FMessageComponentRow
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Discord|Component")
	TArray<FMessageComponent> components;
};

/**
 * @brief A message for discord.
 */
USTRUCT(BlueprintType)
struct FDiscordMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Discord|Message")
	FDiscordSnowflake channel_id;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Message")
	FString content;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Message")
	FString image_url;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Message")
	TArray<FMessageComponentRow> component_rows;

	/**
	 * @brief Should the message be hidden to everyone but the command executor?
	 */
	UPROPERTY(BlueprintReadWrite, Category="Discord|Message")
	bool isEmpherial;
};

/**
 * @brief The information of a slashcommand to register.
 */
USTRUCT(BlueprintType)
struct FSlashcommand_Data
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Discord|Command")
	FString command_name;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Command")
	FString command_description;

	std::string name_to_string() const
	{
		return std::string(TCHAR_TO_UTF8(*command_name));
	}

	std::string description_to_string() const
	{
		return std::string(TCHAR_TO_UTF8(*command_description));
	}
};

/**
 * @brief The information from a message event.
 */
USTRUCT(BlueprintType)
struct FSlashcommand_Event
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Discord|Events|Command")
	FString command_name;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Events|Command")
	FString issuing_user;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Events|Command")
	FDiscordSnowflake issuing_user_id;

	std::string name_to_string() const
	{
		return std::string(TCHAR_TO_UTF8(*command_name));
	}

	std::string issuing_user_to_string() const
	{
		return std::string(TCHAR_TO_UTF8(*issuing_user));
	}
};

/**
 * @brief The information to use for replying from a slash command.
 */
USTRUCT(BlueprintType)
struct FSlashcommand_Reply
{
	GENERATED_BODY()

	/**
	 * @brief The message to reply with.
	 */
	UPROPERTY(BlueprintReadWrite, Category="Discord|Command")
	FDiscordMessage reply;
};

/**
 * @brief The information from a button click event.
 */
USTRUCT(BlueprintType)
struct FButtonClick_Event
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Discord|Events|Button")
	FString custom_id;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Events|Button")
	FString issuing_user;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Events|Button")
	FDiscordSnowflake issuing_user_id;

	dpp::button_click_t raw_event;

	std::string custom_id_to_string() const
	{
		return std::string(TCHAR_TO_UTF8(*custom_id));
	}

	std::string issuing_user_to_string() const
	{
		return std::string(TCHAR_TO_UTF8(*issuing_user));
	}
};

/**
 * @brief The information to use for replying from to a button click.
 */
USTRUCT(BlueprintType)
struct FButtonClick_Reply
{
	GENERATED_BODY()

	/**
	 * @brief The message to reply with.
	 */
	UPROPERTY(BlueprintReadWrite, Category="Discord|Button")
	FDiscordMessage reply;

	/**
	 * @brief Should the message be hidden to everyone but the command executor?
	 */
	UPROPERTY(BlueprintReadWrite, Category="Discord|Button")
	bool editInteractedMessage;
};

/**
 * @brief The information from a slashcommand event.
 */
USTRUCT(BlueprintType)
struct FMessage_event
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Discord|Events|Message")
	FDiscordMessage message;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Events|Message")
	FString message_creator;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Events|Message")
	FDiscordSnowflake message_creator_id;

	std::string message_creator_to_string() const
	{
		return std::string(TCHAR_TO_UTF8(*message_creator));
	}
};

UENUM(BlueprintType)
enum class EPresenceStatusType : uint8
{
	PS_Offline		UMETA(DisplayName="Offline"),
	PS_Online		UMETA(DisplayName="Online"),
	PS_DND			UMETA(DisplayName="Do not Disturb"),
	PS_Idle			UMETA(DisplayName="Idle"),
	PS_Invisible	UMETA(DisplayName="Invisible"),
};

UENUM(BlueprintType)
enum class EActivityType : uint8
{
	AT_GAME			UMETA(DisplayName="Playing..."),
	AT_STREAMING	UMETA(DisplayName="Streaming..."),
	AT_LISTENING	UMETA(DisplayName="Listening to..."),
	AT_WATCHING		UMETA(DisplayName="Watching..."),
	AT_CUSTOM		UMETA(DisplayName="Custom (Does not work)"),
	AT_COMPETING	UMETA(DisplayName="Competing in..."),
};

/**
 * @brief The information from a slashcommand event.
 */
USTRUCT(BlueprintType)
struct FStatus
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Discord|Cluster")
	EPresenceStatusType status_type;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Cluster")
	EActivityType activity_type;

	UPROPERTY(BlueprintReadWrite, Category="Discord|Cluster")
	FString status;

	dpp::presence_status status_type_to_status() const
	{
		return static_cast<dpp::presence_status>(status_type);
	}
	
	dpp::activity_type activity_type_to_status() const
	{
		return static_cast<dpp::activity_type>(activity_type);
	}

	std::string status_to_string() const
	{
		return std::string(TCHAR_TO_UTF8(*status));
	}
};

#pragma endregion

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClusterReady);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlashcommand, const FSlashcommand_Event&, event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageCreate, const FMessage_event&, event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonClick, const FButtonClick_Event&, event);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMessageSent, bool, success);

/**
 * @brief The cluster object that your bot runs off. This manages all the back-end work with DPP and handles a lot of features for you.
 * @note Blueprints does not have all the information that C++ does. If you are wanting to create complicated logic then seek to use C++.
 */
UCLASS(Blueprintable, BlueprintType)
class DPPUE_API UClusterObject : public UObject
{
	GENERATED_BODY()

public:

	UClusterObject();

	virtual void BeginDestroy() override;

#pragma region Functions

	UFUNCTION(BlueprintCallable, Category = "Discord|Cluster")
	void CreateBot(const FString& token, const bool enableMessageIntent, const bool enableGuildIntent, const bool enableGuildPresenceIntent);

	UFUNCTION(BlueprintCallable, Category="Discord|Cluster")
	void StartBot();

	//UFUNCTION(BlueprintCallable, Category="Discord|Cluster")
	//void StopBot();

	UFUNCTION(BlueprintCallable, Category="Discord|Commands")
	void CreateCommand(FSlashcommand_Data command);

	UFUNCTION(BlueprintCallable, Category="Discord|Commands")
	void CreateCommands(TArray<FSlashcommand_Data> commands);

	UFUNCTION(BlueprintNativeEvent, Category="Discord|Events")
	FSlashcommand_Reply OnSlashcommand(FSlashcommand_Event event);

	//UFUNCTION(BlueprintNativeEvent, Category="Discord|Events")
	//FButtonClick_Reply OnButtonClick(FButtonClick_Event event);

	UFUNCTION(BlueprintCallable, Category="Discord|Events")
	void ButtonClickReply(FButtonClick_Event button_event, FButtonClick_Reply button_reply);

	UFUNCTION(BlueprintCallable, Category="Discord|Audio")
	bool JoinVoiceChannel(FDiscordSnowflake GuildID, FDiscordSnowflake VoiceChannelID);

	UFUNCTION(BlueprintCallable, Category="Discord|Audio")
	void LeaveVoiceChannel(FDiscordSnowflake GuildID);

	UFUNCTION(BlueprintCallable, Category="Discord|Audio")
	bool PlayAudioInGuild(FDiscordSnowflake GuildID, USoundWave* SoundWave);

	UFUNCTION(BlueprintCallable, Category="Discord|Cluster")
	void SetBotStatus(FStatus status);
	
	UFUNCTION(BlueprintCallable, Category="Discord|Messages")
	void SendMessageToChannel(FDiscordMessage message, FOnMessageSent messageCallback);
	
#pragma endregion

#pragma region Delegates

	UPROPERTY(BlueprintAssignable, Category="Discord|Events")
	FClusterReady OnClusterReady;

	UPROPERTY(BlueprintAssignable, Category="Discord|Events")
	FOnMessageCreate OnMessageCreate;
	
	UPROPERTY(BlueprintAssignable, Category="Discord|Events")
	FOnButtonClick OnButtonClick;

#pragma endregion

private:

	void CommenceAudioSend(const dpp::voiceconn* Voiceconn, const USoundWave* SoundWave);

	UPROPERTY()
	TArray<FSlashcommand_Data> commands_to_register;

	std::atomic<bool> stop_bot;

	// This is to stop UE from trying to destroy ClusterObject stuff when it's not alive.
	std::atomic<bool> bot_alive;

	dpp::message GenerateDPPMessage(const FDiscordMessage& discord_message);
};
