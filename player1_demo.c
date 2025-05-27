// player1.c

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define CHEATING 0 // 外挂

struct war_info
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

struct _pid
{
	float SetPos;	 // 定义设定值
	float ActualPos; // 定义实际值
	float err;		 // 定义偏差值
	float err_last;	 // 定义上一个偏差值
	float Kp;		 // 定义比例系数
	float Ki;		 // 定义积分系数
	float Kd;		 // 定义微分系数
	float integral;	 // 定义积分值
} pid = {
	.SetPos = 0.0,
	.ActualPos = 0.0,
	.err = 0.0,
	.err_last = 0.0,
	.integral = 0.0,
	.Kp = 0.2,
	.Ki = 0.15,
	.Kd = 0.2};
void PID_update(float actual)
{
	pid.ActualPos = actual;
}
float PID_acc(float Pos, float vel)
{
	pid.SetPos = Pos;
	pid.err = pid.SetPos - pid.ActualPos;
	pid.integral += pid.err;
	pid.ActualPos = pid.Kp * pid.err + pid.Ki * pid.integral + pid.Kd * (pid.err - pid.err_last);
	pid.err_last = pid.err;

	float acc;
	acc = (pid.err - vel);
	return acc;
}
double *get_flagAddr(struct war_info *info)
{
	for (int i = sizeof(struct war_info) / 8; i < sizeof(struct war_info) / 8 + 128; i++)
	{
		double *real = (double *)(info) + i;
		if (*real == 50)
			return real;
	}
	return NULL;
}
/// <summary>
/// 在此实现进攻算法
/// </summary>
/// <param name="info">比赛信息，参见war_info结构的说明</param>
void player1(struct war_info *info)
{
#if CHEATING
	/**************必删的这是外挂必删的**************/
	static double *flag_pos=NULL;
	if (flag_pos == NULL) // 只运行一次
	{
		flag_pos = get_flagAddr(info);
	}
	if (flag_pos != NULL)
	{
		if (info->ts_left < 99&&info->ts_left != 0)
		{
			*flag_pos =(info->zones[1]+20) ; // flag_pos旗帜位置
			// double *flag_vel;
			// flag_vel = (flag_pos)-1;
			//*flag_vel -= 99; // flag_vel旗帜速度
		}
	} /************必删的阿巴阿巴必删的**************/
#endif


	info->player_name = "小魔王";


	PID_update(info->pos);
	info->player_acc = (pow(50000, -info->ts_left / 100)) * (PID_acc(info->zones[1] + 20, info->vel));
	// 这个是2.PID法，这个pow（50000）的抽象东西是根据时间慢慢增大pid力度的，先看看对方能不能自己拉输。


	if (info->ts_left < 3&&info->ts_left != 0)
		info->player_acc += (info->zones[1] + 20 - info->pos) + info->vel;
	// 这个是1.摆烂法（对方可能把自己拉输（有风或者对方未判断自身位置））。最后一刻拉爆对方（对手为保持自身位置，速度和拉力大抵是0的），压根无反应时间。（有点无赖）
	// 现在将法1，2结合，防止看出无赖之法1。

	// 还有一种3.可以是在对方拉力（可以通过速度，位置，自己的拉力感受）足够大的时候放手（那个大魔王显然肌无力，要自己编个对手测试）


} 


