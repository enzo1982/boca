 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/tagging/tagape.h>
#include <boca/common/config.h>

using namespace smooth::IO;

BoCA::TagAPE::TagAPE()
{
}

BoCA::TagAPE::~TagAPE()
{
}

Int BoCA::TagAPE::Render(const Track &track, Buffer<UnsignedByte> &buffer)
{
	Config		*currentConfig = Config::Get();
	char		*prevOutFormat = String::SetOutputFormat(currentConfig->GetStringValue("Tags", "APEv2Encoding", "UTF-8"));

	const Info	&info = track.GetInfo();

	buffer.Resize(32);

	Int		 numItems = 0;

	if (info.artist != NIL) { RenderAPEItem("Artist", info.artist, buffer);		     numItems++; }
	if (info.title  != NIL) { RenderAPEItem("Title", info.title, buffer);		     numItems++; }
	if (info.album  != NIL) { RenderAPEItem("Album", info.album, buffer);		     numItems++; }
	if (info.year    >   0) { RenderAPEItem("Year", String::FromInt(info.year), buffer); numItems++; }
	if (info.genre  != NIL) { RenderAPEItem("Genre", info.genre, buffer);		     numItems++; }
	if (info.label  != NIL) { RenderAPEItem("Publisher", info.label, buffer);	     numItems++; }
	if (info.isrc   != NIL) { RenderAPEItem("ISRC", info.isrc, buffer);		     numItems++; }

	if (info.track > 0)
	{
		String	 trackString = String(info.track < 10 ? "0" : "").Append(String::FromInt(info.track));

		if (info.numTracks > 0) trackString.Append("/").Append(info.numTracks < 10 ? "0" : "").Append(String::FromInt(info.numTracks));

		{ RenderAPEItem("Track", trackString, buffer); numItems++; }
	}

	if (info.disc > 0 && (info.numDiscs > 1 || info.disc > 1))
	{
		String	 discString = String(info.disc < 10 ? "0" : "").Append(String::FromInt(info.disc));

		if (info.numDiscs > 0) discString.Append("/").Append(info.numDiscs < 10 ? "0" : "").Append(String::FromInt(info.numDiscs));

		{ RenderAPEItem("Disc", discString, buffer); numItems++; }
	}

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	{ RenderAPEItem("Comment", info.comment, buffer);						  numItems++; }
	else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL && numItems > 0)		{ RenderAPEItem("Comment", currentConfig->GetStringValue("Tags", "DefaultComment", NIL), buffer); numItems++; }

	if (currentConfig->GetIntValue("Tags", "PreserveReplayGain", True))
	{
		if (info.track_gain != NIL && info.track_peak != NIL)
		{
			{ RenderAPEItem("replaygain_track_gain", info.track_gain, buffer); numItems++; }
			{ RenderAPEItem("replaygain_track_peak", info.track_peak, buffer); numItems++; }
		}

		if (info.album_gain != NIL && info.album_peak != NIL)
		{
			{ RenderAPEItem("replaygain_album_gain", info.album_gain, buffer); numItems++; }
			{ RenderAPEItem("replaygain_album_peak", info.album_peak, buffer); numItems++; }
		}
	}

	if (currentConfig->GetIntValue("Tags", "WriteCoverArt", True) && currentConfig->GetIntValue("Tags", "WriteCoverArtAPEv2", True))
	{
		foreach (const Picture &picInfo, track.pictures)
		{
			String			 itemName = "Cover Art";
			Buffer<UnsignedByte>	 picBuffer(picInfo.data.Size() + 19);

			if	(picInfo.type == 3) itemName.Append(" (front)");
			else if	(picInfo.type == 4) itemName.Append(" (back)");
			else			    itemName.Append(" (other)");

			strncpy((char *) (unsigned char *) picBuffer, "c:\\music\\cover.jpg", 18);
			memcpy(picBuffer + 19, picInfo.data, picInfo.data.Size());

			RenderAPEBinaryItem(itemName, picBuffer, buffer);

			numItems++;
		}
	}

	if (numItems > 0)
	{
		Int	 tagSize = buffer.Size();

		RenderAPEHeader(tagSize, numItems, buffer);
		RenderAPEFooter(tagSize, numItems, buffer);
	}
	else
	{
		buffer.Resize(0);
	}

	String::SetOutputFormat(prevOutFormat);

	return buffer.Size();
}

Int BoCA::TagAPE::RenderAPEHeader(Int tagSize, Int numItems, Buffer<UnsignedByte> &buffer)
{
	OutStream	 out(STREAM_BUFFER, buffer, 32);

	out.OutputString("APETAGEX");
	out.OutputNumber(2000, 4);
	out.OutputNumber(tagSize, 4);
	out.OutputNumber(numItems, 4);
	out.OutputNumber(0xA0000000, 4);
	out.OutputNumber(0, 4);
	out.OutputNumber(0, 4);

	return Success();
}

Int BoCA::TagAPE::RenderAPEFooter(Int tagSize, Int numItems, Buffer<UnsignedByte> &buffer)
{
	buffer.Resize(buffer.Size() + 32);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - 32, 32);

	out.OutputString("APETAGEX");
	out.OutputNumber(2000, 4);
	out.OutputNumber(tagSize, 4);
	out.OutputNumber(numItems, 4);
	out.OutputNumber(0x80000000, 4);
	out.OutputNumber(0, 4);
	out.OutputNumber(0, 4);

	return Success();
}

Int BoCA::TagAPE::RenderAPEItem(const String &id, const String &value, Buffer<UnsignedByte> &buffer)
{
	Int		 size = id.Length() + strlen(value) + 9;

	buffer.Resize(buffer.Size() + size);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - size, size);

	out.OutputNumber(strlen(value), 4);
	out.OutputNumber(0, 4);
	out.OutputString(id);
	out.OutputNumber(0, 1);
	out.OutputString(value);

	return Success();
}

Int BoCA::TagAPE::RenderAPEBinaryItem(const String &id, const Buffer<UnsignedByte> &value, Buffer<UnsignedByte> &buffer)
{
	Int		 size = id.Length() + value.Size() + 9;

	buffer.Resize(buffer.Size() + size);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - size, size);

	out.OutputNumber(value.Size(), 4);
	out.OutputNumber(0x01 << 1, 4); // set binary flag
	out.OutputString(id);
	out.OutputNumber(0, 1);
	out.OutputData(value, value.Size());

	return Success();
}

Int BoCA::TagAPE::Parse(const Buffer<UnsignedByte> &buffer, Track *track)
{
	Int	 numItems = 0;
	Int	 offset = 32;

	if (!ParseAPEHeader(buffer, NIL, &numItems))
	{
		offset = 0;

		if (!ParseAPEFooter(buffer, NIL, &numItems)) return Error();
	}

	char	*prevInFormat = String::SetInputFormat("UTF-8");

	Info	&info = track->GetInfo();

	for (Int i = 0; i < numItems; i++)
	{
		String			 id;
		String			 value;
		Buffer<UnsignedByte>	 item;

		ParseAPEItem(buffer, offset, &id, &value);

		id = id.ToUpper();

		if (id == "!BINARY") ParseAPEBinaryItem(buffer, offset, &id, item);

		if	(id == "ARTIST")    info.artist  = value;
		else if (id == "TITLE")	    info.title   = value;
		else if (id == "ALBUM")	    info.album   = value;
		else if (id == "YEAR")	    info.year	 = value.ToInt();
		else if (id == "GENRE")	    info.genre   = value;
		else if (id == "COMMENT")   info.comment = value;
		else if (id == "PUBLISHER") info.label   = value;
		else if (id == "ISRC")	    info.isrc	 = value;
		else if (id == "TRACK")
		{
			info.track = value.ToInt();

			if (value.Find("/") >= 0) info.numTracks = value.Tail(value.Length() - value.Find("/") - 1).ToInt();
		}
		else if (id == "DISC")
		{
			info.disc = value.ToInt();

			if (value.Find("/") >= 0) info.numDiscs = value.Tail(value.Length() - value.Find("/") - 1).ToInt();
		}
		else if (id.StartsWith("REPLAYGAIN"))
		{
			if	(id == "REPLAYGAIN_TRACK_GAIN") info.track_gain = value;
			else if (id == "REPLAYGAIN_TRACK_PEAK") info.track_peak = value;
			else if (id == "REPLAYGAIN_ALBUM_GAIN") info.album_gain = value;
			else if (id == "REPLAYGAIN_ALBUM_PEAK") info.album_peak = value;
		}
		else if (id.StartsWith("COVER ART"))
		{
			Picture	 picture;

			/* Read and ignore file name. Then copy
			 * picture data to Picture object.
			 */
			for (Int i = 0; i < item.Size(); i++)
			{
				if (item[i] == 0)
				{
					picture.data.Resize(item.Size() - i - 1);

					memcpy(picture.data, item + i + 1, picture.data.Size());

					break;
				}
			}

			if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
			else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
				 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
				 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
				 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

			if	(id.EndsWith("(FRONT)")) picture.type = 3;
			else if (id.EndsWith("(BACK)"))	 picture.type = 4;
			else				 picture.type = 0;

			track->pictures.Add(picture);
		}
	}

	String::SetInputFormat(prevInFormat);

	return Success();
}

Int BoCA::TagAPE::Parse(const String &fileName, Track *track)
{
	InStream		 in(STREAM_FILE, fileName, IS_READONLY);
	Buffer<UnsignedByte>	 buffer(32);

	in.InputData(buffer, 32);

	Int	 tagSize = 0;

	if (ParseAPEHeader(buffer, &tagSize, NIL))
	{
		buffer.Resize(tagSize);

		in.InputData(buffer, tagSize);

		return Parse(buffer, track);
	}

	in.Seek(in.Size() - 32);
	in.InputData(buffer, 32);

	if (ParseAPEFooter(buffer, &tagSize, NIL))
	{
		buffer.Resize(tagSize);

		in.Seek(in.Size() - tagSize);
		in.InputData(buffer, tagSize);

		return Parse(buffer, track);
	}

	return Error();
}

Bool BoCA::TagAPE::ParseAPEHeader(const Buffer<UnsignedByte> &buffer, Int *tagSize, Int *numItems)
{
	InStream	 in(STREAM_BUFFER, buffer, 32);

	if (in.InputString(8) != "APETAGEX")	return False;
	if (in.InputNumber(4) >= 3000)		return False;

	if (tagSize  != NIL) *tagSize  = in.InputNumber(4);
	else		     in.RelSeek(4);

	if (numItems != NIL) *numItems = in.InputNumber(4);
	else		     in.RelSeek(4);

	return True;
}

Bool BoCA::TagAPE::ParseAPEFooter(const Buffer<UnsignedByte> &buffer, Int *tagSize, Int *numItems)
{
	InStream	 in(STREAM_BUFFER, buffer + buffer.Size() - 32, 32);

	if (in.InputString(8) != "APETAGEX")	return False;
	if (in.InputNumber(4) >= 3000)		return False;

	if (tagSize  != NIL) *tagSize  = in.InputNumber(4);
	else		     in.RelSeek(4);

	if (numItems != NIL) *numItems = in.InputNumber(4);
	else		     in.RelSeek(4);

	return True;
}

Bool BoCA::TagAPE::ParseAPEItem(const Buffer<UnsignedByte> &buffer, Int &offset, String *id, String *value)
{
	InStream	 in(STREAM_BUFFER, buffer + offset, buffer.Size() - offset - 32);

	Int	 valueBytes = in.InputNumber(4);
	Int	 flags = in.InputNumber(4);

	/* Check if this is a binary tag item.
	 */
	if (((flags >> 1) & 3) == 1)
	{
		*id = "!Binary";

		return True;
	}

	*id = NIL;

	Byte	 lastChar = 0;

	do
	{
		lastChar = in.InputNumber(1);

		(*id)[id->Length()] = lastChar;
	}
	while (lastChar != 0);

	*value = in.InputString(valueBytes);

	offset += in.GetPos();

	return True;
}

Bool BoCA::TagAPE::ParseAPEBinaryItem(const Buffer<UnsignedByte> &buffer, Int &offset, String *id, Buffer<UnsignedByte> &value)
{
	InStream	 in(STREAM_BUFFER, buffer + offset, buffer.Size() - offset - 32);

	Int	 valueBytes = in.InputNumber(4);

	in.InputNumber(4);

	*id = NIL;

	Byte	 lastChar = 0;

	do
	{
		lastChar = in.InputNumber(1);

		(*id)[id->Length()] = lastChar;
	}
	while (lastChar != 0);

	value.Resize(valueBytes);

	in.InputData(value, valueBytes);

	offset += in.GetPos();

	return True;
}
