#pragma once

#include "dvec3.h"
#include "ECEF.h"
#include "CartesianCoordinateSystem.h"
#define M_PI        3.14159265358979323846


class GIS
{
public:
	static dvec3 WGSToEarthCenterXYZ(
		dvec3 wgs)
	{
		return WGSToEarthCenterXYZ(wgs.x, wgs.y, wgs.z);
	}

	// Earth centered coordinate system
	// https://en.wikipedia.org/wiki/ECEF
	// center: mass center of earth
	// x: 0 degrees longitude (prime-meridian) at equator
	// y: 90 degrees east longitude at equator
	// z: points to north pole
	static dvec3 WGSToEarthCenterXYZ(
		double latitude,
		double longitude)
	{
		latitude = DegreeToRadian(latitude);
		longitude = DegreeToRadian(longitude);

		dvec3 ecef = ECEF::geo_to_ecef(
			dvec3(
				latitude,
				longitude,
				0));

		return ecef;
	}

	static dvec3 WGSToEarthCenterXYZ(
		double latitude,
		double longitude,
		double altitude)
	{
		latitude = DegreeToRadian(latitude);
		longitude = DegreeToRadian(longitude);

		dvec3 ecef = ECEF::geo_to_ecef(
			dvec3(
				latitude,
				longitude,
				altitude));

		return ecef;
	}

	static dvec3 EarthCenterXYZToWGS(dvec3 pos)
	{
		dvec3 geo = ECEF::ecef_to_geo(pos);

		return dvec3(
			RadianToDegree(geo.x), // lat
			RadianToDegree(geo.y),
			geo.z);
	}

	// Local east, north, up (ENU) coordinates
	// https://en.wikipedia.org/wiki/Axes_conventions
	// x: east
	// y: north
	// z: up
	static void BuildEnuCoordinateSystem(
		dvec3 v0,
		dvec3 vnorth,
		dvec3& axisX,
		dvec3& axisY,
		dvec3& axisZ)
	{
		axisZ = v0.normalize();
		axisX = vnorth.cross(v0).normalize();
		axisY = axisZ.cross(axisX).normalize();
	}

	static CartesianCoordinateSystem BuildEnuCoordinateSystem(
		dvec3 v0,
		dvec3 vnorth)
	{
		dvec3 axisX, axisY, axisZ;
		BuildEnuCoordinateSystem(v0, vnorth, axisX, axisY, axisZ);

		CartesianCoordinateSystem coordSys;
		coordSys.Center = v0;
		coordSys.AxisX = axisX;
		coordSys.AxisY = axisY;
		coordSys.AxisZ = axisZ;

		return coordSys;
	}

	static CartesianCoordinateSystem BuildEnuCoordinateSystem(
		dvec3 v0)
	{
		if (v0.length() <= ECEF::Epsilon)
			v0 = dvec3(1, 0, 0);
		dvec3 wgs = EarthCenterXYZToWGS(v0);
		dvec3 vnorth = WGSToEarthCenterXYZ(wgs.x + 1, wgs.y, wgs.z);

		return BuildEnuCoordinateSystem(v0, vnorth);
	}
private:
	static double DegreeToRadian(double deg)
	{
		return deg * M_PI / 180;
	}

	static double RadianToDegree(double rad)
	{
		return rad * 180 / M_PI;
	}
};