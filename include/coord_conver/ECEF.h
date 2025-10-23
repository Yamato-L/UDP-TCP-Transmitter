#pragma once

#include "dvec3.h"
#include <algorithm>
#include <cmath>


class ECEF
{
public:
    static double Epsilon;
private:
    static double a; //WGS-84 semi-major axis
    static double e2; //WGS-84 first eccentricity squared
    static double a1; //a1 = a*e2
    static double a2; //a2 = a1*a1
    static double a3; //a3 = a1*e2/2
    static double a4; //a4 = 2.5*a2
    static double a5; //a5 = a1+a3
    static double a6; //a6 = 1-e2
public:
    static dvec3 ecef_to_geo(const dvec3& ecef)
    {
        double zp, w2, w, r2, r, s2, c2, s, c, ss;
        double g, rg, rf, u, v, m, f, p, x, y, z;

        //dvec3 geo = new dvec3(); //Results go here (Lat, Lon, Altitude)
        dvec3 geo; //Results go here (Lat, Lon, Altitude)
        x = ecef.x;
        y = ecef.y;
        z = ecef.z;
        zp = std::fabs(z);
        w2 = x * x + y * y;
        w = std::sqrt(w2);
        r2 = w2 + z * z;
        if (r2 < Epsilon)
            return dvec3();
        r = std::sqrt(r2);
        geo.y = std::atan2(y, x); //Lon (final)
        s2 = z * z / r2;
        c2 = w2 / r2;
        u = a2 / r;
        v = a3 - a4 / r;
        if (c2 > 0.3)
        {
            s = (zp / r) * (1.0 + c2 * (a1 + u + s2 * v) / r);
            geo.x = std::asin(s); //Lat
            ss = s * s;
            c = std::sqrt(1.0 - ss);
        }
        else
        {
            c = (w / r) * (1.0 - s2 * (a5 - u - c2 * v) / r);
            geo.x = std::acos(c); //Lat
            ss = 1.0 - c * c;
            s = std::sqrt(ss);
        }
        g = 1.0 - e2 * ss;
        rg = a / std::sqrt(g);
        rf = a6 * rg;
        u = w - rg * c;
        v = zp - rf * s;
        f = c * u + s * v;
        m = c * v - s * u;
        p = m / (rf / g + f);
        geo.x = geo.x + p; //Lat
        geo.z = f + m * p / 2.0; //Altitude
        if (z < 0.0)
        {
            geo.x *= -1.0; //Lat
        }
        return std::move(geo); //Return Lat, Lon, Altitude in that order
    }

    //Convert Lat, Lon, Altitude to Earth-Centered-Earth-Fixed (ECEF)
    //Input is a three element array containing lat, lon (rads) and alt (m)
    //Returned array contains x, y, z in meters
    static dvec3 geo_to_ecef(const dvec3& geo)
    {
        double n, lat, lon, alt;

        //dvec3 ecef = new dvec3(); //Results go here (x, y, z)
        dvec3 ecef; //Results go here (x, y, z)
        lat = geo.x;
        lon = geo.y;
        alt = geo.z;
        n = a / std::sqrt(1 - e2 * std::sin(lat) * std::sin(lat));
        ecef.x = (n + alt) * std::cos(lat) * std::cos(lon); //ECEF x
        ecef.y = (n + alt) * std::cos(lat) * std::sin(lon); //ECEF y
        ecef.z = (n * (1 - e2) + alt) * std::sin(lat); //ECEF z
        return std::move(ecef); //Return x, y, z in ECEF
    }
};
