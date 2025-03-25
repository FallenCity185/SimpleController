/**
 * function:菜单UI
 * author:MXY
 * date:2025/3/1
 **/
#include "mxy_menu.h"
#include "key_driver.h"
#include "oled_driver.h"
#include "BT_Comm.h"
#include "mpu6050_driver.h"
#include <Arduino.h>

void basic_display(menuIterm* iterm);
void basic_action(menuIterm* iterm);

void control_action(menuIterm* iterm);
void control_display(menuIterm* iterm);

void posCtl_display(menuIterm* iterm);
void posCtl_action(menuIterm* iterm);
/*---------------菜单项类函数实现-----------------*/
unsigned int menuIterm::menu_num = 0;

void emptyFunc(menuIterm* e){}

menuIterm::menuIterm(const char* t,
					menuIterm* r,
					menuIterm* f,
					void (*pDisplayFunc)(menuIterm*),
					void (*pActionFunc)(menuIterm*), 
					unsigned int dis_idx,
					unsigned int c_n,
					unsigned int s_n
					)
{
	if(pActionFunc != nullptr) this->pAction = pActionFunc; else this->pAction = emptyFunc;
	if(pDisplayFunc!= nullptr) this->pDisplay= pDisplayFunc;else this->pDisplay= emptyFunc;
	int i = 0;
	while(t[i]!='\0' && i< NAME_LENTH-1)
	{
		this->title[i] = t[i];
		i++;
	}
	this->title[i]='\0';
	if(r == nullptr) this->root = this;
	else this->root = r;

	if(f == nullptr) this->father = this;
	else f->addChild(this);

	this->child_num = c_n;
	for(int k=0;k<CHILD_NUM;k++)
	{
		this->child[k] = nullptr;
	}
	this->disp_index = dis_idx;
	this->serial_num = menu_num++;
}

void menuIterm::action()
{
	this->pAction(this);
}

void menuIterm::display()
{
	this->pDisplay(this);
}
//添加动作回调函数
void menuIterm::attachAction(void (*pActionFunc)(menuIterm*))
{
	this->pAction = pActionFunc;
}
//添加显示回调函数
void menuIterm::attachDisplay(void (*pDisplayFunc)(menuIterm*))
{
	this->pDisplay = pDisplayFunc;
}

void menuIterm::addChild(menuIterm* c)
{
	if(this->child_num < CHILD_NUM)
		this->child[this->child_num++] = c;
	// else printf("too much child");
	c->father = this;
}

void menuIterm::addParam(paramType t, int* p_int, int minVal, int maxVal) {
    this->param = paramClass(t, p_int, minVal, maxVal);
}

void menuIterm::addParam(paramType t, float* p_float, float minVal, float maxVal) {
    this->param = paramClass(t, p_float, minVal, maxVal);
}

void menuIterm::addRoot(menuIterm* r)
{
	this->root = r;
}

/*-----------------------------------------------应用层----------------------------------------------------*/
/*
root
	-car_state
		-angle
			-pic	val
			-row	val
			-yaw	val
		-voltage	val
	-paramSet
		-pos_p	val
		-pos_d	val
		-vel_p	val
		-vel_i	val
	-control
		(show)
	-posCtl
*/
//创建菜单	参数：当前菜单名称	链接根菜单	父级菜单	菜单显示函数指针	菜单执行函数指针
menuIterm root("root",&root,&root,basic_display,basic_action);
	menuIterm car_state("car_state",&root,&root,basic_display,basic_action);
		menuIterm pic("pic",&root,&car_state,basic_display,basic_action);
		menuIterm row("row",&root,&car_state,basic_display,basic_action);
		menuIterm yaw("yaw",&root,&car_state,basic_display,basic_action);
		menuIterm speed("speed",&root,&car_state,basic_display,basic_action);
	menuIterm param("paramSet",&root,&root,basic_display,basic_action);
		menuIterm pos_p("pos_p",&root,&param,basic_display,basic_action);
		menuIterm pos_d("pos_d",&root,&param,basic_display,basic_action);
		menuIterm vel_p("vel_p",&root,&param,basic_display,basic_action);
		menuIterm vel_i("vel_i",&root,&param,basic_display,basic_action);
	menuIterm control("control",&root,&root,control_display,control_action);
	menuIterm posCtl("posCtl",&root,&root,posCtl_display,posCtl_action);

menuIterm* pMenu = &root;	//可操作实例
 /**
 * Function : 初始化菜单子项
 * Description：目前用于链接菜单项和参数
 * 			  	完成链接后菜单项可显示参数
 * Param    : NULL
 * return   : NULL
 * author   ：MXY
 * date     : 2025/3/1
 */
void menuInit(void)
{
	//测试用
	my_param.p_pos=1.0;
	my_param.d_pos=0.1;
	my_param.p_vel=0.05;
	my_param.i_vel=0.02;

	//子菜单参数映射
	pos_p.addParam(float_type,&my_param.p_pos,0.0,10.0);
	pos_d.addParam(float_type,&my_param.d_pos,0.0,10.0);
	vel_p.addParam(float_type,&my_param.p_vel,0.0,10.0);
	vel_i.addParam(float_type,&my_param.i_vel,0.0,10.0);

	pic.addParam(float_type,&my_param.car_pit,0.0,10.0);
	row.addParam(float_type,&my_param.car_row,0.0,10.0);
	yaw.addParam(float_type,&my_param.car_yaw,0.0,10.0);
	speed.addParam(float_type,&my_param.car_speed,0.0,10.0);
}
 /**
 * Function : 基本菜单显示函数
 * Description：用于实现各个菜单的基本显示逻辑
 * 				通过函数指针调用
 * Param    : iterm 菜单类
 * return   : NULL
 * author   ：MXY
 * date     : 2025/3/1
 */
void basic_display(menuIterm* iterm)
{
	// 定义行高（根据需要调整）
  	int lineHeight = 10;
	OLEDprintf(0 * lineHeight, "   %s",pMenu->title);	//标题
	for(int i=0;i<pMenu->child_num;i++)
	{
		if(pMenu->child[i]->param.type == float_type && pMenu->child[i]->param.pfloat != NULL)//子参数 浮点数显示
		{
			OLEDprintf((i+1) * lineHeight,"   %d. %s:%.2f", i+1, pMenu->child[i]->title, *(pMenu->child[i]->param.pfloat));	
		}
		else if(pMenu->child[i]->param.type == int_type && pMenu->child[i]->param.pint != NULL)//子参数 整型数显示
		{
			OLEDprintf((i+1) * lineHeight,"   %d. %s:%d", i+1, pMenu->child[i]->title, *(pMenu->child[i]->param.pint));	
		}
		else //子参数 子菜单显示
			OLEDprintf((i+1) * lineHeight,"   %d. %s", i+1, pMenu->child[i]->title);
	}
	OLEDprintf(pMenu->disp_index * lineHeight, "->");	//箭头显示
}
 /**
 * Function : 基本菜单操作函数
 * Description：用于实现各个菜单的基本操作逻辑
 * 				通过函数指针调用
 * Param    : iterm 菜单类
 * return   : NULL
 * author   ：MXY
 * date     : 2025/3/1
 */
void basic_action(menuIterm* iterm)
{
    static unsigned long lastActionTime = 0;
    const unsigned long debounceDelay = 200; // 防抖延迟时间，单位为毫秒

    key_scan(&keyVal);
    KeyEnum key_state = (KeyEnum)get_key_state(&keyVal);

    unsigned long currentMillis = millis();
    
    if ((currentMillis - lastActionTime) > debounceDelay)
    {
        // 只有当防抖延迟已过或space键被按下（重置）时才处理动作
        switch(key_state)
        {
			case space: break;
            case key_m: break;
            case key_s: break;  // 确认
            case s_x_l: pMenu = pMenu->father; break;  // 返回父级
            case s_x_r: 
                if(pMenu->disp_index >= 1 && pMenu->child[pMenu->disp_index-1]->param.type == menu_type) 
                    pMenu = pMenu->child[pMenu->disp_index-1]; 
                break;  // 进入下级
            case s_y_l: 
                pMenu->disp_index = (pMenu->disp_index + 1 >= pMenu->child_num) ? pMenu->child_num : pMenu->disp_index + 1;
                break;  // 向下
            case s_y_r: 
                pMenu->disp_index = (pMenu->disp_index <= 0) ? 0 : pMenu->disp_index - 1; 
                break;  // 向上
            case m_s: pMenu = pMenu->root; break;  // 回到根目录
            case m_y_l:  // 参数--
				if (pMenu->param.type == int_type && pMenu->disp_index >=1) {				//整型参数
					int newVal = *(pMenu->child[pMenu->disp_index-1]->param.pint) - 1;
					*(pMenu->child[pMenu->disp_index-1]->param.pint) = LIMITER(newVal, pMenu->child[pMenu->disp_index-1]->param.minInt , pMenu->child[pMenu->disp_index-1]->param.maxInt);
					paramCMD_T.changeFlag = 1;	//参数改变	下载更新
				} else if (pMenu->child[pMenu->disp_index-1]->param.type == float_type) {	//浮点参数
					float newVal = *(pMenu->child[pMenu->disp_index-1]->param.pfloat) - 0.1f;
					*(pMenu->child[pMenu->disp_index-1]->param.pfloat) = LIMITER(newVal, pMenu->child[pMenu->disp_index-1]->param.minFloat , pMenu->child[pMenu->disp_index-1]->param.maxFloat);
					paramCMD_T.changeFlag = 1;	//参数改变	下载更新
				}
				break;  
            case m_y_r:  // 参数++
				if (pMenu->param.type == int_type && pMenu->disp_index >=1) {
					int newVal = *(pMenu->child[pMenu->disp_index-1]->param.pint) + 1;			//整型参数
					*(pMenu->child[pMenu->disp_index-1]->param.pint) = LIMITER(newVal, pMenu->child[pMenu->disp_index-1]->param.minInt , pMenu->child[pMenu->disp_index-1]->param.maxInt);
					paramCMD_T.changeFlag = 1;	//参数改变	下载更新
				} else if (pMenu->child[pMenu->disp_index-1]->param.type == float_type) {		//整型参数
					float newVal = *(pMenu->child[pMenu->disp_index-1]->param.pfloat) + 0.1f;
					*(pMenu->child[pMenu->disp_index-1]->param.pfloat) = LIMITER(newVal, pMenu->child[pMenu->disp_index-1]->param.minFloat , pMenu->child[pMenu->disp_index-1]->param.maxFloat);
					paramCMD_T.changeFlag = 1;	//参数改变	下载更新
				}
				break;  
            default: break;
        }
        // 更新最后的动作时间
        if(space != key_state) {
            lastActionTime = currentMillis;
        }
    }
}
 /**
 * Function : 控制菜单显示函数
 * Description：专门给控制菜单定义的显示函数
 * 				通过函数指针调用
 * Param    : iterm 菜单类
 * return   : NULL
 * author   ：MXY
 * date     : 2025/3/1
 */
void control_display(menuIterm* iterm)
{
	// 定义行高（根据需要调整）
  	int lineHeight = 10;
	OLEDprintf(0 * lineHeight, "   %s",pMenu->title);	//标题
}
 /**
 * Function : 控制菜单操作函数
 * Description：专门给控制菜单定义的操作函数
 * 				通过函数指针调用
 * Param    : iterm 菜单类
 * return   : NULL
 * author   ：MXY
 * date     : 2025/3/1
 */
void control_action(menuIterm* iterm)
{
	static unsigned long lastActionTime = 0;
    const unsigned long debounceDelay = 200; // 防抖延迟时间，单位为毫秒

	key_scan(&keyVal);
	KeyEnum key_state = (KeyEnum)get_key_state(&keyVal);

	unsigned long currentMillis = millis();
    if ((currentMillis - lastActionTime) > debounceDelay)
    {
		switch(key_state)
		{
			case space: controlCMD_T.key = (int)space;break;
			case key_m: controlCMD_T.key = (int)key_m;break;
			case key_s: controlCMD_T.key = (int)key_s;break;
			case s_x_l: controlCMD_T.key = (int)s_x_l;break;
			case s_x_r: controlCMD_T.key = (int)s_x_r;break;
			case s_y_l: controlCMD_T.key = (int)s_y_l;break;
			case s_y_r: controlCMD_T.key = (int)s_y_r;break; 
			case m_x_l: controlCMD_T.key = (int)m_x_l;break;
			case m_x_r: controlCMD_T.key = (int)m_x_r;break;
			case m_y_l: controlCMD_T.key = (int)m_y_l;break;
			case m_y_r: controlCMD_T.key = (int)m_y_r;break;
			case m_s  : pMenu = pMenu->father; break;  // 返回父级
			default:  break;
		};
		// 更新最后的动作时间
		if(space != key_state) {
			lastActionTime = currentMillis;
		}
	}
}
 /**
 * Function : 姿态控制菜单显示函数
 * Description：专门给姿态控制菜单定义的显示函数
 * 				通过函数指针调用
 * Param    : iterm 菜单类
 * return   : NULL
 * author   ：MXY
 * date     : 2025/3/1
 */
void posCtl_display(menuIterm* iterm)
{
	// 定义行高（根据需要调整）
  	int lineHeight = 10;
	OLEDprintf(0 * lineHeight, "   %s", pMenu->title);	//标题
	OLEDprintf(1 * lineHeight, "   pit:%.2f", statureCMD_T.pitch);
	OLEDprintf(2 * lineHeight, "   row:%.2f", statureCMD_T.row);
	OLEDprintf(3 * lineHeight, "   yaw:%.2f", statureCMD_T.yaw);
}

/**
* Function : 姿态控制菜单操作函数
* Description：专门给姿态控制菜单定义的操作函数
* 				通过函数指针调用
* Param    : iterm 菜单类
* return   : NULL
* author   ：MXY
* date     : 2025/3/1
*/
void posCtl_action(menuIterm* iterm)
{
	//保障基础的UI控制
	basic_action(iterm);
	//上传姿态参数
	statureCMD_T.pitch = mpu6050.getAngleX();
	statureCMD_T.row = mpu6050.getAngleY();
	statureCMD_T.yaw = mpu6050.getAngleZ();
}

/*------------------------------------action model-------------------------------------*/
/*
void action(menuIterm* iterm)
{
	static unsigned long lastActionTime = 0;
    const unsigned long debounceDelay = 200; // 防抖延迟时间，单位为毫秒

	key_scan(&keyVal);
	KeyEnum key_state = (KeyEnum)get_key_state(&keyVal);

	unsigned long currentMillis = millis();
    if ((currentMillis - lastActionTime) > debounceDelay || space == key_state)
    {
		switch(key_state)
		{
			case key_m: break;
			case key_s: break;
			case s_x_l: break;
			case s_x_r: break;
			case s_y_l: break;
			case s_y_r: break; 
			case m_s  : break;
			case m_x_l: break;
			case m_x_r: break;
			case m_y_l: break;
			case m_y_r: break;
			case xl_yl: break;
			case xl_yr: break;
			case xr_yl: break;
			case xr_yr: break;
			default:  break;
		};
		// 更新最后的动作时间
		if(space != key_state) {
			lastActionTime = currentMillis;
		}
	}
}
*/
/*------------------------------------action model-------------------------------------*/
