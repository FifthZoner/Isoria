#pragma once

// a file containing map loading misc objects

#include <string>
#include <unordered_map>

#include "quickWrite.h"

struct datapackCheckStruct {
	str name = "";
	bool isPresent = false;
	ushort ingameNumber = NULL;
};

struct mlObjectID {
	ushort datapackNumber = NULL;
	str name = "";

	bool operator==(const mlObjectID& other) const
	{
		return (datapackNumber == other.datapackNumber
			&& name == other.name);
	}
};

// absolutely no idea what happens here (a hash function?) but it works, note: learn ANYTHING about hashing
namespace std {

	template <>
	struct hash<mlObjectID>
	{
		std::size_t operator()(const mlObjectID& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return ((hash<ushort>()(k.datapackNumber)
				^ (hash<string>()(k.name) << 1)) >> 1);
		}
	};

}

struct mlObjectData {
	ushort datapackNumber = NULL;
	uint objectID = NULL;
};

struct mlConversionTableContainer {
	std::unordered_map<mlObjectID, mlObjectData> background;
	std::unordered_map<mlObjectID, mlObjectData> wall;
	std::unordered_map<mlObjectID, mlObjectData> floor;
};

struct mlSecondaryConversionTableContainer {
	std::vector<mlObjectData> background;
	std::vector<mlObjectData> floor;
	std::vector<mlObjectData> wall;
};