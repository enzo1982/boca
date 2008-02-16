 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(WinampOut)

namespace BoCA
{
	class WinampOut : public CS::OutputComponent
	{
		private:
			Out_Module		*plugin;
		public:
			static const String	&GetComponentSpecs();

						 WinampOut();
						~WinampOut();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();

			Int			 CanWrite();

			Int			 SetPause(Bool);
			Bool			 IsPlaying();
	};
};

BoCA_DEFINE_OUTPUT_COMPONENT(WinampOut)

BoCA_END_COMPONENT(WinampOut)
