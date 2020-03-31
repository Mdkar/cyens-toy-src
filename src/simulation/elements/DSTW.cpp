#include "simulation/ElementCommon.h"
#include <iostream>

static int update(UPDATE_FUNC_ARGS);

void Element::Element_DSTW()
{
	Identifier = "DEFAULT_PT_DSTW";
	Name = "DSTW";
	FullName = "Distilled Water";
	Colour = PIXPACK(0x1020C0);
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
	HotAir = 0.000f	* CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 30;

	DefaultProperties.temp = R_TEMP - 2.0f + 273.15f;
	HeatConduct = 23;
	Description = "Distilled water, does not conduct electricity.";

	Properties = TYPE_LIQUID|PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = 273.15f;
	LowTemperatureTransition = PT_ICEI;
	HighTemperature = 373.0f;
	HighTemperatureTransition = PT_WTRV;

	Update = &update;
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
	if(an == PT_HXDE)
	{
		if(cat == PT_CA || cat == PT_SR || cat == PT_BA || cat == PT_H){
			return true;
		}
		return false;
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
	for (rx=-1; rx<2; rx++)
		for (ry=-1; ry<2; ry++)
			if (BOUNDS_CHECK && (rx || ry))
			{
				r = pmap[y+ry][x+rx];
				switch (TYP(r))
				{
				case PT_WATR:
					if(RNG::Ref().chance(1, 10) && parts[ID(r)].ions != NULL && parts[i].ions == NULL)
					{
						parts[i].ions = parts[ID(r)].ions;
						parts[ID(r)].ions = NULL;
						sim->part_change_type(ID(r), x + rx, y + ry, PT_DSTW);
						sim->part_change_type(i, x, y, PT_WATR);
					}
					break;
				case PT_DSTW:
					if(RNG::Ref().chance(1, 10) && parts[ID(r)].ions == NULL && parts[i].ions != NULL)
					{
						parts[ID(r)].ions = parts[i].ions;
						parts[i].ions = NULL;
						sim->part_change_type(ID(r), x + rx, y + ry, PT_WATR);
						sim->part_change_type(i, x, y, PT_DSTW);
					}
					break;
				case PT_SALT:
					if (RNG::Ref().chance(1, 50) && isSoluble(parts[ID(r)].ionP.type, parts[ID(r)].ionN.type))
					{
						//sim->part_change_type(i, x, y, PT_SLTW);
						// on average, convert 3 WATR to SLTW before SALT turns into SLTW
						if (RNG::Ref().chance(1, 2))//replace chance with solubility?
						{
							if(parts[i].ions == NULL){
								parts[i].ions = new std::vector<ion>();
								std::vector<ion> temp = *(parts[i].ions);
								if(!std::count(temp.begin(), temp.end(), parts[ID(r)].ionP.type) && !std::count(temp.begin(), temp.end(), parts[ID(r)].ionN.type)){
									ion p = ion();
									ion n = ion();
									copyIons(&p,parts[ID(r)].ionP);
									copyIons(&n,parts[ID(r)].ionN);
									parts[i].ions->push_back(p);
									parts[i].ions->push_back(n);
									sim->part_change_type(i, x, y, PT_WATR);
									if (RNG::Ref().chance(99, 100)){
										sim->kill_part(ID(r));
									}	else {
										sim->part_change_type(ID(r), x + rx, y + ry, PT_DSTW);//increase volume tiny amount
										parts[ID(r)].ionP.type = 0;
										parts[ID(r)].ionN.type = 0;
									}
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
					break;
				// case PT_SLTW:
				// 	if (RNG::Ref().chance(1, 2000))
				// 	{
				// 		sim->part_change_type(i,x,y,PT_SLTW);
				// 		break;
				// 	}
				// case PT_WATR:
				// 	if (RNG::Ref().chance(1, 100))
				// 	{
				// 		sim->part_change_type(i,x,y,PT_WATR);
				// 	}
				// 	break;
				case PT_RBDM:
				case PT_LRBD:
					if ((sim->legacy_enable||parts[i].temp>12.0f) && RNG::Ref().chance(1, 100))
					{
						sim->part_change_type(i,x,y,PT_FIRE);
						parts[i].life = 4;
					}
					break;
				case PT_FIRE:
					sim->kill_part(ID(r));
					if (RNG::Ref().chance(1, 30))
					{
						sim->kill_part(i);
						return 1;
					}
					break;
				default:
					continue;
				}
			}
	if(parts[i].ions != NULL){
		sim->part_change_type(i, x, y, PT_WATR);
	}
	return 0;
}
