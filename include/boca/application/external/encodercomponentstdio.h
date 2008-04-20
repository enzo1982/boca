 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_ENCODERCOMPONENTEXTERNALSTDIO
#define H_BOCA_ENCODERCOMPONENTEXTERNALSTDIO

#include "encodercomponent.h"

namespace BoCA
{
	namespace AS
	{
		class EncoderComponentExternalStdIO : public EncoderComponentExternal
		{
			private:
				IO::Driver	*driver_stdin;
				IO::OutStream	*out;

				HANDLE		 rPipe;
				HANDLE		 wPipe;

				HANDLE		 hProcess;

				String		 encFileName;
			public:
						 EncoderComponentExternalStdIO(ComponentSpecs *);
				virtual		~EncoderComponentExternalStdIO();

				virtual Bool	 Activate();
				virtual Bool	 Deactivate();

				virtual Int	 WriteData(Buffer<UnsignedByte> &, Int);
		};
	};
};

#endif
