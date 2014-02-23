 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

#include "info/cdtext.h"
#include "info/cdplayerini.h"

BoCA_BEGIN_COMPONENT(DecoderCDRip)

namespace BoCA
{
	class DecoderCDRip : public CS::DecoderComponent
	{
		private:
			static CDText		 cdText;
			static Int		 cdTextDiscID;

			static CDPlayerIni	 cdPlayer;
			static Int		 cdPlayerDiscID;

			ConfigLayer		*configLayer;

			Int			 dataBufferSize;

			Int			 readOffset;

			Int			 skipSamples;
			Int			 prependSamples;

			Int			 ComputeDiscID();

			Bool			 GetTrackSectors(Int &, Int &);
		public:
			static const String	&GetComponentSpecs();

						 DecoderCDRip();
						~DecoderCDRip();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Bool			 Seek(Int64);

			Int			 ReadData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderCDRip)

BoCA_END_COMPONENT(DecoderCDRip)
