#ifndef __SUNRISE_H__
#define __SUNRISE_H__



#include <QDateTime>
#include <math.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

class Sunrise
{
public:

    Sunrise(double latitude_, double longitude_, double elevation_ = 0);
    int* DoubleToQTime(double);

    double timezone (double longitude)
    {
        double t=(floor((7.5+longitude)/15)-1);
        return longitude > 0 ? (floor((7.5+longitude)/15)-1) : (ceil((-7.5+longitude)/15)-1) ;
    }


    QTime sunrise(const QDate &d);
    QTime noon(const QDate &d);
    QTime sunset(const QDate &d);
    QTime DayLength(const QDate &d);

    static double angle(int degrees, int minutes, int seconds)
    {
    double a;
    a = seconds;
    a /= 60.0;
    a += minutes;
    a /= 60.0;
    a += degrees;
    return a;
    }

private:
    double dRadToDeg(double dAngleRad)
    {
        return (180.0 * dAngleRad / M_PI);
    }
    double dDegToRad(double dAngleDeg)
    {
        return (M_PI * dAngleDeg / 180.0);
    }
    double normalizedAngle(double a)
    {
        while (!(a < 360.0))
            a -= 360.0;
        while (!(a > -360.0))
            a += 360.0;
        return a;
    }

    double julianCycle(int iJulianDay)
    {
        double n = double(iJulianDay) - 2451545.0009 - longitude / 360.0;
        return floor(n + 0.5);
    }

    double solarNoonApprox(int iJulianDay)
    {
        return 2451545.0009  + longitude / 360.0 + julianCycle(iJulianDay);
    }

    double solarMean(double noon)
    {
        double m = 357.5291 + 0.98560028 * (noon - 2451545.0);
        return normalizedAngle(m);
    }

    double center(double mean)
    {
        double m1 = normalizedAngle(mean * 1.0);
        double m2 = normalizedAngle(mean * 2.0);
        double m3 = normalizedAngle(mean * 3.0);
        m1 = sin(dDegToRad(m1));
        m2 = sin(dDegToRad(m2));
        m3 = sin(dDegToRad(m3));
        return 1.9148*m1 + 0.0200*m2 + 0.0003*m3;
    }

    double gamma(double m)
    {
        double c = center(m);
        return normalizedAngle(m + 102.9372 + c + 180.0);
    }

    double solarTransit(double noon, double longitude)
    {
        double m = solarMean(noon);
        double g = gamma(m);
        m = sin(dDegToRad(m));
        g = normalizedAngle(g * 2.0);
        g = sin(dDegToRad(g));
        /*cout <<"solar transit is: "<<noon + 0.0053*m - 0.0069*g<<endl;
        double st= (noon + 0.0053*m - 0.0069*g)+(timezone(longitude)/24);
        cout <<"transit= "<<st-(noon + 0.0053*m - 0.0069*g)<<endl;*/
        return noon + 0.0053*m - 0.0069*g;
    }

    bool omega(double &o, double noon)
    {
        double m = solarMean(noon);
        double g = gamma(m);
        double l = dDegToRad(latitude);
        double sd = sin(dDegToRad(g)) * sin(dDegToRad(23.45));
        double cd = cos(asin(sd));
        double el = 0;
        if (elevation < 0.0) {
            el= 2.776*elevation/M_PI;
        }
         o = sin(dDegToRad(-0.83 + el)) - sin(l)*sd;
        o /= cos(l) * cd;
        if (o > 1.0 || o < -1.0)
          return false;
        o = dRadToDeg(acos(o));
        return true;
    }

    double sunset(int iJulianDay)
     {
        double noon = solarNoonApprox(iJulianDay);
        double m = solarMean(noon);
        double g = gamma(m);
        m = sin(dDegToRad(m));
        g = normalizedAngle(g * 2.0);
        g = sin(dDegToRad(g));
        double o;
        if (!omega(o, noon))
          return -1.0;
        o += longitude;
        o /= 360.0;
        return 2451545.0009 + o + julianCycle(iJulianDay) + 0.0053*m - 0.0069*g;

    }

    double noon(int iJulianDay)
    {
        double noon = solarNoonApprox(iJulianDay);
        return solarTransit(noon,longitude);
    }

    double sunrise(int iJulianDay)
    {
        double transit = noon(iJulianDay);
        double ss = sunset(iJulianDay);
        return ss < 0.0 ? -1.0 : transit - (ss - transit);
    }

    double DayLength(int iJulianDay)
    {
        double ss = sunset(iJulianDay);
        double sr = sunrise(iJulianDay);
        cout <<"day length is "<<ss - sr<<endl;
        return ss < 0.0 ? -1.0 : 0.5+ (ss - sr);
    }

    QDateTime date(double julian);
    int julian(const QDate &d);

    const double latitude;
    const double longitude;
    const double elevation;
};

#endif
