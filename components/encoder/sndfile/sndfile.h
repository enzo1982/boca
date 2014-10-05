 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(EncoderSndFile)

namespace BoCA
{
	class EncoderSndFile : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			FILE			*file;
			SNDFILE			*sndf;

			Int			 SelectBestSubFormat(const Format &, Int);
		public:
			static const String	&GetComponentSpecs();

						 EncoderSndFile();
						~EncoderSndFile();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			String			 GetOutputFileExtension();

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderSndFile)

BoCA_END_COMPONENT(EncoderSndFile)