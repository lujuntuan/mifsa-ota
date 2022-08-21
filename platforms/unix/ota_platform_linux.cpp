/*********************************************************************************
 *Copyright(C): Juntuan.Lu, 2020-2030, All rights reserved.
 *Author:  Juntuan.Lu
 *Version: 1.0
 *Date:  2022/04/01
 *Email: 931852884@qq.com
 *Description:
 *Others:
 *Function List:
 *History:
 **********************************************************************************/

#include <mifsa/ota/platform.h>

MIFSA_NAMESPACE_BEGIN

std::vector<std::string> _test_sentences = {
    { "$GNRMC,032745.000,A,2234.6069,N,11356.4075,E,0.000,156.85,230522,,,A,V*3E\n" },
    { "$GNGST,032743.000,5.7,4.0,3.2,106.3,3.2,3.9,15.0*44\n" },
    { "$GNGST,032744.000,5.7,4.0,3.1,108.5,3.2,3.9,15.4*4C\n" },
    { "$GNGSA,A,3,78,88,76,81,87,,,,,,,,0.94,0.63,0.69,2*0D\n" },
    { "$GNRMC,032746.000,A,2234.6069,N,11356.4075,E,0.000,156.85,230522,,,A,V*3D\n" },
    { "$GNGSA,A,3,25,36,,,,,,,,,,,0.94,0.63,0.69,3*06\n" },
    { "$GNGSA,A,3,03,02,07,,,,,,,,,,0.94,0.63,0.69,5*04\n" },
    { "$GPGSV,3,1,12,01,61,036,37.2,30,55,253,34.1,07,49,203,32.9,41,46,237,32.0,1*62\n" },
    { "$GPGSV,3,2,12,50,46,122,,14,43,333,33.9,17,33,292,30.4,03,30,128,34.4,1*76\n" },
    { "$GPGSV,1,1,4,01,61,036,27.7,30,55,253,25.8,14,43,333,25.0,03,30,128,23.1,8*5D\n" },
    { "$GBGSV,3,1,12,37,70,175,36.4,07,62,004,32.0,27,60,314,31.8,08,49,179,29.5,1*7B\n" },
    { "$PMTKVNED,109182,0.01,0.02,0.20,0.02,0.20,0.47,0.58,0.54,1638.205*03\n" },
    { "$PMTKVNED,109182,0.01,0.02,0.20,0.02,0.20,0.47,0.58,0.54,1638.205*04\n" },
    { "$PMTKCHL,0,014,21837879.94,-444870.185,4216.9,0,38,582353.88,15970998.00,21198742.00,00,C0,0.00,0,3*15\n" },
    { "$PMTKCHL,0,030,21163122.56,-121961.393,1306.7,0,37,958377.81,25804502.00,5703229.00,00,49,0.00,0,3*5A\n" },
    { "$PMTKCHL,1,017,21145926.19,-247319.671,3623.9,0,29,7176543.50,24357066.00,2621090.75,12,100,0.00,0,3*5D\n" },
    { "$PMTKCHL,1,023,22316216.64,0.000,-1306.3,2,28,-9863527.00,3265915.25,23281452.00,11,100,0.00,0,3*4C\n" },
    { "$PMTKCHL,2,230,24601475.80,0.000,3351.5,3,31,0.00,0.00,0.00,00,00,0.00,0,3*48\n" },
    { "$PMTKCHL,2,213,37991426.67,-122582.898,3127.8,0,33,0.00,0.00,0.00,00,00,0.00,0,3*57\n" },
    { "$PMTKAGC,032744.000,2868,3064,6120,6136,0,0,0,6,117,0*70\n" },
    { "$PMTKVNED,111182,0.02,0.06,0.19,0.06,0.19,0.49,0.58,0.54,477.807*37\n" },
    { "$PMTKGRP,111182,00098882.001,2211,3,18,54103*58\n" },
    { "$GNGST,032744.000,5.7,4.0,3.1,108.5,3.2,3.9,15.4*4C\n" },
    { "$GNACCURACY,1.4,180.0,0.9,15.3*26\n" },
    { "$GNVTG,156.85,T,,M,0.000,N,0.000,K,A*2C\n" },
    { "$GNGGA,032747.000,2234.6069,N,11356.4075,E,1,26,0.58,223.6,M,-2.6,M,,*6A\n" },
    { "$GNGGA,032748.000,2234.6069,N,11356.4075,E,1,26,0.58,223.6,M,-2.6,M,,*65\n" },
    { "$GNACCURACY,1.3,180.0,0.9,16.3*22\n" },
};

int _index = 0;

namespace Ota {
class PlatformUnix : public Platform {
public:
    virtual std::string getNmea() override
    {
        if (_index < 0 || _index >= _test_sentences.size()) {
            _index = 0;
        }
        const auto& nmea = _test_sentences.at(_index);
        _index++;
        return nmea;
    }
};

MIFSA_CREATE_PLATFORM(PlatformUnix, 1, 0);
}

MIFSA_NAMESPACE_END
