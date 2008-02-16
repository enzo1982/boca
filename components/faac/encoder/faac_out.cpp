 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "faac_out.h"
#include "config.h"
#include "dllinterface.h"

const String &BoCA::FAACOut::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>FAAC MP4/AAC Encoder</name>		\
	    <version>1.0</version>			\
	    <id>mp4-out</id>				\
	    <type>encoder</type>			\
	    <format>					\
	      <name>MP4 Audio Files</name>		\
	      <extension>m4a</extension>		\
	      <extension>m4b</extension>		\
	      <extension>m4r</extension>		\
	      <extension>mp4</extension>		\
	      <extension>3gp</extension>		\
	    </format>					\
	    <format>					\
	      <name>Advanced Audio Files</name>		\
	      <extension>aac</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

ConfigureFAAC	*configLayer = NIL;

Void smooth::AttachDLL(Void *instance)
{
	LoadFAACDLL();
	LoadMP4v2DLL();

	configLayer = new ConfigureFAAC();
}

Void smooth::DetachDLL()
{
	Object::DeleteObject(configLayer);

	FreeFAACDLL();
	FreeMP4v2DLL();
}

BoCA::FAACOut::FAACOut()
{
}

BoCA::FAACOut::~FAACOut()
{
}

Bool BoCA::FAACOut::Activate()
{
	if (format.channels > 2)
	{
		Utilities::ErrorMessage("BonkEnc does not support more than 2 channels!");

		errorState = True;

		return False;
	}

	Config	*config = Config::Get();

	if (config->enable_mp4)
	{
		if (GetTempFile(format.outfile) != format.outfile)
		{
			File	 mp4File(format.outfile);

			mp4File.Delete();
		}
	}

	unsigned long	 samplesSize	= 0;
	unsigned long	 bufferSize	= 0;

	handle = ex_faacEncOpen(format.rate, format.channels, &samplesSize, &bufferSize);

	outBuffer.Resize(bufferSize);
	samplesBuffer.Resize(samplesSize);

	fConfig = ex_faacEncGetCurrentConfiguration(handle);

	fConfig->mpegVersion	= config->enable_mp4 ? MPEG4 : config->GetIntValue("FAAC", "MPEGVersion", 0);
	fConfig->aacObjectType	= config->GetIntValue("FAAC", "AACType", 2);
	fConfig->allowMidside	= config->GetIntValue("FAAC", "AllowJS", 1);
	fConfig->useTns		= config->GetIntValue("FAAC", "UseTNS", 0);
	fConfig->bandWidth	= config->GetIntValue("FAAC", "BandWidth", 16000);

	if (config->enable_mp4) fConfig->outputFormat	= 0; // Raw AAC frame headers

	if (config->GetIntValue("FAAC", "SetQuality", 1)) fConfig->quantqual	= config->GetIntValue("FAAC", "AACQuality", 100);
	else						  fConfig->bitRate	= config->GetIntValue("FAAC", "Bitrate", 64) * 1000;

	if (format.bits == 8)	fConfig->inputFormat	= FAAC_INPUT_16BIT;
	if (format.bits == 16)	fConfig->inputFormat	= FAAC_INPUT_16BIT;
	if (format.bits == 24)	fConfig->inputFormat	= FAAC_INPUT_32BIT;
	if (format.bits == 32)	fConfig->inputFormat	= FAAC_INPUT_FLOAT;

	ex_faacEncSetConfiguration(handle, fConfig);

	if (config->enable_mp4)
	{
		mp4File		= ex_MP4CreateEx(GetTempFile(format.outfile), 0, 0, 1, 1, NIL, 0, NIL, 0);
		mp4Track	= ex_MP4AddAudioTrack(mp4File, format.rate, MP4_INVALID_DURATION, MP4_MPEG4_AUDIO_TYPE);	

		ex_MP4SetAudioProfileLevel(mp4File, 0x0F);

		unsigned char	*buffer = NIL;

		ex_faacEncGetDecoderSpecificInfo(handle, &buffer, &bufferSize);

		ex_MP4SetTrackESConfiguration(mp4File, mp4Track, (u_int8_t *) buffer, bufferSize);

		frameSize	= samplesSize / format.channels;

		totalSamples	= 0;
		encodedSamples	= 0;
		delaySamples	= frameSize;
	}

	packageSize	= samplesSize * (format.bits / 8);

	if (!config->enable_mp4)
	{
		if ((format.artist != NIL || format.title != NIL) && config->enable_id3v2 && config->enable_id3 && config->GetIntValue("FAAC", "AllowID3v2", 0))
		{
			Buffer<unsigned char>	 id3Buffer;
			Int			 size = format.RenderID3Tag(2, id3Buffer);

			driver->WriteData(id3Buffer, size);
		}
	}

	return True;
}

Bool BoCA::FAACOut::Deactivate()
{
	Config	*config = Config::Get();

	unsigned long	 bytes = 0;

	do
	{
		bytes = ex_faacEncEncode(handle, NULL, 0, outBuffer, outBuffer.Size());

		if (config->enable_mp4)
		{
			if (bytes > 0)
			{
				Int		 samplesLeft	= totalSamples - encodedSamples + delaySamples;
				MP4Duration	 dur		= samplesLeft > frameSize ? frameSize : samplesLeft;
				MP4Duration	 ofs		= encodedSamples > 0 ? 0 : delaySamples;

				ex_MP4WriteSample(mp4File, mp4Track, (u_int8_t *) (unsigned char *) outBuffer, bytes, dur, ofs, true);

				encodedSamples += dur;
			}
		}
		else
		{
			driver->WriteData(outBuffer, bytes);
		}
	}
	while (bytes > 0);

	ex_faacEncClose(handle);

	if (config->enable_mp4)
	{
		if (config->enable_mp4meta)
		{
			char	*prevOutFormat = String::SetOutputFormat(config->mp4meta_encoding);

			if (config->default_comment != NIL) ex_MP4SetMetadataComment(mp4File, config->default_comment);

			if (format.artist != NIL || format.title != NIL)
			{
				if (format.title != NIL)	ex_MP4SetMetadataName(mp4File, format.title);
				if (format.artist != NIL)	ex_MP4SetMetadataArtist(mp4File, format.artist);
				if (format.year > 0)		ex_MP4SetMetadataYear(mp4File, String::FromInt(format.year));
				if (format.album != NIL)	ex_MP4SetMetadataAlbum(mp4File, format.album);
				if (format.genre != NIL)	ex_MP4SetMetadataGenre(mp4File, format.genre);
				if (format.track > 0)		ex_MP4SetMetadataTrack(mp4File, format.track, 0);
			}

			String::SetOutputFormat(prevOutFormat);
		}

		ex_MP4Close(mp4File);

		ex_MP4Optimize(GetTempFile(format.outfile), NIL, 0);

		if (GetTempFile(format.outfile) != format.outfile)
		{
			File	 tempFile(GetTempFile(format.outfile));

			tempFile.Move(format.outfile);
		}
	}

	return True;
}

Int BoCA::FAACOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	Config	*config = Config::Get();

	unsigned long	 bytes = 0;
	Int		 samplesRead = size / (format.bits / 8);

	totalSamples += samplesRead / format.channels;

	if (format.bits != 16)
	{
		for (int i = 0; i < samplesRead; i++)
		{
			if (format.bits == 8)	((short *) (int32_t *) samplesBuffer)[i] = (data[i] - 128) * 256;
			if (format.bits == 24) samplesBuffer[i] = data[3 * i] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0);
			if (format.bits == 32)	((float *) (int32_t *) samplesBuffer)[i] = (1.0 / 65536) * ((int32_t *) (unsigned char *) data)[i];
		}

		bytes = ex_faacEncEncode(handle, samplesBuffer, samplesRead, outBuffer, outBuffer.Size());
	}
	else
	{
		bytes = ex_faacEncEncode(handle, (int32_t *) (unsigned char *) data, samplesRead, outBuffer, outBuffer.Size());
	}

	if (config->enable_mp4)
	{
		if (bytes > 0)
		{
			Int		 samplesLeft	= totalSamples - encodedSamples + delaySamples;
			MP4Duration	 dur		= samplesLeft > frameSize ? frameSize : samplesLeft;
			MP4Duration	 ofs		= encodedSamples > 0 ? 0 : delaySamples;

			ex_MP4WriteSample(mp4File, mp4Track, (u_int8_t *) (unsigned char *) outBuffer, bytes, dur, ofs, true);

			encodedSamples += dur;
		}
	}
	else
	{
		driver->WriteData(outBuffer, bytes);
	}

	return bytes;
}

ConfigLayer *BoCA::FAACOut::GetConfigurationLayer()
{
	return configLayer;
}

String BoCA::FAACOut::GetTempFile(const String &oFileName)
{
	String	 rVal	= oFileName;
	Int	 lastBs	= -1;

	for (Int i = 0; i < rVal.Length(); i++)
	{
		if (rVal[i] > 255)	rVal[i] = '#';
		if (rVal[i] == '\\')	lastBs = i;
	}

	if (rVal == oFileName) return rVal;

	String	 tempDir = S::System::System::GetTempDirectory();

	for (Int j = lastBs + 1; j < rVal.Length(); j++)
	{
		tempDir[tempDir.Length()] = rVal[j];
	}

	return tempDir.Append(".out.temp");
}
