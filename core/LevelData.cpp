// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it).
// All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "LevelData.h"
#include "GameScene.h"
#include "View.h"
#include "mathUtils.h"
#include <fstream>
#include <stdexcept>

using namespace agp;

LevelData::LevelData(const std::string& path) : _path(path)
{
	try
	{
		std::ifstream file(path);
		if (!file.is_open())
			throw std::runtime_error("cannot open file");
		_json = nlohmann::ordered_json::parse(file);
		validate(_json);
	}
	catch (const std::exception& error)
	{
		throw std::runtime_error("Cannot load level \"" + path + "\": " + error.what());
	}
}

RectF LevelData::rect(const nlohmann::ordered_json& json)
{
	RectF result(json.at("x"), json.at("y"), json.at("width"), json.at("height"), json.at("yUp"));
	if (!std::isfinite(result.pos.x) || !std::isfinite(result.pos.y) ||
		!std::isfinite(result.size.x) || !std::isfinite(result.size.y) || !result.isValid())
		throw std::runtime_error("invalid rectangle");
	return result;
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
		throw std::runtime_error("invalid rotated rectangle");
	return result;
}

std::vector<PointF> LevelData::multiline(const nlohmann::ordered_json& json)
{
	if (!json.is_array() || json.size() < 2)
		throw std::runtime_error("a multiline needs at least two points");
	std::vector<PointF> points;
	for (const auto& point : json)
	{
		PointF p(point.at("x"), point.at("y"));
		if (!std::isfinite(p.x) || !std::isfinite(p.y))
			throw std::runtime_error("invalid multiline point");
		points.push_back(p);
	}
	return points;
}

void LevelData::validate(const nlohmann::ordered_json& json)
{
	const auto& categories = json.at("categories");
	if (!categories.is_array() || categories.empty())
		throw std::runtime_error("categories must be a non-empty array");
	for (const auto& category : categories)
		if (!category.is_string())
			throw std::runtime_error("category names must be strings");

	const auto& objects = json.at("objects");
	if (!objects.is_array())
		throw std::runtime_error("objects must be an array");
	for (size_t i = 0; i < objects.size(); i++)
	{
		try
		{
			const auto& object = objects[i];
			const auto& category = object.at("category");
			if (!category.is_number_integer() || category.get<int>() < 0 || category.get<size_t>() >= categories.size())
				throw std::runtime_error("category index out of range");
			if (object.contains("name"))
				object.at("name").get<std::string>();
			int geometries = object.contains("rect") + object.contains("rotRect") + object.contains("multiline");
			if (geometries != 1)
				throw std::runtime_error("expected one geometry: rect, rotRect or multiline");
			if (object.contains("multiline"))
				multiline(object.at("multiline"));
			else
				rotRect(object);
		}
		catch (const std::exception& error)
		{
			throw std::runtime_error("object " + std::to_string(i) + ": " + error.what());
		}
	}

	const auto& scene = json.at("scene");
	rect(scene.at("rect"));
	const auto& unit = scene.at("pixelUnitSize");
	if (!unit.at("x").is_number_integer() || !unit.at("y").is_number_integer() ||
		unit.at("x").get<int>() <= 0 || unit.at("y").get<int>() <= 0)
		throw std::runtime_error("pixelUnitSize must contain positive integers");
	float dt = scene.at("dt");
	if (!std::isfinite(dt) || dt <= 0)
		throw std::runtime_error("dt must be positive");
	if (scene.contains("view"))
		rect(scene.at("view"));
	if (scene.contains("backgroundColor"))
	{
		const auto& color = scene.at("backgroundColor");
		if (!color.is_array() || color.size() != 4)
			throw std::runtime_error("backgroundColor must contain RGBA components");
		for (const auto& component : color)
			if (!component.is_number_integer() || component.get<int>() < 0 || component.get<int>() > 255)
				throw std::runtime_error("color components must be integers in [0,255]");
	}
}

std::string LevelData::category(const nlohmann::ordered_json& object) const
{
	return _json.at("categories").at(object.at("category").get<size_t>()).get<std::string>();
}

RectF LevelData::sceneRect() const
{
	return rect(_json.at("scene").at("rect"));
}

Point LevelData::pixelUnitSize() const
{
	const auto& unit = _json.at("scene").at("pixelUnitSize");
	return { unit.at("x"), unit.at("y") };
}

float LevelData::timeStep() const
{
	return _json.at("scene").at("dt");
}

void LevelData::configure(GameScene* scene) const
{
	scene->setJsonPath(_path);
	const auto& settings = _json.at("scene");
	if (settings.contains("view"))
		scene->view()->setRect(rect(settings.at("view")));
	if (settings.contains("backgroundColor"))
	{
		const auto& color = settings.at("backgroundColor");
		scene->setBackgroundColor(Color(color[0], color[1], color[2], color[3]));
	}
}

void LevelData::save(const std::string& path, const nlohmann::ordered_json& json)
{
	validate(json);
	std::ofstream file(path);
	if (!file.is_open() || !(file << json.dump(3) << '\n'))
		throw std::runtime_error("Cannot save level \"" + path + "\"");
	file.close();
	if (!file)
		throw std::runtime_error("Cannot finish saving level \"" + path + "\"");
}
