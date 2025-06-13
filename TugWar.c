// TugWar.c
//
/********************************************************************************
* 拔河仿真程序
*
* 1. 物理模型
* ①比赛区地面有个滑块，质量为1，地面摩檫力为1。
* ---------------------------M--------------------
* ②比赛区有阵风，最大风力w，方向随机向左或向右。
* ③左右两侧玩家可以对滑块施加拉力a1、a2>0，方向分别向左和向右。
* ④滑块的位置   pos(ts) = pos(ts - 1) + vel(ts - 1)
*         速度   vel(ts) = vel(ts - 1) + acc(ts) + f(ts)
*         加速度 acc(ts) = a2(ts) - a1(ts) + w(ts)。
*         摩檫力 f(ts) = ±1：与vel(ts-1)反向，vel(ts-1)为0时与acc(ts)反向，acc(ts)为0时f(ts)为0。
*   其中ts为时刻，向右为正向。
*
* 2. 胜负规则
* 整个比赛区分为四个区域：
*      零            一              二            三    
* ------------................________________------------
* ①比赛持续一定时间（PLAYTIME）。
* ②比赛过程中，如果滑块进入零区，则右侧胜；如果滑块进入三区，则左侧胜。
* ③比赛结束时，如果滑块在一区，则左侧胜，如果滑块在二区，则右侧胜。
*
* 3. 算法函数
* 两侧算法函数均为右侧视角，因此算法可以随意交换位置。
* 对于右侧算法函数，pos是滑块位置，vel是滑块速度，输出拉力实际方向向右（正）。
* 对于左侧算法函数，pos和vel是右侧算法函数的镜像，输出拉力实际方向向左（负）。
* 
* 4. 使用说明
* ①编写算法
* 在项目中添加一个c文件（例如myplayer.c），编写玩家函数。例程见player1.c。
* 
* ②测试
* 在TugWar.c中，按照的注释说明声明玩家函数并设置左侧和右侧的玩家函数。例程demo_player可以作为其中一侧玩家。
* 生成应用程序后运行。
* 
* ③比赛
* - 参赛者在项目目录中找到玩家函数的c文件，将c文件（例如myplayer.c）或相应的obj文件（例如myplayer.obj）发给裁判。
* - 裁判在项目中添加所有参赛者的c文件或obj文件，在TugWar.c中按照注释说明在玩家函数列表中声明玩家函数，设置参赛玩家函数。
* - 裁判在TugWar.c中按照注释说明设置比赛参数（比赛过程中不能改动）。
* - 裁判生成和运行应用程序，公布比赛结果。然后设置下一次参赛玩家函数后运行程序。如此反复直到完成比赛。
* 
* Writen by Lin, Jingyu, jylin@gdut.edu.cn, 2024.03
*
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct war_info
{
	// 比赛信息
	double pos;		// 滑块当前位置
	double vel;		// 滑块当前速度
	double wind;	// 实时风力
	double ts_left;	// 剩余时间（转换为整数）
	double zones[4];	// 比赛区域大小：[0] - 零区长度；[1] - 零区+一区长度；[2] - 零区+一区+二区长度；[3] - 比赛区长度
	
	// 玩家输出信息
	double player_acc;  // 玩家输出
	char* player_name;  // 玩家团队名称和成员姓名
};

/************************ 玩家函数 ************************/
// 在这里列出所有参赛玩家函数声明
void demo_player(struct war_info* info);	// 示例玩家
void random_player(struct war_info* info);	// 示例玩家
void player1(struct war_info* info);
void enimy(struct war_info* info);



// 在这里列出所有参赛玩家函数和名称
void (*playeracc[])(struct war_info* info) =
{ 
	demo_player, player1, random_player,enimy
};

// 正式比赛时，在命令行输入“TugWar <p1> <p2>”，其中<p1>、<p2>是参赛玩家编号
// 测试时，在这里选择本次参赛的玩家函数编号
int LEFT = 3;	// 左侧玩家
int RIGHT = 1;	// 右侧玩家


/************************ 在这里设置比赛参数 ************************/
#define PLAYTIME 100 /*比赛时间（节拍数）*/
#define SHOWRATE 150 /*显示间隔（ms）*/
#define WIND 2 /*最大风力*/
#define ZONELEN 100   /*比赛区长度*/
#define WINZONE 40  /*胜负区长度*/

#define INIT_POS (ZONELEN / 2)  /*滑块初始位置*/
#define ZONE_C (ZONELEN / 2)
#define ZONE1 (ZONE_C - WINZONE)
#define ZONE3 (ZONE_C + WINZONE)


/************************ 玩家例程，不要改动 ************************/

void demo_player(struct war_info* info)
{
	// 玩家团队名称和成员姓名
	info->player_name = "大魔王";

	double pos_target, vel_target, acc_target;
	//pos_target = (info.zones[1] + info.zones[2]) / 2;
	pos_target = info->zones[1] + 9*sizeof(double);
	if (info->pos > pos_target)
	{
		// 输出加速度
		info->player_acc = 0;
		return;
	}

	vel_target = (double)sqrt(0.5 * (pos_target - info->pos));
	acc_target = (vel_target - info->vel) / 2.0;
	if (acc_target < 0)
		acc_target = 0;
		

	// 输出加速度
	info->player_acc = acc_target;

}

void random_player(struct war_info* info)
{
	// 玩家团队名称和成员姓名
	info->player_name = "随机玩家";

	double a = rand() % 5;

	// 输出加速度
	info->player_acc = a;

}


/************************ 主程序，不要改动 ************************/

void clear_buffer(char* buf)
{
	for (int i = 0; i < ZONE1; ++i)
		buf[i] = ' ';
	for (int i = ZONE1; i < ZONE_C; ++i)
		buf[i] = '.';
	for (int i = ZONE_C; i < ZONE3; ++i)
		buf[i] = '_';
	for (int i = ZONE3; i < ZONELEN; ++i)
		buf[i] = ' ';
}

void display(char* buf, int ts)
{
	//for (int j = 0; j < ZONELEN; ++j)
	//	printf("\b");
	printf("\r%s%.3d",buf, ts);
}

int main(int argc, char* argv[])
{
	if (argc > 2)
	{
		LEFT = strtol(argv[1], NULL, 10);
		RIGHT = strtol(argv[2], NULL, 10);
		system("cls");
	}

	// 初始化
	char buf[ZONELEN + 1];
	struct war_info rt_info = { 0, 0, 0, 0, { ZONE1, ZONE_C, ZONE3, ZONELEN }, 0, 0 };
	double flag_pos, flag_vel, flag_acc, acc[2]; // 滑块的位置、速度、加速度和输入加速度
	flag_pos = INIT_POS;
	flag_vel = 0;
	buf[ZONELEN] = 0;
	

	// 玩家名称
	rt_info.player_acc = 0; 
	rt_info.player_name = NULL;
	playeracc[LEFT](&rt_info);
	char* playername_LEFT = rt_info.player_name;
	if (!playername_LEFT)
		playername_LEFT = "无名队";

	rt_info.player_acc = 0;
	rt_info.player_name = NULL;
	playeracc[RIGHT](&rt_info);
	char* playername_RIGHT = rt_info.player_name;
	if (!playername_RIGHT)
		playername_RIGHT = "无名队";

	srand((unsigned int)time(0));
	printf("\n\t左边：（第 %d 队)  %s", LEFT, playername_LEFT);
	printf("\n\t右边：（第 %d 队)  %s", RIGHT, playername_RIGHT);
	printf("\n\n");

	// 主循环
	for (int ts = PLAYTIME; ts > 0; --ts)
	{
		// 计算位置
		flag_pos += flag_vel;
		if (flag_pos < 0)
			flag_pos = 0;
		else if (flag_pos >= ZONELEN)
			flag_pos = ZONELEN - 1;

		// 随机风
		const int mu = 10;
		double wind = (rand() % (WIND * 2 * mu + 1) - WIND * mu) / (double)mu;
		//double wind = (int)round((double)rand() / RAND_MAX * (WIND + WIND)) - WIND;

		// 玩家输入加速度
		rt_info.ts_left = ts; // 剩余时间
		rt_info.pos = flag_pos; // 滑块当前位置
		rt_info.vel = flag_vel; // 滑块当前速度
		rt_info.wind = wind;	// 风力
		rt_info.player_acc = 0;
		playeracc[RIGHT](&rt_info);
		acc[1] = rt_info.player_acc;
		if (acc[1] < 0)
			acc[1] = 0;

		rt_info.pos = ZONELEN - 1 - flag_pos; // 滑块当前位置（镜像）
		rt_info.vel = -flag_vel; // 滑块当前速度（镜像）
		rt_info.wind = -wind;	// 风力（镜像）
		rt_info.player_acc = 0;
		playeracc[LEFT](&rt_info);
		acc[0] = rt_info.player_acc;
		if (acc[0] < 0)
			acc[0] = 0;

		flag_acc = wind + acc[1] - acc[0];

		// 摩擦力
		double friction = 0;
		if (flag_vel > 0)
			friction = -1.0;
		else if (flag_vel < 0)
			friction = 1.0;
		else
		{
			if (flag_acc > 1.0)
				friction = -1.0;
			else if (flag_acc < -1.0)
				friction = 1.0;
			else
				friction = -flag_acc;
		}

		// 计算速度
		flag_acc += friction;
		flag_vel += flag_acc;

		// 显示状态
		clear_buffer(buf);
		if (flag_pos >= 0 && flag_pos < ZONELEN)
			buf[(int)flag_pos] = 'M';
		display(buf, ts);
		if (flag_pos < ZONE1 || flag_pos >= ZONE3)
			break; // 比赛结束

		// 延时
		printf("\nflag_pos:%f acc[0]=%f acc[1]=%f vel=%f",flag_pos,acc[0],acc[1],flag_vel);//仅测试
		if(ts==2)
		{
			putchar('\n');
		}
		clock_t clk = clock();
		while (clock() - clk < SHOWRATE);
	}

	// 比赛结束，判断胜负
	printf("\n\n\n");
	if (flag_pos < ZONE1)
		printf("\t胜者是 >>>>>>>> 第 %d 队 - %s <<<<<<<<\n", RIGHT, playername_RIGHT);
	else if (flag_pos >= ZONE3)
		printf("\t胜者是 >>>>>>>> 第 %d 队 - %s <<<<<<<<\n", LEFT, playername_LEFT);
	else if (flag_pos < ZONE_C)
		printf("\t胜者是 >>>>>>>> 第 %d 队 - %s <<<<<<<<\n", LEFT, playername_LEFT);
	else
		printf("\t胜者是 >>>>>>>> 第 %d 队 - %s <<<<<<<<\n", RIGHT, playername_RIGHT);

	printf("\n\n\n");
	return 0;
}
