#ifndef SRC_UTILS_ASSERT_H__
#define SRC_UTILS_ASSERT_H__

#include <iostream>
#include <string>

#define assert_util(exp, msg) {if(!(exp)) {std::cout << msg << std::endl; exit(1);}}

#endif