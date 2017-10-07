 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include <time.h>

#include "bladeenc.h"
#include "config.h"

const String &BoCA::EncoderBlade::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (bladedll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>BladeEnc MP3 Encoder %VERSION%</name>			\
		    <version>1.0</version>					\
		    <id>bladeenc-enc</id>					\
		    <type threadSafe=\"false\">encoder</type>			\
		    <format>							\
		      <name>MPEG 1 Audio Layer 3</name>				\
		      <extension>mp3</extension>				\
		      <tag id=\"id3v1-tag\" mode=\"append\">ID3v1</tag>		\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>	\
		    </format>							\
		  </component>							\
										\
		";

		BE_VERSION	 beVer;

		ex_beVersion(&beVer);

		componentSpecs.Replace("%VERSION%", String("v").Append(String::FromInt(beVer.byMajorVersion)).Append(".").Append(String::FromInt(beVer.byMinorVersion)));
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadBladeDLL();
}

Void smooth::DetachDLL()
{
	FreeBladeDLL();
}

BoCA::EncoderBlade::EncoderBlade() : beConfig()
{
	configLayer = NIL;

	handle	    = NIL;
}

BoCA::EncoderBlade::~EncoderBlade()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderBlade::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info = track.GetInfo();

	if (format.rate != 32000 && format.rate != 44100 && format.rate != 48000)
	{
		errorString = "Bad sampling rate! BladeEnc supports only 32, 44.1 or 48 kHz.";
		errorState  = True;

		return False;
	}

	if (format.channels > 2)
	{
		errorString = "This encoder does not support more than 2 channels!";
		errorState  = True;

		return False;
	}

	const Config	*config = GetConfiguration();

	beConfig.dwConfig			= BE_CONFIG_MP3;
	beConfig.format.mp3.dwSampleRate	= format.rate;

	if (format.channels == 2)
	{
		if (config->GetIntValue(ConfigureBlade::ConfigID, "DualChannel", 0)) beConfig.format.mp3.byMode = BE_MP3_MODE_DUALCHANNEL;
		else								     beConfig.format.mp3.byMode = BE_MP3_MODE_STEREO;
	}
	else if (format.channels == 1)
	{
		beConfig.format.mp3.byMode = BE_MP3_MODE_MONO;
	}

	beConfig.format.mp3.wBitrate	= config->GetIntValue(ConfigureBlade::ConfigID, "Bitrate", 0);
	beConfig.format.mp3.bCopyright	= config->GetIntValue(ConfigureBlade::ConfigID, "Copyright", 0);
	beConfig.format.mp3.bCRC	= config->GetIntValue(ConfigureBlade::ConfigID, "CRC", 0);
	beConfig.format.mp3.bOriginal	= config->GetIntValue(ConfigureBlade::ConfigID, "Original", 1);
	beConfig.format.mp3.bPrivate	= config->GetIntValue(ConfigureBlade::ConfigID, "Private", 0);

	unsigned long	 bufferSize	= 0;
	unsigned long	 samplesSize	= 0;

	ex_beInitStream(&beConfig, &samplesSize, &bufferSize, &handle);

	outBuffer.Resize(bufferSize);
	samplesBuffer.Resize(samplesSize);

	packageSize = samplesSize * (format.bits / 8);

	/* Write ID3v2 tag if requested.
	 */
	if (config->GetIntValue("Tags", "EnableID3v2", True) && (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->SetConfiguration(GetConfiguration());
			tagger->RenderBuffer(id3Buffer, track);

			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	return True;
}

Bool BoCA::EncoderBlade::Deactivate()
{
	const Config	*config = GetConfiguration();
	const Info	&info = track.GetInfo();

	unsigned long	 bytes = 0;

	ex_beDeinitStream(handle, outBuffer, &bytes);

	driver->WriteData(outBuffer, bytes);

	ex_beCloseStream(handle);

	/* Write ID3v1 tag if requested.
	 */
	if (config->GetIntValue("Tags", "EnableID3v1", False) && info.HasBasicInfo())
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v1-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->SetConfiguration(GetConfiguration());
			tagger->RenderBuffer(id3Buffer, track);

			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	/* Update ID3v2 tag with correct chapter marks.
	 */
	if (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True) && config->GetIntValue("Tags", "EnableID3v2", True))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->SetConfiguration(GetConfiguration());
			tagger->RenderBuffer(id3Buffer, track);

			driver->Seek(0);
			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	return True;
}

Int BoCA::EncoderBlade::WriteData(Buffer<UnsignedByte> &data)
{
	unsigned long	 bytes = 0;

	const Format	&format = track.GetFormat();

	if (format.bits != 16)
	{
		for (int i = 0; i < data.Size() / (format.bits / 8); i++)
		{
			if	(format.bits ==  8) samplesBuffer[i] =	     (				  data [i] - 128) * 256;
			else if (format.bits == 32) samplesBuffer[i] = (int) (((long *) (unsigned char *) data)[i]	  / 65536);

			else if (format.bits == 24) samplesBuffer[i] = (int) ((data[3 * i] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0)) / 256);
		}

		ex_beEncodeChunk(handle, data.Size() / (format.bits / 8), samplesBuffer, outBuffer, &bytes);
	}
	else
	{
		ex_beEncodeChunk(handle, data.Size() / (format.bits / 8), (short *) (unsigned char *) data, outBuffer, &bytes);
	}

	driver->WriteData(outBuffer, bytes);

	return bytes;
}

ConfigLayer *BoCA::EncoderBlade::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureBlade();

	return configLayer;
}
