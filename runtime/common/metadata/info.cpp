 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/metadata/info.h>

BoCA::Info::Info() : mcdi(Buffer<UnsignedByte>())
{
	year		= -1;

	track		= -1;
	numTracks	= -1;

	disc		= -1;
	numDiscs	= -1;

	rating		= 0;
}

BoCA::Info::Info(const Info &oInfo) : mcdi(Buffer<UnsignedByte>())
{
	*this = oInfo;
}

BoCA::Info::~Info()
{
}

BoCA::Info &BoCA::Info::operator =(const Info &oInfo)
{
	if (&oInfo == this) return *this;

	artist		= oInfo.artist;
	title		= oInfo.title;
	album		= oInfo.album;
	genre		= oInfo.genre;
	year		= oInfo.year;
	comment		= oInfo.comment;

	track		= oInfo.track;
	numTracks	= oInfo.numTracks;

	disc		= oInfo.disc;
	numDiscs	= oInfo.numDiscs;

	rating		= oInfo.rating;

	label		= oInfo.label;
	isrc		= oInfo.isrc;

	track_gain	= oInfo.track_gain;
	track_peak	= oInfo.track_peak;
	album_gain	= oInfo.album_gain;
	album_peak	= oInfo.album_peak;

	mcdi		= oInfo.mcdi;
	offsets		= oInfo.offsets;

	other.RemoveAll();

	foreach (const String &string, oInfo.other)
	{
		other.Add(string);
	}

	return *this;
}
