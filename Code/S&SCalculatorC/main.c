#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAX_YR  9999
#define MIN_YR  1900


double longitude= -10;
double latitude=30;
double elevation=0;

typedef struct Date{
    int day;
    int month;
    int year;

} Date;

typedef struct DateTime{
    int day;
    int month;
    int year;
    int hour;
    int min;
    int sec ;
} DateTime;

typedef struct Time{
int hour;
int min;
int sec;
}Time;


typedef struct Calculations{
    double latitude;
    double longitude;
    double elevation;
}Calculations;

Calculations setCalculationValues(double latitude, double longitude, double elevation ){
    Calculations calc;
    calc.elevation = elevation;
    calc.latitude = latitude;
    calc.longitude = longitude;
}

Date toDate(int year1, int month1,int day1){
    Date dt;
    dt.day = day1;
    dt.month = month1;
    dt.year = year1;
    return dt;
}

//needs time format control
Time toTime(int hour, int minute,int second){
    Time t;
    t.hour= hour;
    t.min=minute;
    t.sec=second;
    return t;
}

double timezone (double longitude)
    {
        return longitude > 0 ? (floor((7.5+longitude)/15)) : (ceil((-7.5+longitude)/15)) ;
    }

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
        double n = (iJulianDay) - 2451545.0009 - longitude / 360.0;
        return floor(n + 0.5);


    }

double solarNoonApprox(int iJulianDay)
    {
        return 2451545.0009  + longitude / 360.0 + julianCycle(iJulianDay);
    }

double solarMean(double noon)
    {
        double m =357.5291 + 0.98560028 * (noon - 2451545.0);
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

double solarTransit(double noon)
    {
        double m = solarMean(noon);
        double g = gamma(m);
        m = sin(dDegToRad(m));
        g = normalizedAngle(g * 2.0);
        g = sin(dDegToRad(g));
        return noon + 0.0053*m - 0.0069*g;
    }

bool omega(double *o, double noon)
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
         *o = sin(dDegToRad(-0.83 + el)) - sin(l)*sd;
        *o /= cos(l) * cd;
        if (*o > 1.0 || *o < -1.0)
          return false;
        *o = dRadToDeg(acos(*o));
        return true;
    }

double sunsetcalc(int iJulianDay)
     {
       double noon = solarNoonApprox(iJulianDay);
        double m = solarMean(noon);
        double g = gamma(m);
        m = sin(dDegToRad(m));
        g = normalizedAngle(g * 2.0);
        g = sin(dDegToRad(g));
        double p=0;
        double *o= &p;
        if (!omega(o, noon))
          return -1.0;
        p += longitude;
        p /= 360.0;
        return 2451545.0009 + p + julianCycle(iJulianDay) + (0.0053 * m) - (0.0069 * g);

    }

double nooncalc(int iJulianDay)
    {
        double noon = solarNoonApprox(iJulianDay);
        return solarTransit(noon);
    }

double sunrisecalc (int iJulianDay)
    {
        double transit = nooncalc (iJulianDay);
        double ss = sunsetcalc(iJulianDay);
        return ss < 0.0 ? -1.0 : transit - (ss - transit);
    }

double DayLengthcalc (int iJulianDay)
    {
        double ss = sunsetcalc(iJulianDay);
        double sr = sunrisecalc(iJulianDay);
        return ss < 0.0 ? -1.0 : 0.5+ (ss - sr);
    }

//leap year
int  IsLeapYear(int year)
{
    return (((year % 4 == 0) &&
             (year % 100 != 0)) ||
            (year % 400 == 0));
}

// returns 1 if given date is valid.
int isValidDate(Date *validDate)
{
    //check range of year,month and day
    if (validDate->year > MAX_YR ||
            validDate->year < MIN_YR)
        return 0;
    if (validDate->month < 1 || validDate->month > 12)
        return 0;
    if (validDate->month < 1 || validDate->month > 31)
        return 0;
    //Handle feb days in leap year
    if (validDate->month == 2)
    {
        if (IsLeapYear(validDate->year))
            return (validDate->day <= 29);
        else
            return (validDate->day <= 28);
    }
    //handle months which has only 30 days
    if (validDate->month == 4 || validDate->month == 6 ||
            validDate->month == 9 || validDate->month == 11)
        return (validDate->day <= 30);
    return 1;
}

// add secs to the time from the julian date
Time addSeconds (Time t, int Secs){
    if (Secs >= 86400){
        Secs%=86400;
        t.hour += (Secs/3600);
        Secs%=3600;
        t.min += (Secs/60);
        Secs%=60;
        t.sec += Secs;
        if (t.sec>=60){
            t.sec=t.sec-60;
            t.min=t.min+1;
        }
    }
    else if (Secs>=3600){
        t.hour += (Secs /3600) ;
        Secs%=3600;
        t.min += (Secs/60);
        Secs%=60;
        t.sec += Secs;
        if (t.sec>=60){
            t.sec=t.sec-60;
            t.min=t.min+1;
        }
           }
    else if ( Secs >= 60){
        t.min +=(Secs / 60) ;
        Secs%=60;
        t.sec += Secs;
        if (t.sec>=60){
            t.sec=t.sec-60;
            t.min=t.min+1;
        }
    }
    else{
        t.sec += Secs;
         if (t.sec>=60){
            t.sec=t.sec-60;
            t.min=t.min+1;
        }
    }
    return t;
}

//print date time
void printDateTime(DateTime dt){
    int test= 0;
    if (dt.min<10 && dt.sec<10 && dt.hour<10 && dt.day<10 && dt.month<10){
        printf("Date : %d/0%d/0%d Time : 0%d:0%d:0%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        test = 1;
    }
    else if (dt.min<10  && dt.hour<10 && dt.day<10 && dt.month<10){
        printf("Date : %d/0%d/0%d Time : 0%d:0%d:%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        test = 1;
    }
    else if (dt.min<10 && dt.sec<10  && dt.day<10 && dt.month<10){
        printf("Date : %d/0%d/0%d Time : 0%d:%d:0%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        test = 1;
    }
    else if (dt.min<10 && dt.sec<10 && dt.hour<10  && dt.month<10){
        printf("Date : %d/0%d/%d Time : 0%d:0%d:0%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        test = 1;
    }
    else  if (dt.min<10 && dt.sec<10 && dt.hour<10 && dt.day<10 ){
        printf("Date : %d/%d/0%d Time : 0%d:0%d:0%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        test = 1;
    }
    else if (dt.sec<10 && dt.hour<10 && dt.day<10 && dt.month<10){
        printf("Date : %d/0%d/0%d Time : 0%d:%d:0%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        test = 1;
    }
    else if (dt.sec<10  && dt.day<10 && dt.month<10){
        printf("Date : %d/0%d/0%d Time : %d:%d:0%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        test = 1;
    }
    else if (dt.sec<10  && dt.min<10 && dt.month<10){
        printf("Date : %d/0%d/%d Time : %d:0%d:0%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        test = 1;
    }

    else if (dt.hour<10 && dt.day<10 && dt.month<10){
        printf("Date : %d/0%d/0%d Time : 0%d:%d:%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        test = 1;
    }
     else if (dt.day<10 && dt.month<10){
        printf("Date : %d/0%d/0%d Time : %d:%d:%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        test = 1;
    }
    else if (dt.month<10 ){
        printf("Date : %d/0%d/%d Time : %d:%d:%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        test = 1;
    }
    else if (dt.sec<10 ){
        printf("Date : %d/%d/%d Time : %d:%d:0%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        test = 1;
    }
    if (test==0)
        printf("Date : %d/%d/%d Time : %d:%d:%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
}

//DateTime constractor
DateTime toDateTime(Date currentDT, int hours, int mins, int secs){
    DateTime dt;
    dt.day = currentDT.day;
    dt.month = currentDT.month;
    dt.year = currentDT.year;
    dt.hour = hours;
    dt.min = mins;
    dt.sec = secs;
    return dt;
}

//convert julian date to normal date
Date jd_to_date(int jd){
    Date date;
    int a = jd + 32044;
    int b = (((4 * a) + 3) / 146097);
    int c = a - ((b * 146097) / 4);

    int d = (((4 * c) + 3) / 1461);
    int e = c - ((d * 1461) / 4);
    int m = (((5 * e) + 2) / 153);
    int m2= (m / 10);

    date.day   =  (e + 1 - (((153 * m) + 2) / 5));
    date.month =  (m + 3 - (12 * m2));
    date.year  =  ((b*100) + d - 4800 + m2);

    return date ;
}

//Convert normal date to julian date
int toJulianDay(Date d) {
    int a = ((14 - d.month) / 12);
    int y = (d.year + 4800 - a);
    int m = (d.month + (12 * a) - 3);
    return (d.day + (((153*m)+2)/5) + (y*365) + (y/4) - (y/100) + (y/400) - 32045)-1;
}

//replaces date function in qt that converts the julian date to time
 Time toGeorgTime(double julianDay){
    if (julianDay<0.0){
        Time t;
        t.sec=0;
        t.hour=0;
        t.min=0;
        return t;
    }
    int julianDayint = floor(julianDay);
    int julianSecs = (julianDay - julianDayint) * 86400 ;
    Time time;
    time = toTime(12,0,0);
    time = addSeconds(time, julianSecs);
    if (time.hour>=24){
        time.hour-=24;
    }
    return time;
}

Time sunrise (Date d)
{
    if (isValidDate(&d)==0)
    {
    Time t;
    t.hour=0;
    t.min=0;
    t.sec=0;
        return t;
    }
    int k= toJulianDay(d);
    double j = sunrisecalc (k);
    j=j+(timezone(-longitude)/24);
    return toGeorgTime(j);
}

Time noon (Date d)
{
    if (isValidDate(&d)==0)
    {
    Time t;
    t.hour=0;
    t.min=0;
    t.sec=0;
        return t;
    }
    int k= toJulianDay(d);
    double j = nooncalc (k);
    j=j+(timezone(-longitude)/24);
    return toGeorgTime(j);
}

Time sunset (Date d)
{
    if (isValidDate(&d)==0)
    {
    Time t;
    t.hour=0;
    t.min=0;
    t.sec=0;
        return t;
    }
    int k= toJulianDay(d);
    double j = sunsetcalc (k);
    j=j+(timezone(-longitude)/24);
    return toGeorgTime(j);

}
Time DayLength(Date d)
{
    int k= toJulianDay(d);
    double j = DayLengthcalc (k);
    return toGeorgTime(j);
}

int main()
{
    Date dt;
    double latitude, longitude, elevation;

    printf("enter the date \n");
    printf("day: \n");
    scanf("%d",&dt.day);
    printf("month: \n");
    scanf("%d",&dt.month);
    printf("year: \n");
    scanf("%d",&dt.year);

    printf("enter the latitude \n");
    scanf("%d",&latitude);

    printf("enter the longitude \n");
    scanf("%d",&longitude);

    printf("enter the elevation angle of the sun \n");
    scanf("%d",&elevation);

    int julian= toJulianDay(dt);
    printf("julian day  %d\n",julian);
    Time sr,n,ss,leng;
    ss=sunset(dt);
    sr=sunrise(dt);
    n=noon(dt);
    leng=DayLength(dt);
    printf("sunrise time is %d:%d:%d\n",sr.hour,sr.min,sr.sec);
    printf("noon time is %d:%d:%d\n",n.hour,n.min,n.sec);
    printf("sunset time is %d:%d:%d\n",ss.hour,ss.min,ss.sec);
    printf("daylength is %d:%d:%d\n",leng.hour,leng.min,leng.sec);

    return 0;
}
