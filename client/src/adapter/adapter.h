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

#if defined(MIFSA_SUPPORT_ROS)
#include "client_interface_ros.hpp"
#elif defined(MIFSA_SUPPORT_VSOMEIP)
#include "client_interface_vsomeip.hpp"
#elif defined(MIFSA_SUPPORT_FDBUS)
#include "client_interface_fdbus.hpp"
#else
#include "client_interface_custom.hpp"
#endif
