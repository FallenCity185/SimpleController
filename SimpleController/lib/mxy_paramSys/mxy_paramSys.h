/**
 * function:参数管理
 * author:MXY
 * date:2025/3/1
 **/
#ifndef MXY_PARAMSYS_H
#define MXY_PARAMSYS_H

#define LIMITER(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

typedef struct
{
    //下发PID
    float p_pos;
    float d_pos;
    float p_vel;
    float i_vel;

    //小车数据回传
    float car_pit;
    float car_row;
    float car_yaw;
    float car_speed;

}mxy_paramType;

enum paramType{
	menu_type,
	int_type,
	float_type
};

class paramClass{
	public:
	paramType type;	//0:表现为菜单项 1:int 2:float
	union {
        int* pint;
        float* pfloat;
    };
    union {
        int minInt;
        float minFloat;
    };
    union {
        int maxInt;
        float maxFloat;
    };
	paramClass() : type(menu_type), pint(nullptr), minInt(0), maxInt(0) {};
    paramClass(paramType t, int* pi = nullptr, int minVal = 0, int maxVal = 0);
    paramClass(paramType t, float* pf = nullptr, float minVal = 0.0f, float maxVal = 0.0f);
	// 复制构造函数
    paramClass(const paramClass& other);
    // 赋值操作符重载
    paramClass& operator=(const paramClass& other);
};


/*------------------------Param manager--------------------------*/
// class ParamManager {
//     public:
//         static void registerParam(menuIterm* item);
//         static void saveToStorage();
//         static void loadFromStorage();
        
//     private:
//         static const int MAX_PARAMS = 50;
//         static menuIterm* params[MAX_PARAMS];
//         static int paramCount;
//     };

    extern mxy_paramType my_param;

#endif
