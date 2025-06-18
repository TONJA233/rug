// Fin_player.c v114.514
// 风相对与自己，风是往自己吹的为正，所以抵消风力要加速度=-风力+...
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define I_SAFE(MyAcc, Boundary) (is_Safe(info->pos, MyAcc, info->vel, info->wind, (int)(info->ts_left), Boundary))
#define YOU_DANGER(YourAcc, Boundary) (!is_Safe(100 - info->pos - 1, -YourAcc, -info->vel, -info->wind, (int)(info->ts_left), Boundary))
// x为我方加速度
// 以上都是基于选手在右侧的情况
#define GetTypeEnd 45
// 判断敌人类型结束的时刻

static int EnemyType = -1; // 敌人类型 0摆烂类，1 进攻型sadism

struct war_info // 游戏已经将我们的视角对称为右方玩家
{
	// 比赛信息
	double pos;		 // 滑块当前位置
	double vel;		 // 滑块当前速度
	double wind;	 // 实时风力
	double ts_left;	 // 剩余时间（转换为整数）
	double zones[4]; // 比赛区域大小：[0] - 零区长度；[1] - 零区+一区长度；[2] - 零区+一区+二区长度；[3] - 比赛区长度

	// 玩家输出信息
	double player_acc; // 玩家输出
	char *player_name; // 玩家团队名称和成员姓名
};

/// @brief 摩擦力的代码
/// @param acc 当前加速度
/// @param vel 当前速度
/// @return 摩擦力
static double GetFriction(double acc, double vel)
{
	double friction = 0;
	// 摩擦计算
	if (vel > 0)
	{
		friction = -1.0;
	}
	else if (vel < 0)
	{
		friction = 1.0;
	}
	else
	{
		if (acc > 1.0)
		{
			friction = -1.0;
		}
		else if (acc < -1.0)
		{
			friction = 1.0;
		}
		else
		{
			friction = -acc;
		}
	} // 这么好的代码居然免费拿
	return friction;
}

/// @brief 位置安全检测（对于右侧玩家）
/// @param Pos 当前位置
/// @param acc 当前加速度
/// @param vel 当前速度
/// @param wind 当前风力
/// @param count 预测步长：预测count时刻后的位置，小于等于0运行到速度为0；
/// @param boundary 边界（对于右侧玩家）
/// @return 1-安全 0-危险
static int is_Safe(double Pos, double acc, double vel, double wind, int count, int boundary)
{
	double predict_pos, real_acc;

	real_acc = wind + GetFriction(acc, vel) + acc; // 此时加速度
	vel += real_acc;							   // 此时速度
	if (vel - count <= 0)						   // 预测步长小于等于0，即为运行到速度为0（非常保守）；
	{
		count = vel;
	} // 这里count也代表数列vel,vel-1,vel-2.....,1,0的项数
	predict_pos = Pos + ((vel + (vel - count - 1)) * (count)) / 2; // 预测后count时刻的位置
	if (predict_pos > boundary)
	{
		return 0;
	}
	return 1;
}

/// @brief 获取敌人类型，赋值到全局变量EnemyISStableType
/// @param info 结构体地址
static void GetEnemyType(struct war_info *info)
{
	static double accqq = 0, velqq = 0, OnlyWindPos;
	if (info->ts_left == 100)
	{
		OnlyWindPos = 50;
		// 很危险的代码，我们不用拉也可以判断敌人类型
		//  info->player_acc = 1; // 开始先拉过来，主动出击判断类型，不要太大，风有概率把你吹飞（经常）
		//  info->wind += info->player_acc;
	}

	if (info->ts_left != 100)
		OnlyWindPos += velqq; // 计算只有风且我自己不一直拉的情况下
	accqq = info->wind;
	accqq += GetFriction(accqq, velqq); // 此时加速度
	velqq += accqq;						// 此时速度
	if (info->ts_left == GetTypeEnd)
	{
		if (info->pos - OnlyWindPos >= 0) // 敌人类型判断
		{
			EnemyType = 0; // 摆烂类
		}
		else
		{
			EnemyType = 1; // 主动型
		}
	}
}

/// @brief 获取敌人上一次加速度，以此预测未来加速度
/// @param My_acc 我的当前加速度
/// @param vel 当前速度
/// @return 敌人上一次加速度
static double GetEnemyLastAcc(double My_acc, double vel, double wind)
{
	static double Enemy_last_acc, last_vel, My_last_acc = 1000, last_wind;
	if (My_last_acc != 1000) // 类似continue，跳过当前计算（现在没有"last_"前缀的量），目的是获得"last_"前缀的量
		Enemy_last_acc = My_last_acc + last_vel - vel + last_wind;
		Enemy_last_acc+=GetFriction(Enemy_last_acc,last_vel);

	My_last_acc = My_acc;
	last_vel = vel;
	last_wind = wind;
	return Enemy_last_acc;
}

/*
##逻辑架构##

当100到GetTypeEnd：判断敌人类型

当GetTypeEnd到4时
{
对于敌人类型：
	进攻型：当对方力气大到可以拉飞自己(游戏刻较大的时候就是拉着玩，快结束就认真了)
	摆烂类：我也摆
}

当4到2：让对方拉个够，拉回去

当1：
{
旗帜在对面：必胜
旗帜在我方：玄乎
		对于摆烂类的最后大力型：施加更大力
		对于摆烂类的绝对摆烂型：无解
}




*/

/// @brief a
/// @param info
void player19(struct war_info *info)
{

	info->player_name = "耐磨王";
	static double Enemy_acc, my_acc;

	Enemy_acc = GetEnemyLastAcc(my_acc, info->vel, info->wind);

	if (info->ts_left >= GetTypeEnd && info->ts_left <= 100) // 获得敌人类型
	{
		GetEnemyType(info);
		return;
	}
	else if (YOU_DANGER(Enemy_acc, info->zones[2] + 20) && info->ts_left > 2 && EnemyType == 1) // 针对进攻型 他危险时松手
	{
		info->player_acc = 0; // 松手
		my_acc = info->player_acc;
		return;
	}
	else if (info->vel < 0 && info->pos <= 50 && EnemyType == 1 && info->ts_left >= 4)
	{ // 针对进攻型
		// 当旗帜在右侧

		for (my_acc = 0; I_SAFE(my_acc, info->zones[2] - 25); my_acc += 0.01) // 促使对手增大拉力减25是为了防止风吹飞我
			;

		info->player_acc = my_acc;
		return;
	}
	else if (info->ts_left == 2) // 针对所有
	{
		// 将旗子放在对方那边则必胜，在最后一刻他需要面临是否输出大拉力的困境（输出大拉力，意味着要拉出去|不输出力或力大小不能使自己出界，意味着要被我方拉回）
		if (EnemyType == 1)
		{
			info->player_acc = (info->zones[2] - info->pos) - info->wind - 2 - info->vel;
		}
		else if (EnemyType == 0)
		{
			info->player_acc = 40 - info->wind - 2 - info->vel; // 不会有人从头到尾都不发力吧。
		}
		my_acc = info->player_acc;
		return;
		// 用距离算出需要的加速度，减去向自身速度和风力，神秘数字2（去除正好向左的摩擦）恰好能让自己不出界面
	}
}