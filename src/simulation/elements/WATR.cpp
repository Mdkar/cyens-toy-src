#include "simulation/ElementCommon.h"
#include <iostream>

static int update(UPDATE_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_WATR()
{
	Identifier = "DEFAULT_PT_WATR";
	Name = "WATR";
	FullName = "Water";
	Colour = PIXPACK(0x2030D0);
	MenuVisible = 1;
	MenuSection = SC_LIQUID;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 30;

	DefaultProperties.temp = R_TEMP - 2.0f + 273.15f;
	HeatConduct = 29;
	Description = "Water. Conducts electricity, freezes, and extinguishes fires.";

	Properties = TYPE_LIQUID | PROP_CONDUCTS | PROP_LIFE_DEC | PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = 273.15f;
	LowTemperatureTransition = PT_ICEI;
	HighTemperature = 373.0f;
	HighTemperatureTransition = PT_WTRV;
	GasTemperaturetransition = ITH;
	GasTransition = NT;
	PlsmTemperaturetransition = -1;
	SolidLiquidlatent = 52.f;
	LiquidGaslatent = 350.f;
	GasPlsmlatent = 5000.f;

	Update = &update;
	Create = &create;
}

static bool hasIons(Particle part)
{
	return part.ionP.type != 0 || part.ionN.type != 0;
}
static bool isSoluble(int cat, int an)//TODO: add polyatomic ions
{
	if(cat == PT_LI || cat == PT_NA || cat == PT_K || cat == PT_RB || cat == PT_CS || cat == PT_FR)
		return true; //Group 1 always soluble
	if(an == PT_CL || an == PT_BR || an == PT_I)
	{
		if(cat == PT_AG || cat == PT_PB || cat == PT_CU || cat == PT_HG)
		{
			return false; //Cl, Br, I exceptions
		}
		return true;
	}
	return false;
}
static void copyIons(ion *copy, ion old){
	copy->type = old.type;
	copy->number = old.number;
	copy->charge = old.charge;
}

static int update(UPDATE_FUNC_ARGS)
{
	int r, rx, ry;
	for (rx = -1; rx < 2; rx++)
		for (ry = -1; ry < 2; ry++)
			if (BOUNDS_CHECK && (rx || ry))
			{
				r = pmap[y + ry][x + rx];
				if (!r)
					continue;
				if((TYP(r) == PT_WATR || TYP(r) == PT_DSTW) && RNG::Ref().chance(1, 10) && (parts[ID(r)].ions != NULL || parts[i].ions != NULL))
				{
					std::vector<ion>* temp = parts[ID(r)].ions;
					parts[ID(r)].ions = parts[i].ions;
					parts[i].ions = temp;
				}
				else if (TYP(r) == PT_SALT && RNG::Ref().chance(1, 50) && isSoluble(parts[ID(r)].ionP.type, parts[ID(r)].ionN.type))
				{
					//sim->part_change_type(i, x, y, PT_SLTW);
					// on average, convert 3 WATR to SLTW before SALT turns into SLTW
					if (RNG::Ref().chance(1, 2))//replace chance with solubility?
					{
						if(parts[i].ions == NULL){
							parts[i].ions = new std::vector<ion>();
						}
						std::vector<ion> temp = *(parts[i].ions);
						if(!std::count(temp.begin(), temp.end(), parts[ID(r)].ionP.type) && !std::count(temp.begin(), temp.end(), parts[ID(r)].ionN.type)){
							ion p = ion();
							ion n = ion();
							copyIons(&p,parts[ID(r)].ionP);
							copyIons(&n,parts[ID(r)].ionN);
							parts[i].ions->push_back(p);
							parts[i].ions->push_back(n);

							if (RNG::Ref().chance(99, 100)){
								sim->kill_part(ID(r));
							}	else {
								sim->part_change_type(ID(r), x + rx, y + ry, PT_DSTW);//increase volume tiny amount
								parts[ID(r)].ionP.type = 0;
								parts[ID(r)].ionN.type = 0;
							}
						}

					}
					/*if (hasIons(parts[i])) //TODO: precipitation reaction
					{
						if(!isSoluble(parts[i].ionP.type, parts[r].ionN.type))
						{
							//make sure to check double replacement
						}
						else if(!isSoluble(parts[r].ionP.type, parts[i].ionN.type))
						{

						}
					}*/
					//TODO: common ion effect
				}
				else if ((TYP(r) == PT_RBDM || TYP(r) == PT_LRBD || TYP(r) == PT_SDUM) && (sim->legacy_enable || parts[i].temp > (273.15f + 12.0f)) && !(rand() % 100))
				{
					sim->part_change_type(i, x, y, PT_FIRE);
					if (TYP(r) == PT_SDUM)
						sim->part_change_type(ID(r), x + rx, y + ry, PT_SMKE);
					parts[i].life = 4;
					parts[i].ctype = PT_WATR;
				}
				else if (TYP(r) == PT_FIRE && parts[ID(r)].ctype != PT_WATR)
				{
					sim->kill_part(ID(r));
					if (RNG::Ref().chance(1, 30))
					{
						sim->kill_part(i);
						return 1;
					}
				}
				// else if (TYP(r) == PT_SLTW && RNG::Ref().chance(1, 2000))
				// {
				// 	sim->part_change_type(i, x, y, PT_SLTW);
				// }
			}
	if(parts[i].ions == NULL){
		sim->part_change_type(i, x, y, PT_DSTW);
	}
	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	if(sim->parts[i].ions == NULL){
		sim->parts[i].ions = new std::vector<ion>();
		ion p = ion();
		ion n = ion();
		p.type = PT_H;
		p.number = 1;
		p.charge = 1;
		n.type = PT_HXDE;
		n.number = 1;
		n.charge = -1;
		sim->parts[i].ions->push_back(p);
		sim->parts[i].ions->push_back(n);
	}
}
