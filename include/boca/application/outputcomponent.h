 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_OUTPUTCOMPONENT
#define H_BOCA_OUTPUTCOMPONENT

#include "component.h"
#include "../common/track.h"

using namespace smooth::GUI;

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT OutputComponent : public Component, public IO::Filter
		{
			public:
					 OutputComponent(ComponentSpecs *);
					~OutputComponent();

				Bool	 SetAudioTrackInfo(const Track &);

				Bool	 Activate();
				Bool	 Deactivate();

				Int	 WriteData(Buffer<UnsignedByte> &buffer, Int size);

				Int	 GetPackageSize();

				Int	 CanWrite();

				Int	 SetPause(Bool);
				Bool	 IsPlaying();
		};
	};
};

#endif
