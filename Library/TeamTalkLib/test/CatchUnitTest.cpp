/*
 * Copyright (c) 2005-2018, BearWare.dk
 * 
 * Contact Information:
 *
 * Bjoern D. Rasmussen
 * Kirketoften 5
 * DK-8260 Viby J
 * Denmark
 * Email: contact@bearware.dk
 * Phone: +45 20 20 54 59
 * Web: http://www.bearware.dk
 *
 * This source code is part of the TeamTalk SDK owned by
 * BearWare.dk. Use of this file, or its compiled unit, requires a
 * TeamTalk SDK License Key issued by BearWare.dk.
 *
 * The TeamTalk SDK License Agreement along with its Terms and
 * Conditions are outlined in the file License.txt included with the
 * TeamTalk SDK distribution.
 *
 */

#include "catch.hpp"

#include <ace/ACE.h>

#include "TTUnitTest.h"

#include <codec/OggOutput.h>

#if defined(WIN32)
#include <ace/Init_ACE.h>
#include <assert.h>
static class WinInit
{
public:
    WinInit()
    {
        int ret = ACE::init();
        assert(ret >= 0);
    }
    ~WinInit()
    {
        int ret = ACE::fini();
        assert(ret >= 0);
    }
} wininit;
#endif

TEST_CASE( "Init TT", "" ) {
    TTInstance* ttinst;
    REQUIRE( (ttinst = TT_InitTeamTalkPoll()) );
    REQUIRE( TT_CloseTeamTalk(ttinst) );
}

TEST_CASE( "Ogg Write", "" ) {
    SpeexEncFile spxfile;
    REQUIRE( spxfile.Open(ACE_TEXT("/foo.spx"), 1, DEFAULT_SPEEX_COMPLEXITY, 7, 32000, 48000, false) == false);
}

TEST_CASE( "Record mux") {
    std::vector<TTInstance*> clients(2);
    for (auto i=0;i<clients.size();++i)
    {
        REQUIRE((clients[i] = TT_InitTeamTalkPoll()));
        REQUIRE(InitSound(clients[i], SHARED_INPUT));
        REQUIRE(Connect(clients[i], ACE_TEXT("127.0.0.1"), 10333, 10333));
        REQUIRE(Login(clients[i], ACE_TEXT("MyNickname"), ACE_TEXT("guest"), ACE_TEXT("guest")));

        if (i == 0)
        {
            AudioCodec audiocodec = {};
            audiocodec.nCodec = OPUS_CODEC;
            audiocodec.opus.nApplication = OPUS_APPLICATION_VOIP;
            audiocodec.opus.nTxIntervalMSec = 240;
            audiocodec.opus.nFrameSizeMSec = 120;
            audiocodec.opus.nBitRate = OPUS_MIN_BITRATE;
            audiocodec.opus.nChannels = 2;
            audiocodec.opus.nComplexity = 10;
            audiocodec.opus.nSampleRate= 48000;
            audiocodec.opus.bDTX = true;
            audiocodec.opus.bFEC = true;
            audiocodec.opus.bVBR = false;
            audiocodec.opus.bVBRConstraint = false;

            Channel chan = MakeChannel(clients[i], ACE_TEXT("foo"), TT_GetRootChannelID(clients[i]), audiocodec);
            REQUIRE(WaitForCmdSuccess(clients[i], TT_DoJoinChannel(clients[i], &chan)));
        }
        else
        {
            REQUIRE(WaitForCmdSuccess(clients[i], TT_DoJoinChannelByID(clients[i], TT_GetMyChannelID(clients[0]), ACE_TEXT(""))));
        }
    }

    Channel chan;
    REQUIRE(TT_GetChannel(clients[1], TT_GetMyChannelID(clients[1]), &chan));

    REQUIRE(TT_EnableVoiceTransmission(clients[0], true));
    WaitForEvent(clients[0], CLIENTEVENT_NONE, nullptr, 100);
    REQUIRE(TT_EnableVoiceTransmission(clients[0], false));

    REQUIRE(TT_StartRecordingMuxedAudioFile(clients[1], &chan.audiocodec, ACE_TEXT("MyMuxFile.wav"), AFF_WAVE_FORMAT));
    
    REQUIRE(TT_DBG_SetSoundInputTone(clients[0], STREAMTYPE_VOICE, 500));
    REQUIRE(TT_EnableVoiceTransmission(clients[0], true));
    WaitForEvent(clients[0], CLIENTEVENT_NONE, nullptr, 2500);
    REQUIRE(TT_EnableVoiceTransmission(clients[0], false));

    REQUIRE(TT_DBG_SetSoundInputTone(clients[1], STREAMTYPE_VOICE, 600));
    REQUIRE(TT_EnableVoiceTransmission(clients[1], true));
    WaitForEvent(clients[1], CLIENTEVENT_NONE, nullptr, 2500);
    REQUIRE(TT_EnableVoiceTransmission(clients[1], false));
    
    WaitForEvent(clients[1], CLIENTEVENT_NONE, nullptr, 10000);
    
    REQUIRE(TT_StopRecordingMuxedAudioFile(clients[1]));

    for(auto c : clients)
        REQUIRE(TT_CloseTeamTalk(c));
}
