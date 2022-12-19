#include <iostream>
#include <iomanip>
#include <math.h>
#include <fstream>

using namespace std;

float func_eps(float num);

#define PI 3.14159265
enum state { up, fly, down };

int main() {
	std::ofstream out("Protocol.txt", std::ios::app);
	out.setf(ios::left);

	cout.setf(ios::left);
	int step = 0;
	int pred_step = step;
	/*int cos_sin_znamenat = 302;*/
	float ugol = 0;

	float mr = 287 * 1000 + 4800;
	float J = 4 * 971 * 1000 + 912 * 1000;
	float C_f = 0.47;
	float S = 10;
	float v_rash = 1629.679144385;

	float R = 8.314;
	float M = 0.0292;
	float Tem = 285.65;
	float ro_0 = 1.225;
	float ro = 1.225;
	float g = 9.8;
	float R_z = 6.371;	//без 10^6
	float M_z = 5.972;	//без 10^24
	float G = 6.67;		//без 10^-11

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
	int rocket_stage_th_time = 574;
	bool TDU = true;
	float sinus = 0;
	float cosinus = 0;

	float max_speed_x = 0;
	float max_speed_y = 0;
	float max_h = 0;

	for (t; t < 8000; t += 0.001) {
		float k = 55;
		mr -= v_rash * 0.001;
		if (h < 0 || h > 400000)break;

		if (now == state::up) {

			if (h < 100000) {
				ugol = (k / 100000) * h * (PI / 180);
			}
			else {
				ugol = (90 + 0.01845 * k) * (PI / 180);
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
				v_x += 324;
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

		if (now == state::down) {
			a_y = -(g - pow(v_x, 2) / (R_z * pow(10, 6) + h));
			if (TDU && (t - down_time < 40)) {
				a_x = (-J) / mr;
			}
			else if (TDU && (t - down_time) >= 40) {
				TDU = false;
				a_x = 0;
				J = 0;
				v_rash = 0;
			}
			if (h <= 100000) {
				a_x = (-((C_f * ro * pow(v_x, 2) * S) / 2) / mr);
				a_y = (-mr * (g - pow(v_x, 2) / (R_z * pow(10, 6) + h)) + (C_f * ro * pow(v_y, 2) * S) / 2) / mr;
			}

			if (h <= 7000) {
				break;
			}
		}

		v_x += a_x * 0.001;
		v_y += a_y * 0.001;
		max_speed_x = max_speed_x < v_x ? v_x : max_speed_x;
		max_speed_y = max_speed_y < v_y ? v_y : max_speed_y;
		max_h = max_h < h ? h : max_h;

		if (abs(v_x * 0.001) < L_eps) {
			L_cash += v_x * 0.001;
			if (L_cash > L_eps) {
				L += L_cash;
				L_cash = 0;
				L_eps = func_eps(L);
			}
		}
		else {
			L += v_x * 0.001;
			if (L_cash > L_eps) {
				L += L_cash;
				L_cash = 0;
				L_eps = func_eps(L);
			}
		}

		if (abs(v_y * 0.001) < h_eps) {
			h_cash += v_y * 0.001;
			if (h_cash > h_eps) {
				h += h_cash;
				h_cash = 0;
				h_eps = func_eps(h);
			}
		}
		else {
			h += v_y * 0.001; 
			if (h_cash > h_eps) {
				h += h_cash;
				h_cash = 0;
				h_eps = func_eps(h);
			}
		}

		g = (G * pow(10, -11) * M_z * pow(10, 24)) / (pow((R_z * pow(10, 6) + h), 2));
		ro = ro_0 * exp(-((M * g * h) / (R * Tem)));

		if ((step - pred_step) > 99) {

			cout << setw(3) << "t:" << setw(6) << round(t);
			cout << setw(2) << "h" << setw(10) << h;
			cout << setw(2) << "L" << setw(12) << L;
			//cout << setw(5) << "g - v**2/(R + h) " << setw(10) << g - pow(v_x, 2) / (R_z * pow(10, 6) + h);
			//cout << setw(2) << "g" << setw(12) << g;
			//cout << setw(3) << "ro" << setw(12) << ro;
			cout << setw(4) << "a_y" << setw(11) << a_y;
			cout << setw(4) << "v_y" << setw(10) << v_y;
			cout << setw(4) << "a_x" << setw(10) << a_x;
			cout << setw(4) << "v_x" << setw(10) << v_x;
			cout << setw(3) << "mr" << setw(10) << mr;
			cout << endl;

			out << setw(3) << "t:" << setw(6) << round(t);
			out << setw(2) << "h" << setw(10) << h;
			out << setw(2) << "L" << setw(12) << L;
			//out << setw(5) << "g - v**2/(R + h) " << setw(10) << g - pow(v_x, 2) / (R_z * pow(10, 6) + h);
			//out << setw(2) << "g" << setw(12) << g;
			//out << setw(3) << "ro" << setw(12) << ro;
			out << setw(4) << "a_y" << setw(11) << a_y;
			out << setw(4) << "v_y" << setw(10) << v_y;
			out << setw(4) << "a_x" << setw(10) << a_x;
			out << setw(4) << "v_x" << setw(10) << v_x;
			out << setw(3) << "mr" << setw(10) << mr;
			out << endl;
			pred_step = step;
		}
		step = round(t);
	}
	cout << "\n\tEND\n" << endl;
	cout << setw(29) << "first stage separation time" << setw(4) << rocket_stage_fr_time;
	cout << setw(30) << "second stage separation time" << setw(4) << rocket_stage_sc_time;
	cout << setw(29) << "third stage separation time" << setw(4) << rocket_stage_th_time << endl;
	cout << setw(19) << "TDU ignition time" << setw(6) << down_time;
	cout << setw(13) << "max_speed_x" << setw(8) << max_speed_x;
	cout << setw(13) << "max_speed_y" << setw(8) << max_speed_y;
	cout << setw(8) << "max_h" << setw(8) << max_h;
	cout << setw(2) << "L" << setw(12) << L;
	cout << endl;
	out.close();
	return 0;
}

float func_eps(float num) {
	num = abs(num);
	float eps = 1;
	while ((num + eps) != num) eps /= 2;
	return eps;
}