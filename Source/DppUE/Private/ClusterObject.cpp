// Fill out your copyright notice in the Description page of Project Settings.


#include "ClusterObject.h"

#include "AudioDecompress.h"
#include "AudioThread.h"
#include "codecvt"

#define DPP_LOG_STR(x) UE_LOG(LogTemp, Display, TEXT("[DPP]: %s"), *FString(x.c_str()));
#define DPP_LOG_FSTR(x) UE_LOG(LogTemp, Display, TEXT("[DPP]: %s"), *FString(x));

#define DPPUE_LOG_STR(x) UE_LOG(LogTemp, Display, TEXT("[DPP-UE]: %s"), *FString(x.c_str()));
#define DPPUE_LOG_FSTR(x) UE_LOG(LogTemp, Display, TEXT("[DPP-UE]: %s"), *FString(x));

#define DPPUE_WARN_STR(x) UE_LOG(LogTemp, Warning, TEXT("[DPP-UE]: %s"), *FString(x.c_str()));
#define DPPUE_WARN_FSTR(x) UE_LOG(LogTemp, Warning, TEXT("[DPP-UE]: %s"), *FString(x));

#define DPPUE_ERR_STR(x) UE_LOG(LogTemp, Error, TEXT("[DPP-UE]: %s"), *FString(x.c_str()));
#define DPPUE_ERR_FSTR(x) UE_LOG(LogTemp, Error, TEXT("[DPP-UE]: %s"), *FString(x));

dpp::cluster* clusterRef{};
dpp::discord_client* bot_client{};
std::thread* bot_thread{};

UClusterObject::UClusterObject()
{
}

void UClusterObject::BeginDestroy()
{

	if(!bot_alive)
	{
		UObject::BeginDestroy();
		return;
	}
	
	commands_to_register.Empty();

	// Wipe bot client ref.
	if(bot_client)
		delete bot_client;

	if(clusterRef)
	{
		stop_bot = true;
		bot_thread->join();

		clusterRef = nullptr;
		bot_thread = nullptr;
	}

	bot_alive = false;
	
	UObject::BeginDestroy();
}

void UClusterObject::CreateBot(const FString& token, const bool enableMessageIntent, const bool enableGuildIntent, const bool enableGuildPresenceIntent)
{
	if(token.IsEmpty())
	{
		DPPUE_ERR_FSTR("The token is empty. Aborting bot start.");
		return;
	}
	
	uint32_t intent = dpp::i_default_intents;
 
	if(enableMessageIntent)
		intent = intent | dpp::i_message_content;

	if(enableGuildIntent)
		intent = intent | dpp::i_guild_members;

	if(enableGuildPresenceIntent)
		intent = intent | dpp::i_guild_presences;
	
	// Create bot.
	clusterRef = new dpp::cluster(std::string(TCHAR_TO_UTF8(*token)), intent);

	clusterRef->on_log([](const dpp::log_t& event)
	{
		DPP_LOG_STR(event.message);
	});

	clusterRef->on_slashcommand([this](const dpp::slashcommand_t& event)
	{
		FSlashcommand_Event command_event;
		command_event.command_name = FString(event.command.get_command_name().c_str());
		command_event.issuing_user = FString(event.command.get_issuing_user().global_name.c_str());

		// We can't do a lot of UE stuff on a separate thread, so we need to make sure that "OnSlashCommand" happens on the GameThread.
		AsyncTask(ENamedThreads::Type::GameThread, [this, event, command_event]
		{
			FSlashcommand_Reply command_reply = OnSlashcommand(command_event);

			const dpp::message msg{GenerateDPPMessage(command_reply.reply)};

			event.reply(msg);
		});
	});

	clusterRef->on_message_create([this](const dpp::message_create_t& event)
	{
		if(event.msg.author.id.empty() || event.msg.author.id == clusterRef->me.id)
		{
			DPPUE_WARN_FSTR("Message was sent by an empty ID or was sent by self. Ignoring.");
			return;
		}
		
		FMessage_event message_event;
		FDiscordMessage message;
		message.content = FString(event.msg.content.c_str());
		message.isEmpherial = false;
		message_event.message = message;
		message_event.message_creator = FString(event.msg.author.global_name.c_str());
		
		OnMessageCreate.Broadcast(message_event);
	});

	clusterRef->on_button_click([this](const dpp::button_click_t& event)
	{
		FButtonClick_Event buttonclick_event;
		buttonclick_event.custom_id = FString(event.custom_id.c_str());
		buttonclick_event.issuing_user = FString(event.command.get_issuing_user().global_name.c_str());
		buttonclick_event.raw_event = event;

		//event.thinking(true);
		
		AsyncTask(ENamedThreads::Type::GameThread, [this, event, buttonclick_event]
		{
			OnButtonClick.Broadcast(buttonclick_event);
		});
	});
	
	// When bot is ready, register all our commands and fire OnClusterReady.
	clusterRef->on_ready([this](const dpp::ready_t& event)
	{
		if(dpp::run_once <struct register_bot_commands>())
		{
			std::vector<dpp::slashcommand> temp_commands;

			for(FSlashcommand_Data command_data : commands_to_register)
				temp_commands.emplace_back(command_data.name_to_string(), command_data.description_to_string(), clusterRef->me.id);
			
			clusterRef->global_bulk_command_create(temp_commands);
		}

		bot_client = event.from;
		bot_alive = true;
		OnClusterReady.Broadcast();
	});
}

void UClusterObject::StartBot()
{
	// Now, let's start.
	bot_thread = new std::thread([this]
	{
		if(clusterRef != nullptr)
			clusterRef->start(dpp::st_return);

		while(!stop_bot)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	});
}

void UClusterObject::CreateCommand(FSlashcommand_Data command)
{
	commands_to_register.Add(command);
}

void UClusterObject::CreateCommands(TArray<FSlashcommand_Data> commands)
{
	commands_to_register.Append(commands);
}

FSlashcommand_Reply UClusterObject::OnSlashcommand_Implementation(FSlashcommand_Event event)
{
	FSlashcommand_Reply reply;
	FDiscordMessage message;
	
	message.content = "This is a generic reply, this message will appear for every command. You need to override this function.";
	message.isEmpherial = true;
	reply.reply = message;
	return reply;
}

void UClusterObject::ButtonClickReply(FButtonClick_Event button_event, FButtonClick_Reply button_reply)
{
	const dpp::message msg{GenerateDPPMessage(button_reply.reply)};

	if(button_reply.editInteractedMessage)
	{
		button_event.raw_event.reply(dpp::ir_update_message, msg);
	} else
	{
		button_event.raw_event.reply(msg);
	}
}

bool UClusterObject::JoinVoiceChannel(FDiscordSnowflake GuildID, FDiscordSnowflake UserID)
{
	dpp::guild* guild = dpp::find_guild(GuildID.string_id_to_snowflake());

	if(!guild)
	{
		DPPUE_WARN_FSTR("Passed an invalid guild id for joining a voice channel. Double check the GuildID is valid.");
		return false;
	}

	if(!guild->connect_member_voice(UserID.string_id_to_snowflake()))
	{
		DPPUE_WARN_FSTR("Failed to connect to the user's voice channel.");
		return false;
	}

	return true;
}

void UClusterObject::LeaveVoiceChannel(FDiscordSnowflake VoiceChannel)
{
	bot_client->disconnect_voice(VoiceChannel.string_id_to_snowflake());
}

bool UClusterObject::PlayAudioInGuild(FDiscordSnowflake GuildID, USoundWave* SoundWave)
{
	dpp::voiceconn* v = bot_client->get_voice(GuildID.string_id_to_snowflake());

	if(!v || !v->voiceclient || !v->voiceclient->is_ready())
	{
		DPPUE_WARN_FSTR("The bot is not currently in a voice channel.");
		return false;
	}
	
	if (SoundWave->bProcedural)
	{
		DPPUE_WARN_FSTR("There is currently no support for procedural sounds.");
		return false;
	}

	if (SoundWave->RawPCMData == nullptr)
	{
		//UE_LOG(LogTemp, Display, TEXT("[DPP-UE]: %s"), *FString("Could not find any PCM data for the SoundWave passed into 'PlayAudioInGuild'. Calculationg PCM Data..."));
		DPPUE_LOG_FSTR("Could not find any PCM data for the SoundWave passed into 'PlayAudioInGuild'. Calculationg PCM Data...");

		// We have to tell the GameThread to do all the decompressing. It would be the audio thread but I can't seem to get access to that thread.
		FAudioThread::RunCommandOnAudioThread([this, SoundWave, v]
		{

			FAudioDevice* AudioDevice = GEngine->GetMainAudioDeviceRaw();

			if(!AudioDevice)
			{
				DPPUE_ERR_FSTR("Could not find a main audio device. Raw PCM Data could not be gathered.");
				return false;
			}
			
			EDecompressionType DecompressionType = SoundWave->DecompressionType;
			SoundWave->DecompressionType = DTYPE_Native;

			FName format = SoundWave->GetRuntimeFormat();
			
			FByteBulkData* Bulk = SoundWave->GetCompressedData(format, SoundWave->GetPlatformCompressionOverridesForCurrentPlatform());

			if(!Bulk)
			{
				DPPUE_ERR_FSTR("Failed to get the compressed data from the sound wave.");
				return false;
			}
		
			SoundWave->InitAudioResource(*Bulk);
	
			if (SoundWave->DecompressionType != DTYPE_RealTime || SoundWave->CachedRealtimeFirstBuffer == nullptr)
			{
				DPPUE_LOG_FSTR("Need to de-compress SoundWave. Proceeding with operation...");
			
				const float timeStarted = GetWorld()->RealTimeSeconds * 1000;
			
				FAsyncAudioDecompress TempDecompress(SoundWave, 128, AudioDevice);
				TempDecompress.StartSynchronousTask();

				const float timeEnded = GetWorld()->RealTimeSeconds * 1000;

				UE_LOG(LogTemp, Warning, TEXT("Finished de-compressing. It took %f ms to decompress the audio."), timeEnded - timeStarted);
			}

			SoundWave->DecompressionType = DecompressionType;

			// Because the data is done, let's start sending audio and get ourselves out of the game thread.
			CommenceAudioSend(v, SoundWave);
			
			return true;
		});
	}

	CommenceAudioSend(v, SoundWave);

	return true; 
	
}

void UClusterObject::SetBotStatus(FStatus status)
{
	clusterRef->set_presence(dpp::presence(status.status_type_to_status(), status.activity_type_to_status(), status.status_to_string()));
}

void UClusterObject::SendMessageToChannel(FDiscordMessage message, FOnMessageSent messageCallback)
{
	const dpp::message msg{GenerateDPPMessage(message)};
	
	clusterRef->message_create(msg, [&messageCallback](const dpp::confirmation_callback_t& callback)
	{
		DPP_LOG_FSTR("Message create callback!");
		// Promoted to variable to silence IDEs.
		bool executed = messageCallback.ExecuteIfBound(callback.is_error());
	});
}

void UClusterObject::CommenceAudioSend(const dpp::voiceconn* Voiceconn, const USoundWave* SoundWave)
{
	// RawPCMData "claims" 16 bit and proceeds to give us 8 bit.
	// This is the silliest thing I've ever read, so we have to do reinterpret_cast.
	Voiceconn->voiceclient->send_audio_raw(reinterpret_cast<uint16_t*>(SoundWave->RawPCMData), SoundWave->RawPCMDataSize);
}

dpp::message UClusterObject::GenerateDPPMessage(const FDiscordMessage& discord_message)
{
	dpp::message msg(discord_message.channel_id.string_id_to_snowflake(), TCHAR_TO_UTF8(*discord_message.content));
	
	if(!discord_message.image_url.IsEmpty())
	{
		/*
		 * If you get an error here from calling this in an event,
		 * your file may be too big for your internet (and possibly storage drive) to load the file in time!
		 * Look to use the "thinking" event if this keeps happening, or try to compress your image more!
		 */
		try {
			std::ifstream ifs(StringCast<ANSICHAR>(*discord_message.image_url).Get(), std::ios::binary);
			msg.add_file("image.png", std::string(std::istreambuf_iterator(ifs), std::istreambuf_iterator<char>()));
		}
		catch (const std::exception& e) {
			UE_LOG(LogTemp, Display, TEXT("[DPP-UE]: Failed to get data from image file. %s"), *FString(e.what()));
		}
	}
	
	for(auto row : discord_message.component_rows)
	{
		dpp::component row_comp;
		
		for(auto fmsg_comp : row.components)
		{
			dpp::component msg_comp;
			msg_comp.set_label(std::string(TCHAR_TO_UTF8(*fmsg_comp.component_label)))
			.set_style(dpp::cos_primary)
			.set_id(std::string(TCHAR_TO_UTF8(*fmsg_comp.component_id)));

			msg_comp.set_disabled(!fmsg_comp.component_enabled);

			row_comp.add_component(msg_comp);
		}

		msg.add_component(row_comp);
	}

	if(discord_message.isEmpherial)
		msg.set_flags(dpp::m_ephemeral);

	return msg;
}
