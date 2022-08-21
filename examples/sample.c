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

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <mifsa/ota/client_c.h>

#ifdef WIN32
#include <Windows.h>
#define SSLEEP(t) Sleep(t * 1000)
#else
#include <unistd.h>
#define SSLEEP(t) sleep(t)
#endif

static void on_connect_changed(bool connected)
{
    if (connected) {
        printf("server connected\n");
    } else {
        printf("server disconnected\n");
    }
}

static void on_printf_location(const mifsa_ota_location_t* location)
{
    printf("*****print ota location\n");
    char nmea[150] = { 0 };
    char* nmea_addr = nmea;
    int nmea_size = 0;
    mifsa_ota_get_nmea_data(&nmea_addr, &nmea_size);
    printf("%s: %s\n", "nmea-data", nmea);
    printf("%s: %d\n", "size", location->size);
    printf("%s: %d\n", "flags", location->flags);
    printf("%s: %lf\n", "latitude", location->latitude);
    printf("%s: %lf\n", "longitude", location->longitude);
    printf("%s: %lf\n", "altitude", location->altitude);
    printf("%s: %f\n", "speed", location->speed);
    printf("%s: %f\n", "bearing", location->bearing);
    printf("%s: %f\n", "accuracy", location->accuracy);
    printf("%s: %" PRId64 "\n", "timestamp", location->timestamp);
    printf("%s: %d\n", "data-size", location->data_len);
    printf("*****\n\n");
    fflush(stdout);
}

int main(int argc, char* argv[])
{
    mifsa_ota_cb_tables_t cb_tables = {
        on_connect_changed,
        on_printf_location,
    };
    mifsa_ota_init(cb_tables);
    mifsa_ota_waitfor_server(-1);
    mifsa_ota_start_navigation(on_printf_location);
    SSLEEP(20);
    mifsa_ota_stop_navigation();
    mifsa_ota_deinit();
    return 0;
}
