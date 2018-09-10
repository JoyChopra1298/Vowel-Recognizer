#include "stdafx.h"
#include "helper.h"

vector<double> get_audio_from_file(string filePath){
	/*
	Input - absolute path of the audio file
	Output - vector of audio samples extracted from the file
	*/

	//Open audio file
	fstream audio_file(filePath, ios::in);

	//Read file to get audio data
	string ignore;
	audio_file >> ignore;
	long num_samples;
	audio_file >> num_samples;
	int ignore_count = 5;
	for (int i = 0; i < ignore_count; i++){
		getline(audio_file, ignore);
	}
	vector<double> audio(num_samples);
	double temp;
	for (int i = 0; i < num_samples; i++){
		audio_file >> temp;
		audio[i] = temp;
	}

	//Close file since not needed anymore
	audio_file.close();

	return audio;
}

vector<int> preprocess_audio(vector<double> &audio){
	// Apply DC shift correction and amplitude normalisation
	int i;
	int num_samples = audio.size();
	//DC shift correction. Pre-determined using silent recordings
	for (i = 0; i < num_samples; i++){
		audio[i] -= dc_shift_correction;
	}

	// normalise signal by scaling it
	double max_amplitude_signal = 0.0;
	for (i = 0; i < num_samples; i++){
		if (max_amplitude_signal < audio[i]){
			max_amplitude_signal = audio[i];
		}
	}
	double normalisation_factor = max_amplitude_normalised / max_amplitude_signal;
	for (i = 0; i < num_samples; i++){
		audio[i] *= 1;
	}

	// trim silence intervals from front and end by finding markers
	int start_marker = 0;
	for (i = 0; i < num_samples; i += frame_size){
		double total_energy_of_frame = 0.0;
		for (int j = 0; j < frame_size; j++){
			total_energy_of_frame += audio[i + j] * audio[i + j];
		}
		if (total_energy_of_frame / frame_size > silence_energy_threshold){
			start_marker = i;
			break;
		}
	}
	int end_marker = num_samples - 1;
	for (i = num_samples - 1; i >= 0; i -= frame_size){
		double total_energy_of_frame = 0.0;
		for (int j = 0; j < frame_size; j++){
			total_energy_of_frame += audio[i - j] * audio[i - j];
		}
		if (total_energy_of_frame / frame_size > silence_energy_threshold){
			end_marker = i;
			break;
		}
	}
	vector<int> markers(2, 0);
	markers[0] = start_marker;
	markers[1] = end_marker;
	return markers;
}

int get_vowel(string fileName){
	//Extract vowel from the name of file. Assumed files stored as <roll_num>_<vowel>_<iteration>.txt
	int vowel_index = fileName.find_first_of('_') + 1;
	char vowel = (fileName[vowel_index]);
	switch (vowel){
		case 'a':{
			 return 0;
			 break;
		}
		case 'e':{
			return 1;
			break;
		}
		case 'i':{
			return 2;
			break;
		}
		case 'o':{
			return 3;
			break;
		}
		case 'u':{
			return 4;
			break;
		}
	}
}

vector<vector<long double>> load_cepstral_coefficients(string filePath){
	vector<vector<long double>> c_frames(num_frames_to_analyse);

	// open file
	fstream cepstrum_file(filePath, ios::in);

	//ignore first line
	cepstrum_file.ignore(10000, '\n');

	vector<long double> c(1 + number_of_coefficients,0.0);
	for (int l = 0; l < num_frames_to_analyse; l++){
		for (int i = 0; i < number_of_coefficients; i++){
			cepstrum_file >> c[i + 1];
		}
		c_frames[l] = c;
	}
	return c_frames;
}