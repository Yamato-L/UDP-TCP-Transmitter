#pragma once

#include "dvec3.h"

class CartesianCoordinateSystem
{
public:
    dvec3 AxisX = dvec3(1, 0, 0);
    dvec3 AxisY = dvec3(0, 1, 0);
    dvec3 AxisZ = dvec3(0, 0, 1);
    dvec3 Center = dvec3(0, 0, 0);
public:
    void Normalize()
    {
        AxisX = AxisY.cross(AxisZ);
        AxisX = AxisX.normalize();
        AxisY = AxisZ.cross(AxisX);
        AxisY = AxisY.normalize();

        // mAxisZ = mAxisX.Cross(mAxisY);
    }

    dvec3 LocalToWorld(const dvec3& local)
    {
        return std::move(Center + local.x * AxisX + local.y * AxisY + local.z * AxisZ);
    }

    dvec3 LocalToWorldDir(const dvec3& local)
    {
        return std::move(local.x * AxisX + local.y * AxisY + local.z * AxisZ);
    }

    dvec3 WorldToLocal(const dvec3& p)
    {
		dvec3 v = p - Center;

        return std::move(dvec3(
            v.dot(AxisX),
            v.dot(AxisY),
            v.dot(AxisZ)));
    }

    dvec3 WorldToLocalDir(dvec3& p)
    {
        return std::move(dvec3(
            p.dot(AxisX),
            p.dot(AxisY),
            p.dot(AxisZ)));
    }
};
