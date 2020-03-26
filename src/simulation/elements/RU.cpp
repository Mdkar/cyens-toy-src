#include "simulation/ElementCommon.h"

	static int update(UPDATE_FUNC_ARGS);
	static int graphics(GRAPHICS_FUNC_ARGS);

	void Element::Element_RU()
	{
		Identifier = "DEFAULT_PT_RU";
		Name = "Ru";
		FullName = "Ruthenium";
		Colour = PIXPACK(0xFFFFFF);
		MenuVisible = 0;
		Enabled = 1;

		Advection = 0.4f;
		AirDrag = 0.04f * CFDS;
		AirLoss = 0.94f;
		Loss = 0.95f;
		Collision = -0.1f;
		Gravity = 0.3f;
		Diffusion = 0.00f;
		HotAir = 0.000f	* CFDS;
		Falldown = 1;

		// element properties here

		Update = &update;
		Graphics = &graphics;
	}

	static int update(UPDATE_FUNC_ARGS)
	{
		// update code here

		return 0;
	}

	static int graphics(GRAPHICS_FUNC_ARGS)
	{
		// graphics code here
		// return 1 if nothing dymanic happens here

		return 0;
	}

	