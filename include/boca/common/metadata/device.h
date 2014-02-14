 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_DEVICE
#define H_BOCA_DEVICE

#include <smooth.h>
#include "../../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	enum DeviceType
	{
		DEVICE_NONE = 0,
		DEVICE_CDROM,

		NUM_DEVICETYPES
	};

	class BOCA_DLL_EXPORT Device
	{
		public:
			/* Device information:
			 */
			DeviceType	 type;
			String		 name;
			String		 path;

			Bool		 canOpenTray;

			/* Class constructor / destructor:
			 */
					 Device(int = 0);
					~Device();
	};
};

#endif
