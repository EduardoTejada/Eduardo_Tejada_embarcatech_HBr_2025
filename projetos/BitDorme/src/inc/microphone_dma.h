void microphone_init();
int microphone_read();
float sample_mic();
uint8_t get_intensity(float v);
float convert_to_db(float voltage_rms);