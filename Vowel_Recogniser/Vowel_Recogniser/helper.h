vector<double> get_audio_from_file(string filePath);

vector<int> preprocess_audio(vector<double> &audio);

int get_vowel(string fileName);

vector<vector<long double>> load_cepstral_coefficients(string filePath);