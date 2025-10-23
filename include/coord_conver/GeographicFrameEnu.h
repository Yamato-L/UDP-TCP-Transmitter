#pragma once

#include "GeographicFrameBase.h"
#include "CartesianCoordinateSystem.h"
#include "GIS.h"

class GeographicFrameEnu : public GeographicFrameBase
{
public:
    CartesianCoordinateSystem LocalEnu;
public:
    GeographicFrameEnu(const CartesianCoordinateSystem& localEnu)
        :
        LocalEnu(localEnu)
    {
    }

    dvec3 LocalToWgs(const dvec3& localPosition)
    {
        dvec3 ecef = LocalEnu.LocalToWorld(localPosition);
        return GIS::EarthCenterXYZToWGS(ecef);
    }

    dvec3 LocalToEcef(const dvec3& localPosition)
    {
        return LocalEnu.LocalToWorld(localPosition);
    }

    dvec3 WgsToLocal(const dvec3& wgsPosition)
    {
        dvec3 ecef = GIS::WGSToEarthCenterXYZ(wgsPosition);
        return LocalEnu.WorldToLocal(ecef);
    }

    dvec3 EcefToLocal(const dvec3& ecefPosition)
    {
        return LocalEnu.WorldToLocal(ecefPosition);
    }

};