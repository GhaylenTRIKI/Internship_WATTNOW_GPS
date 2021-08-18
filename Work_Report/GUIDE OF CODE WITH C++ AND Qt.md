# GUIDE OF CODE WITH C++ AND Qt
## Description: 
This class calculates the Sunrise/Noon/Sunset based on date and location ( latitude and longitude). An elevation angle of the sun is added to calculate the time of different types of twilights ( Civil, Nautical, Astronomical ).


**This code is based on this source that provides astronomy answers: position of the sun** [https://www.aa.quae.nl/en/reken/zonpositie.html]


## Steps

1. start by calculating the number of days since January 1, 2000. Add that number to 2451545 (the Julian day of January 1, 2000). This will be variable Jdate. 
```sh
int Sunrise::julian(const QDate &d)
{
    QDateTime dt(d, QTime(0, 0, 0, 0));
    return dt.toUTC().date().toJulianDay();
}
```
2.  next step is to calculate the Julian cycle. This is not equal to the days since Jan 1, 2000. Depending on your longitude, this may be a different number. 
```sh
 double julianCycle(int iJulianDay)
    {
        double n = double(iJulianDay) - 2451545.0009 - longitude / 360.0;
        return floor(n + 0.5);
    }
```
3.  Now, it is time to approximate the Julian date of solar noon. This is just an approximation so that we can make some intermediate calculations before we calculate the actual Julian date of solar noon.
```sh
 double solarNoonApprox(int iJulianDay)
    {
        return 2451545.0009  + longitude / 360.0 + julianCycle(iJulianDay);
    }
```
4.  Using the approximate value, calculate the mean solar anomaly. This will get a very close value to the actual mean solar anomaly.
```sh
    double solarMean(double noon)
    {
        double m = 357.5291 + 0.98560028 * (noon - 2451545.0);
        return normalizedAngle(m);
    }
```
5.  Calculate the equation of center
```sh
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
```
6.  Calculate the equation of center
```sh
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
```
7. Now, calculate the ecliptical longitude of the sun, using C and M.
```sh
    double gamma(double m)
    {
        double c = center(m);
        return normalizedAngle(m + 102.9372 + c + 180.0);
    }
```
8. Now there is enough data to calculate an accurate Julian date for solar noon.
```sh
    double solarTransit(double noon)
    {
        double m = solarMean(noon);
        double g = gamma(m);
        m = sin(dDegToRad(m));
        g = normalizedAngle(g * 2.0);
        g = sin(dDegToRad(g));
        return noon + 0.0053*m - 0.0069*g;
    }
```
9. To calculate the hour angle we need to find the declination of the sun (sd).
 then calculate the hour angle, which corresponds to half of the arc length of the sun at this latitude at this declination of the sun.
```sh
    bool omega(double &o, double noon)
    {
        double m = solarMean(noon);
        double g = gamma(m);
        double l = dDegToRad(latitude);
        double sd = sin(dDegToRad(g)) * sin(dDegToRad(23.45));
        double cd = cos(asin(sd));
        double el = 0;
        if (elevation > 0.0) 
        {
            el= -2.076*elevation/M_PI;
        }
        o = sin(dDegToRad(-0.83 + el)) - sin(l)*sd;
        o /= cos(l) * cd;
        if (o > 1.0 || o < -1.0)
          return false;
        o = dRadToDeg(acos(o));
        return true;
    }
```
10. time to go back through the approximation again, this time we use H in the calculation. The values of m and gamma from above don't really change from solar noon to sunset, so there is no need to recalculate them before calculating sunset.
```sh
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
```
11. calculate the noon
```sh
    double noon(int iJulianDay)
    {
        double noon = solarNoonApprox(iJulianDay);
        return solarTransit(noon);
    }
```
12. Instead of going through that mess again, assume that solar noon is half-way between sunrise and sunset (valid for latitudes < 60) and approximate sunrise.
```sh
    double sunrise(int iJulianDay)
    {
        double transit = noon(iJulianDay);
        double ss = sunset(iJulianDay);
        return ss < 0.0 ? -1.0 : transit - (ss - transit);
    }
```
**Now we have to add some conversions**
- convert angle from .°.'."" to double
```sh
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
```
- Convert radian angle to degrees
```sh
    double dRadToDeg(double dAngleRad)
    {
        return (180.0 * dAngleRad / M_PI);
    }
```
- Convert degree angle to radians
```sh
    double dDegToRad(double dAngleDeg)
    {
        return (M_PI * dAngleDeg / 180.0);
    }
```
- normlise angle
```sh
    double normalizedAngle(double a)
    {
        while (!(a < 360.0))
            a -= 360.0;
        while (!(a > -360.0))
            a += 360.0;
        return a;
    }
```
- add timezone function to convert the results from UTC to local time using the longitude.
```sh
    double timezone (double longitude)
    {
        return longitude > 0 ? floor((7.5+longitude)/15) : ceil((-7.5+longitude)/15) ;
    }
```
## Graphical user interface (using Qt Creator)
**We have four inputs, three outputs and a push button to release the calculation**
- first input: date
```sh
    QDate d;
     d = ui->dateEdit->date();
     cout << d.toString().toStdString() << endl;
```
- first input: date
```sh
    QDate d;
     d = ui->dateEdit->date();
     cout << d.toString().toStdString() << endl;
```
- rest of inputs: latitude, longitude, elevation angle
```sh
Sunrise sunrise( ui->lineEdit_latitude->text().toDouble(),                                           ui->lineEdit_longitude->text().toDouble(),                                          ui->lineEdit_elevation->text().toDouble() );
```
- first output: sunrise time
```sh
    ui->label_sunrise->setText("SUNRISE: "+sunrise.sunrise(d).toString()+ " UTC "+QString::fromUtf8(d_str1.c_str()));
```
- second output: noon time
```sh
    ui->label_noon->setText("NOON: "+sunrise.noon(d).toString()+ " UTC "+QString::fromUtf8(d_str1.c_str()));
```
- third output: sunset time
```sh
    ui->label_sunset->setText("SUNSET: "+sunrise.sunset(d).toString()+ " UTC "+QString::fromUtf8(d_str1.c_str()));
```

## Demo
to test the accurancy of this class we have to compare the outputs with real values from surise sunset site [https://sunrise-sunset.org/]
we choosed tunisia as an exemple:
- first we have to know the latitude and longitude
![tunisia coordinates](https://raw.githubusercontent.com/GhaylenTRIKI/Internship_WATTNOW_GPS/main/Work_Report/imgs/tunisa%20coordinates.PNG)
- then enter the appropriate coordinates and date. we fixed the elevation angle on -0.5 to have sunrise and sunset as outputs.
![graphical user interface](https://raw.githubusercontent.com/GhaylenTRIKI/Internship_WATTNOW_GPS/main/Work_Report/imgs/GUI.PNG)
- now we have to search for the real values in the site
![sunrise sunset time tunisia](https://raw.githubusercontent.com/GhaylenTRIKI/Internship_WATTNOW_GPS/main/Work_Report/imgs/sunrise%26sunset_time_tunisia.PNG)
- finally we notice that there is a maximum of 1 minute difference between the two results wich is acceptable.














