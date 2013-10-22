 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_WINAMPCONFIG
#define H_WINAMPCONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureWinamp : public ConfigLayer
	{
		private:
			ListBox		*list_output;
			Button		*button_output;
			Button		*button_output_about;
		slots:
			Void		 SelectOutputPlugin();
			Void		 ConfigureOutputPlugin();
			Void		 AboutOutputPlugin();
		public:
					 ConfigureWinamp();
					~ConfigureWinamp();

			Int		 SaveSettings();
	};
};

#endif
