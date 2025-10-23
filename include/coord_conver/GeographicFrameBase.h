#pragma once

#include <memory>

class dvec3;

class GeographicFrameBase
{
public:
    virtual dvec3 LocalToWgs(const dvec3& localPosition) = 0;

    virtual dvec3 LocalToEcef(const dvec3& localPosition) = 0;

    virtual dvec3 WgsToLocal(const dvec3& wgsPosition) = 0;

    virtual dvec3 EcefToLocal(const dvec3& ecefPosition) = 0;

    static std::shared_ptr<GeographicFrameBase> CreateEnuFromWgs(
        double centerlat,
        double centerlon,
		double centeralt);
};
