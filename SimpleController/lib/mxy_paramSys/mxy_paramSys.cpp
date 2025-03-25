#include "mxy_paramSys.h"

mxy_paramType my_param;

/*参数类*/
paramClass::paramClass(paramType t, int* pi, int minVal, int maxVal) {
    type = t;
    if(type == int_type)
    {
        pint = pi;
        minInt = minVal;
        maxInt = maxVal;
    }
}

paramClass::paramClass(paramType t, float* pf, float minVal, float maxVal) {
    type = t;
    if(type == float_type)
    {
        pfloat = pf;
        minFloat = minVal;
        maxFloat = maxVal;
    }
}

    // 复制构造函数
paramClass::paramClass(const paramClass& other) {
	type = other.type;
    if(type == int_type)
    {
        pint = other.pint;
    }else if(type == float_type)
    {
        pfloat = other.pfloat;
    }
    minFloat = other.minFloat;
    maxFloat = other.maxFloat;
}

// 赋值操作符重载
paramClass& paramClass::operator=(const paramClass& other) {
	type = other.type;
    if(type == int_type)
    {
        pint = other.pint;
    }else if(type == float_type)
    {
        pfloat = other.pfloat;
    }
    minFloat = other.minFloat;
    maxFloat = other.maxFloat;
	return *this;
}


/*------------------------Param manager--------------------------*/

// menuIterm* ParamManager::params[] = {nullptr};
// int ParamManager::paramCount = 0;

// void ParamManager::registerParam(menuIterm* item) {
//     if (paramCount < MAX_PARAMS) {
//         params[paramCount++] = item;
//     }
// }

// void ParamManager::saveToStorage() {
//     // 这里添加具体存储实现（如EEPROM）
//     for (int i = 0; i < paramCount; i++) {
//         menuIterm* item = params[i];
//         if (item->param.type == int_type) {
//             // 保存int类型
//         } else if (item->param.type == float_type) {
//             // 保存float类型
//         }
//     }
// }

// void ParamManager::loadFromStorage() {
//     // 这里添加具体加载实现
// }

