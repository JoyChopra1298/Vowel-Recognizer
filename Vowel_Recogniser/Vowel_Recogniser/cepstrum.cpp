#include "stdafx.h"
#include "cepstrum.h"


vector<long double> calculate_Ri(const vector<long double> &filtered_audio){
	vector<long double> R(1 + number_of_coefficients, 0.0);
	for (int i = 0; i <= number_of_coefficients; i++){
		for (int j = 0; j < frame_size - i; j++){
			R[i] += (filtered_audio[j]) * (filtered_audio[j + i]);
		}
	}
	return R;
}

vector<long double> calculate_ai(const vector<long double> &R){
	vector<long double> E(1 + number_of_coefficients, 0.0);
	E[0] = R[0];
	vector<vector <long double>> a(number_of_coefficients + 1);
	for (int i = 0; i <= number_of_coefficients; i++){
		for (int j = 0; j <= number_of_coefficients; j++){
			a[i].push_back(0.0);
		}
	}
	for (int i = 1; i <= number_of_coefficients; i++){
		long double k_i = R[i];
		for (int j = 1; j <= i - 1; j++){
			k_i -= a[i - 1][j] * R[i - j];
		}
		k_i /= E[i - 1];

		a[i][i] = k_i;
		for (int j = 1; j <= i - 1; j++){
			a[i][j] = a[i - 1][j] - k_i * a[i - 1][i - j];
		}
		E[i] = (1 - (k_i * k_i))*E[i - 1];
	}
	return a[number_of_coefficients];
}

vector<long double> calculate_ci(const vector<long double> &a, long double gain){
	vector<long double> c(1 + number_of_coefficients, 0.0);
	c[0] = log2(gain*gain);
	for (int m = 1; m <= number_of_coefficients; m++){
		c[m] = a[m];
		for (int k = 1; k <= m - 1; k++){
			c[m] += ((c[k] * a[m - k]) *k) / m;
		}
	}
	return c;
}