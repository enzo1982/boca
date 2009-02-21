 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_TAG_ID3V1
#define H_BOCA_TAG_ID3V1

#include <smooth.h>
#include "tag.h"

using namespace smooth;

namespace BoCA
{
	class TagID3v1 : public Tag
	{
		private:
			static const String	 genres[148];

			const String		&GetID3CategoryName(Int);
			Int			 GetID3CategoryID(const String &);
		public:
						 TagID3v1();
						~TagID3v1();

			Int			 Render(const Track &, Buffer<UnsignedByte> &);

			Int			 Parse(const Buffer<UnsignedByte> &, Track *);
			Int			 Parse(const String &, Track *);
	};
};

#endif