// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Windows.h>
#include <iomanip>

// Global constants
#define frame_size 320
#define frame_skip (frame_size / 4) // 75% overlap between frames
#define number_of_coefficients 12   // p
#define max_amplitude_normalised 5000.0
#define silence_energy_threshold  (max_amplitude_normalised*max_amplitude_normalised / 2000)
#define ouput_width 13
#define dc_shift_correction -0.0018
#define output_float_precision 4
#define num_frames_to_analyse 10
using namespace std;

const vector<int> tokhura_weights = { 1, 3, 7, 13, 19, 22, 25, 33, 42, 50, 56, 61 };
const vector<char> vowels = { 'a', 'e', 'i', 'o', 'u' };