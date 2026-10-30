// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it).
// All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "LevelData.h"
#include "mathUtils.h"
#include <fstream>
#include <string>

using namespace agp;

LevelData::LevelData(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
		throw "cannot open file";
	_json = nlohmann::ordered_json::parse(file);
	validate(_json);
}

RectF LevelData::rect(const nlohmann::ordered_json& json)
{
	RectF result(json.at("x"), json.at("y"), json.at("width"), json.at("height"), json.at("yUp"));
	if (!std::isfinite(result.pos.x) || !std::isfinite(result.pos.y) ||
		!std::isfinite(result.size.x) || !std::isfinite(result.size.y) || !result.isValid())
		throw "invalid rectangle";
	return result;
}

RectF LevelData::axisAlignedRect(const nlohmann::ordered_json& object, bool yUp)
{
	RotatedRectF geometry = rotRect(object);
	if (geometry.angle != 0)
		throw "Expected an axis-aligned rectangle";
	if (geometry.yUp != yUp)
		throw "Rectangle yUp does not match the required coordinate system";
	return geometry.toRect();
}

RotatedRectF LevelData::rotRect(const nlohmann::ordered_json& object)
{
	if (object.contains("rect"))
		return rect(object.at("rect"));
	const auto& json = object.at("rotRect");
	RotatedRectF result(json.at("cx"), json.at("cy"), json.at("width"), json.at("height"),
		deg2rad(json.at("angle").get<float>()), json.at("yUp"));
	if (!std::isfinite(result.center.x) || !std::isfinite(result.center.y) ||
		!std::isfinite(result.size.x) || !std::isfinite(result.size.y) ||
		!std::isfinite(result.angle) || !result.isValid())
		throw "invalid rotated rectangle";
	return result;
}

std::vector<PointF> LevelData::multiline(const nlohmann::ordered_json& json)
{
	if (!json.is_array() || json.size() < 2)
		throw "a multiline needs at least two points";
	std::vector<PointF> points;
	for (const auto& point : json)
	{
		PointF p(point.at("x"), point.at("y"));
		if (!std::isfinite(p.x) || !std::isfinite(p.y))
			throw "invalid multiline point";
		points.push_back(p);
	}
	return points;
}

void LevelData::validate(const nlohmann::ordered_json& json)
{
	const auto& categories = json.at("categories");
	if (!categories.is_array() || categories.empty())
		throw "categories must be a non-empty array";
	for (const auto& category : categories)
		if (!category.is_string())
			throw "category names must be strings";

	const auto& objects = json.at("objects");
	if (!objects.is_array())
		throw "objects must be an array";
	for (size_t i = 0; i < objects.size(); i++)
	{
		const auto& object = objects[i];
		const auto& category = object.at("category");
		if (!category.is_number_integer() || category.get<int>() < 0 || category.get<size_t>() >= categories.size())
			throw "category index out of range";
		if (object.contains("name"))
			object.at("name").get<std::string>();
		int geometries = object.contains("rect") + object.contains("rotRect") + object.contains("multiline");
		if (geometries != 1)
			throw "expected one geometry: rect, rotRect or multiline";
		if (object.contains("multiline"))
			multiline(object.at("multiline"));
		else
			rotRect(object);
	}

}

std::string LevelData::category(const nlohmann::ordered_json& object) const
{
	return _json.at("categories").at(object.at("category").get<size_t>()).get<std::string>();
}

void LevelData::save(const std::string& path, const nlohmann::ordered_json& json)
{
	validate(json);
	std::ofstream file(path);
	if (!file.is_open() || !(file << json.dump(3) << '\n'))
		throw "Cannot save level file";
	file.close();
	if (!file)
		throw "Cannot finish saving level file";
}
