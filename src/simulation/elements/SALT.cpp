#include "simulation/ElementCommon.h"
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_SALT()
{
	Identifier = "DEFAULT_PT_SALT";
	Name = "SALT";
	FullName = "Salt";
	Colour = PIXPACK(0xFFFFFF);
	MenuVisible = 1;
	MenuSection = SC_POWDERS;
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

	Flammable = 0;
	Explosive = 0;
	Meltable = 5;
	Hardness = 1;

	Weight = 75;

	HeatConduct = 110;
	Description = "Salt, metal anion + cation (default NaCl).";

	Properties = TYPE_PART;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1173.0f;
	HighTemperatureTransition = PT_LAVA;

	Create = &create;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].ionP.type = PT_NA;
	sim->parts[i].ionP.number = 1;
	sim->parts[i].ionP.charge = 1;
	sim->parts[i].ionN.type = PT_CL;
	sim->parts[i].ionN.number = 1;
	sim->parts[i].ionN.charge = -1;
}
