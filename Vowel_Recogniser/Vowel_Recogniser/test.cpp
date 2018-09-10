#include "stdafx.h"
#include "test.h"

long double tokhura_distance(const vector<long double> &c1, const vector<long double> &c2){
	long double distance = 0;
	for (int i = 1; i <= number_of_coefficients; i++){
		distance += tokhura_weights[i-1] * (c1[i] - c2[i]) * (c1[i] - c2[i]);
	}
	return distance;
}


char test(string trained_files_dir, string test_file_path){

	////////Get cepstral coefficients for test file

	// Get audio data from test file
	vector<double> audio = get_audio_from_file(test_file_path);
	int num_samples = audio.size();

	// Apply DC shift correction, normalise amplitude and trim silence.
	vector<int> markers = preprocess_audio(audio);
	int start_marker = markers[0];
	int end_marker = markers[1];

	// take 10 steady state frames from the signal (middle portion of the trimmed signal)
	int steady_frame_start = start_marker + ((end_marker - start_marker) - (frame_size + (num_frames_to_analyse - 1) * frame_skip)) / 2;

	vector<vector<long double>> c_test(num_frames_to_analyse);

	for (int l = 0; l < num_frames_to_analyse; l++){
		//apply Hamming's window to the frame
		vector<long double> filtered_audio(frame_size, 0.0);
		long double sum = 0.0;
		for (int j = 0; j < frame_size; j++){
			double w_j = 0.54 - 0.46 * (cos(2 * (3.14159)*j / (frame_size - 1)));
			filtered_audio[j] = w_j * audio[j + steady_frame_start + (l *frame_skip)];
		}
		// calculate R_i
		vector<long double> R = calculate_Ri(filtered_audio);
		// calculate a_i
		vector<long double> a = calculate_ai(R);
		//calculate c_i
		vector<long double> c = calculate_ci(a, R[0]);
		c_test[l] = c;
	}
	
	// initialise vectors for storing count of files and cumulative distances for each vowel
	vector<int> file_count(5, 0);
	vector<long double> vowel_accumulated_distance(5, 0.0);

	// Traverse over all files in the directory
	std::string pattern(trained_files_dir);
	pattern.append("\\*.txt");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			// generate absolute path for file 
			string fileName = data.cFileName;
			string filePath = trained_files_dir + string("\\") + fileName;
			
			// extract vowel from the filename
			int train_vowel = get_vowel(fileName);

			// get cepstral coefficients from the file
			vector<vector<long double>> c_trained = load_cepstral_coefficients(filePath);

			// calculate tokhura's distance between the frames
			long double distance = 0.0;
			for (int i = 0; i < num_frames_to_analyse; i++){
				distance += tokhura_distance(c_trained[i], c_test[i]);
			}
			long double average_distance = distance / num_frames_to_analyse;

			// accumulate this distance for corresponding vowel
			vowel_accumulated_distance[train_vowel] += average_distance;
			file_count[train_vowel] += 1;

		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
	// calculate average distance for each vowel and find minimum
	int predicted_vowel = 0;
	long double minimum_distance = 10000000;
	cout << endl;
	for (int i = 0; i < 5; i++){
		long double average_distance = vowel_accumulated_distance[i] / file_count[i];
		cout << "Distance for vowel " << vowels[i] << " is - " << average_distance << endl;
		if (average_distance < minimum_distance){
			minimum_distance = average_distance;
			predicted_vowel = i;
		}
	}
	return vowels[predicted_vowel];
}