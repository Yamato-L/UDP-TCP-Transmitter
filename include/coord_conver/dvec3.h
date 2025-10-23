#pragma once

extern const double EPSILON;
extern const double MaxValue;

class dvec3
{
public:
    dvec3();
    dvec3(const double& xx, const double& yy, const double& zz);
    dvec3(const dvec3& rhs);
    bool isValidDirection();
    dvec3 normalize();
    double length();
    double dot(const dvec3& rhs);
    dvec3 cross(const dvec3& rhs);
    friend dvec3 operator +(const dvec3& lhs, const dvec3& rhs);
    friend dvec3 operator -(const dvec3& lhs, const dvec3& rhs);
    friend dvec3 operator *(const double& lhs, const dvec3& rhs);
    friend dvec3 operator *(const dvec3& self, const double& s);
    friend dvec3 operator /(const dvec3& lhs, const double& rhs);
    friend bool operator == (const dvec3& lhs, const dvec3& rhs);
    friend bool operator !=(const dvec3& lhs, const dvec3& rhs);
    static dvec3 normalize(const dvec3& v);
    static double length(const dvec3& v);
    static double distance(const dvec3& v1, const dvec3& v2);
    static double dot(const dvec3& lhs, const dvec3& rhs);
    static dvec3 cross(const dvec3& lhs, const dvec3& rhs);
    static double angleBetween(const dvec3& lhs, const dvec3& rhs);
public:
    double x;
    double y;
    double z;
    static dvec3 zero;
    static dvec3 axisX;
    static dvec3 axisY;
    static dvec3 axisZ;
};
