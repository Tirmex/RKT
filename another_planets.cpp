#include <iostream>
#include <iomanip>
#include <math.h>


using namespace std;

float func_eps(float num);

#define PI 3.14159265
enum state { up, fly, down };

int main() {
	cout.setf(ios::left);
	int step = 0;
	int pred_step = step;
	float ugol = 0;

	float mr = 287 * 1000 + 4800;
	float J = 4 * 971 * 1000 + 912 * 1000;
	float C_f = 0.47;
	float S = 10;
	float v_rash = 1629.679144385;

	cout << "Программа ПРИМЕРНО симулирует полёт ракеты-носителя Восток-1 в условиях описанной планеты.\n";
	cout << "Пожалуйста, введите некоторые данные.\n";
	float R = 8.314;
	cout << "Введите молярную массу газа на вашей планете, кг/моль: ";
	float M = 0.0292; cin >> M;
	cout << "Температуру на поверхности, Кельвин: ";
	float Tem = 285.65; cin >> Tem;
	cout << "Плотность атмосферы, кг/м^3: ";
	float ro_0 = 1.225; cin >> ro_0;
	float ro = 1.225;
	float g = 9.8;
	cout << "Радиус планеты, млн метров: ";
	float R_z = 6.371; cin >> R_z;
	cout << "Массу планеты, 10^24 кг: ";
	float M_z = 5.972; cin >> M_z;
	float G = 6.67;
	cout << "Угол, к которому ракета наклонится к первой ступени: ";
	float k = 55; cin >> k;
	cout << "До какой секунды симулировать полёт? Не менее 730 секунд. ";
	float tau; cin >> tau;
	float a_x = 0;
	float v_x = 0;
	float a_y = 0;
	float v_y = 0;
	float h = 0;
	float h_eps = 0;
	float h_cash = 0;
	float L = 0;
	float L_eps = 0;
	float L_cash = 0;

	state now = state::up;
	float t = 0;
	float down_time = 0;
	bool rocket_stage_fr = true;
	bool obtekatel = true;
	bool rocket_stage_sc = true;
	bool rocket_stage_th = true;
	int rocket_stage_fr_time = 119;//Время отделения
	int rocket_stage_sc_time = 308;
	int rocket_stage_th_time = 730;
	bool TDU = true;
	float sinus = 0;
	float cosinus = 0;
	for (; t < tau; t += 0.001) {


		mr -= v_rash * 0.001;
		if (h < 0)break;

		if (now == state::up) {

			if (h < 100000) {
				ugol = (k / 100000) * h * (PI / 180);
			}

			if (rocket_stage_fr && (round(t) > rocket_stage_fr_time)) {
				rocket_stage_fr = false;
				mr = 100400 + 12490 - 119 * 366.525974026;
				v_rash = 301.948051948;
				J = 912 * 1000;
				S = 5.64;
			}
			else if (obtekatel && (round(t) > 155)) {
				obtekatel = false;
				mr -= 650;
			}
			else if (rocket_stage_sc && (t > rocket_stage_sc_time)) {
				rocket_stage_sc = false;
				mr = 12490;
				v_rash = 15.6492891;
				J = 54.5 * 1000;
			}
			else if (rocket_stage_th && (t > rocket_stage_th_time)) {
				rocket_stage_th = false;
				mr = 4800;
				v_rash = 0;
				J = 0;
				v_x += 324 / R_z * 6300000;
			}

			if (h < 100000) {
				a_x = (J * sin(ugol) - (C_f * ro * pow(v_x, 2) * S) / 2) / mr;
				a_y = (J * cos(ugol) - (C_f * ro * pow(v_y, 2) * S) / 2 - mr * (g - pow(v_x, 2) / (R_z * pow(10, 6) + h))) / mr;
			}
			else {
				a_x = (J * sin(ugol)) / mr;
				a_y = (J * cos(ugol) - mr * (g - pow(v_x, 2) / (R_z * pow(10, 6) + h))) / mr;
			}


			if (t >= 730) {
				now = state::fly;
				S = 5.64;
				J = 0;
				v_rash = 0;
			}
		}

		if (now == state::fly) {
			a_y = -(g - (pow(v_x, 2) / (R_z * pow(10, 6) + h)));


			if (L >= 40868600) {
				now = state::down;
				J = 15800;
				v_rash = 35.75;
				S = 3.8;
				C_f = 0.5;
				down_time = round(t);
			}
		}

		v_x += a_x * 0.001;
		v_y += a_y * 0.001;

		if (abs(v_x * 0.001) < L_eps) {
			L_cash += v_x * 0.001;
			if (L_cash > L_eps) {
				L += L_cash;
				L_cash = 0;
				L_eps = func_eps(L);
			}
		}
		else L += v_x * 0.001;

		if (abs(v_y * 0.001) < h_eps) {
			h_cash += v_y * 0.001;
			if (h_cash > h_eps) {
				h += h_cash;
				h_cash = 0;
				h_eps = func_eps(h);
			}
		}
		else h += v_y * 0.001;

		g = (G * pow(10, -11) * M_z * pow(10, 24)) / (pow((R_z * pow(10, 6) + h), 2));
		ro = ro_0 * exp(-((M * g * h) / (R * Tem)));

		if ((step - pred_step) > 9) {

			pred_step = step;
		}
		step = round(t);
	}
	cout << endl;
	cout << setw(3) << "t:" << setw(6) << round(t) << endl;
	if (h < 0) {
		if (round(t) == 0) cout << "Вы не взлетели...";
		else {
			cout << "Ваша космический корабль приземлился на поверхность, либо разбился.\n";
			cout << "Он пролетел примерно " << L << endl;
		}
	}
	else {

		if (v_x * v_x / R_z / pow(10, 6) / sqrt(2) > g)
			cout << "Вы превысили вторую космическую скорость и оторвались от планеты.\n";
		else if (v_x * v_x / R_z / pow(10, 6) > g)
			cout << "Вы превысили первую космическую скорость и обращаетесь вокруг планеты.\n";
		else
			cout << "Вы всё ещё в космосе, но упадете на поверхность планеты в течении конечного времени.\n";
		cout << setw(2) << "Пройденное расстояние: " << setw(12) << L << endl;
		cout << setw(4) << "Ваша скорость: " << setw(10) << v_x << endl;
		cout << endl;
	}
	return 0;
}

float func_eps(float num) {
	num = abs(num);
	float eps = 1;
	while ((num + eps) != num) eps /= 2;
	return eps;
}