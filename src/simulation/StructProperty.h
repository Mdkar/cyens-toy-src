#ifndef STRUCTPROPERTY_H_
#define STRUCTPROPERTY_H_

#include "common/String.h"
#include <cstdint>

struct ion {
	int type;
	int number;
	int charge;
};

struct StructProperty
{
	enum PropertyType
	{
		TransitionType,
		ParticleType,
		IonStruct,
		Colour,
		Integer,
		UInteger,
		Float,
		BString,
		String,
		Char,
		UChar,
		Removed
	};
	ByteString Name;
	PropertyType Type;
	intptr_t Offset;

	StructProperty(ByteString name, PropertyType type, intptr_t offset):
	Name(name),
	Type(type),
	Offset(offset)
	{

	}

	StructProperty():
	Name(""),
	Type(Char),
	Offset(0)
	{

	}
};

union PropertyValue {
	int Integer;
	unsigned int UInteger;
	float Float;
	ion IonStruct;
};

#endif
