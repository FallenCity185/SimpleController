#ifndef MXY_MENU_H
#define MXY_MENU_H

#define CHILD_NUM 5	//
#define NAME_LENTH 15

#include "mxy_paramSys.h"


class menuIterm{
	private:
	    static unsigned int menu_num;	//菜单项个数 生成菜单项编号
		void (*pAction)(menuIterm*);
		void (*pDisplay)(menuIterm*);
	
	public:
		char title[NAME_LENTH];
		unsigned int child_num;
		menuIterm * root;
		menuIterm * child[CHILD_NUM];
		paramClass param;
		menuIterm * father;
		unsigned int disp_index;		//显示索引
		unsigned int serial_num;		//菜单项编号 （即第几个创建的对象）

		menuIterm(const char* t = "",
	              menuIterm* r = nullptr,
	              menuIterm* f = nullptr,
				  void (*pDisplayFunc)(menuIterm*)=nullptr,
				  void (*pActionFunc)(menuIterm*)=nullptr, 
	              unsigned int dis_idx = 0,
				  unsigned int c_n = 0,
	              unsigned int s_n = 0
				  );
		void action();	//动作函数调用
		void display();	//显示函数调用
        void attachAction(void (*pActionFunc)(menuIterm*)); 	//添加动作回调函数
        void attachDisplay(void (*pDisplayFunc)(menuIterm*)); 	//添加显示回调函数
		void addChild(menuIterm* c);							//添加子菜单
		// 参数添加
		void addParam(paramType t, int* p_int, int minVal, int maxVal);
		void addParam(paramType t, float* p_float, float minVal, float maxVal);
		void addRoot(menuIterm* r);								//添加根目录
};

extern menuIterm* pMenu;

void menuInit(void);
void basic_display(menuIterm* iterm);
void basic_action(menuIterm* iterm);

#endif
