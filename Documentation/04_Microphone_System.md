# Microphone System - MSM261S4030H0

## Hardware Specifications

### Microphone: MSM261S4030H0

- **Type**: MEMS Digital Microphone
- **Interface**: I2S (Pulse Density Modulation)
- **Sensitivity**: -26 dBFS
- **SNR**: 64 dB
- **Frequency Response**: 50Hz to 20kHz
- **Power**: Low power consumption
- **Package**: Surface mount

### Audio Specifications

- **Sample Rates**: 8kHz to 48kHz
- **Bit Depth**: 16-bit/24-bit
- **Channels**: Mono (single microphone)
- **Dynamic Range**: 64 dB
- **Maximum SPL**: 120 dB

## Pin Configuration

### I2S Microphone Input

```cpp
#define I2S_PIN_BCK   15        // I2S Bit Clock
#define I2S_PIN_WS    2         // I2S Word Select (Left/Right Clock)
#define I2S_PIN_DOUT  -1        // Not used for input
#define I2S_PIN_DIN   39        // I2S Data Input
```

**Note**: Microphone uses separate I2S pins from audio output to avoid conflicts.

### I2S Configuration

```cpp
// I2S configuration for microphone input
i2s_config_t i2s_config = {
    .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM,
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
};
```

## Library Integration

### ESP_I2S Library

The system uses ESP_I2S library for microphone input:

```cpp
#include "ESP_I2S.h"
#include "ESP_SR.h"  // Speech recognition (optional)

// I2S object for microphone
I2SClass I2S;
```

### ESP_SR Integration

For speech recognition capabilities:

```cpp
#include "esp_task_wdt.h"

// Speech recognition can be integrated
// Requires additional configuration
```

## Initialization

### Microphone System Setup

```cpp
void MIC_Init(void) {
    Serial.println("Initializing microphone...");
    
    // Configure I2S for microphone input
    I2S.setAllPins(I2S_PIN_BCK, I2S_PIN_WS, I2S_PIN_DOUT, I2S_PIN_DIN);
    
    // Start I2S with microphone configuration
    if (!I2S.begin(I2S_MODE_PDM, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO)) {
        Serial.println("❌ Failed to initialize I2S for microphone");
        return;
    }
    
    Serial.println("✓ Microphone initialized successfully");
}
```

### Advanced I2S Configuration

```cpp
void MIC_Init_Advanced(void) {
    // More detailed I2S setup
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_PIN_BCK,
        .ws_io_num = I2S_PIN_WS,
        .data_out_num = I2S_PIN_DOUT,
        .data_in_num = I2S_PIN_DIN
    };
    
    // Install and start I2S driver
    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("Failed to install I2S driver: %s\n", esp_err_to_name(err));
        return;
    }
    
    err = i2s_set_pin(I2S_NUM_0, &pin_config);
    if (err != ESP_OK) {
        Serial.printf("Failed to set I2S pins: %s\n", esp_err_to_name(err));
        return;
    }
    
    Serial.println("✓ Advanced microphone setup complete");
}
```

## Audio Recording Functions

### Basic Audio Capture

```cpp
void record_audio_sample() {
    const int SAMPLE_SIZE = 1024;
    int16_t samples[SAMPLE_SIZE];
    size_t bytes_read = 0;
    
    // Read audio data from I2S
    esp_err_t result = i2s_read(I2S_NUM_0, samples, sizeof(samples), 
                               &bytes_read, portMAX_DELAY);
    
    if (result == ESP_OK && bytes_read > 0) {
        int samples_read = bytes_read / sizeof(int16_t);
        
        // Process audio samples
        Serial.printf("Read %d samples\n", samples_read);
        
        // Calculate average amplitude
        long sum = 0;
        for (int i = 0; i < samples_read; i++) {
            sum += abs(samples[i]);
        }
        int average = sum / samples_read;
        
        Serial.printf("Average amplitude: %d\n", average);
    }
}
```

### Continuous Recording

```cpp
static bool recording = false;
static File audioFile;

void start_recording(const char* filename) {
    if (recording) {
        Serial.println("Already recording");
        return;
    }
    
    // Open file for writing
    audioFile = SD.open(filename, FILE_WRITE);
    if (!audioFile) {
        Serial.printf("Failed to open file: %s\n", filename);
        return;
    }
    
    // Write WAV header (simplified)
    write_wav_header(audioFile, 0);  // Size will be updated later
    
    recording = true;
    Serial.printf("Started recording to: %s\n", filename);
}

void stop_recording() {
    if (!recording) {
        Serial.println("Not recording");
        return;
    }
    
    recording = false;
    
    // Update WAV header with actual file size
    unsigned long fileSize = audioFile.size();
    audioFile.seek(0);
    write_wav_header(audioFile, fileSize - 44);  // Exclude header size
    
    audioFile.close();
    Serial.println("Recording stopped");
}

void recording_loop() {
    if (!recording) return;
    
    const int BUFFER_SIZE = 512;
    int16_t buffer[BUFFER_SIZE];
    size_t bytes_read = 0;
    
    esp_err_t result = i2s_read(I2S_NUM_0, buffer, sizeof(buffer), 
                               &bytes_read, 100);  // 100ms timeout
    
    if (result == ESP_OK && bytes_read > 0) {
        // Write audio data to SD card
        audioFile.write((uint8_t*)buffer, bytes_read);
        
        // Flush periodically to ensure data is written
        static int flush_counter = 0;
        if (++flush_counter >= 10) {
            audioFile.flush();
            flush_counter = 0;
        }
    }
}
```

### WAV File Format Support

```cpp
void write_wav_header(File &file, unsigned long dataSize) {
    // WAV file header
    file.write((uint8_t*)"RIFF", 4);
    unsigned long fileSize = dataSize + 36;
    file.write((uint8_t*)&fileSize, 4);
    file.write((uint8_t*)"WAVE", 4);
    
    // Format chunk
    file.write((uint8_t*)"fmt ", 4);
    unsigned long fmtSize = 16;
    file.write((uint8_t*)&fmtSize, 4);
    unsigned short audioFormat = 1;  // PCM
    file.write((uint8_t*)&audioFormat, 2);
    unsigned short numChannels = 1;  // Mono
    file.write((uint8_t*)&numChannels, 2);
    unsigned long sampleRate = 16000;
    file.write((uint8_t*)&sampleRate, 4);
    unsigned long byteRate = sampleRate * numChannels * 2;
    file.write((uint8_t*)&byteRate, 4);
    unsigned short blockAlign = numChannels * 2;
    file.write((uint8_t*)&blockAlign, 2);
    unsigned short bitsPerSample = 16;
    file.write((uint8_t*)&bitsPerSample, 2);
    
    // Data chunk
    file.write((uint8_t*)"data", 4);
    file.write((uint8_t*)&dataSize, 4);
}
```

## Audio Processing

### Volume Level Detection

```cpp
int get_audio_level() {
    const int SAMPLE_SIZE = 256;
    int16_t samples[SAMPLE_SIZE];
    size_t bytes_read = 0;
    
    esp_err_t result = i2s_read(I2S_NUM_0, samples, sizeof(samples), 
                               &bytes_read, 100);
    
    if (result != ESP_OK || bytes_read == 0) {
        return 0;
    }
    
    // Calculate RMS level
    long sum_squares = 0;
    int samples_read = bytes_read / sizeof(int16_t);
    
    for (int i = 0; i < samples_read; i++) {
        sum_squares += (long)samples[i] * samples[i];
    }
    
    // Return RMS as percentage (0-100)
    double rms = sqrt((double)sum_squares / samples_read);
    return (int)(rms / 327.67);  // Scale to 0-100
}
```

### Simple Voice Activity Detection

```cpp
bool detect_voice_activity() {
    int level = get_audio_level();
    const int VOICE_THRESHOLD = 15;  // Adjust based on environment
    
    static int consecutive_detections = 0;
    const int MIN_DETECTIONS = 3;
    
    if (level > VOICE_THRESHOLD) {
        consecutive_detections++;
        if (consecutive_detections >= MIN_DETECTIONS) {
            return true;
        }
    } else {
        consecutive_detections = 0;
    }
    
    return false;
}
```

### Audio Visualization

```cpp
void display_audio_meter() {
    int level = get_audio_level();
    
    // Create simple text-based meter
    Serial.print("Audio: [");
    int bars = level / 5;  // 0-20 bars
    for (int i = 0; i < 20; i++) {
        if (i < bars) {
            Serial.print("#");
        } else {
            Serial.print("-");
        }
    }
    Serial.printf("] %d%%\n", level);
}
```

## Speech Recognition Integration

### Basic Speech Commands

```cpp
// This would require ESP_SR library configuration
void init_speech_recognition() {
    // Initialize speech recognition engine
    // Configure wake word detection
    // Set up command recognition
    
    Serial.println("Speech recognition initialized");
    Serial.println("Say 'Hello' to test...");
}

void process_speech_commands() {
    // Process incoming audio for speech recognition
    // This would integrate with ESP_SR library
    
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 100) {  // Check every 100ms
        lastCheck = millis();
        
        if (detect_voice_activity()) {
            Serial.println("Voice detected - processing...");
            // Process speech recognition here
        }
    }
}
```

## Troubleshooting

### Common Issues

1. **No Audio Input**
   - Check I2S pin connections (15, 2, 39)
   - Verify microphone power supply
   - Test with oscilloscope on data line
   - Check I2S clock signals

2. **Noisy Audio**
   - Check ground connections
   - Verify power supply stability
   - Add filtering capacitors
   - Check for electromagnetic interference

3. **Low Audio Levels**
   - Verify microphone sensitivity settings
   - Check gain configuration
   - Test in quieter environment
   - Calibrate threshold values

4. **I2S Conflicts**
   - Ensure separate I2S pins from audio output
   - Check for pin conflicts with other peripherals
   - Verify I2S port assignment (I2S_NUM_0 vs I2S_NUM_1)

### Debug Functions

```cpp
void microphone_system_test() {
    Serial.println("=== Microphone System Test ===");
    
    // Test I2S initialization
    MIC_Init();
    delay(1000);
    
    // Test audio level detection
    Serial.println("Testing audio levels (speak into microphone):");
    for (int i = 0; i < 50; i++) {
        display_audio_meter();
        delay(200);
    }
    
    // Test voice activity detection
    Serial.println("Testing voice activity detection:");
    for (int i = 0; i < 100; i++) {
        if (detect_voice_activity()) {
            Serial.println("🎤 Voice activity detected!");
        }
        delay(100);
    }
    
    Serial.println("Microphone test complete");
}

void i2s_status_check() {
    // Check I2S driver status
    Serial.println("I2S Status:");
    
    // Read some samples to test
    const int TEST_SIZE = 64;
    int16_t test_samples[TEST_SIZE];
    size_t bytes_read = 0;
    
    esp_err_t result = i2s_read(I2S_NUM_0, test_samples, sizeof(test_samples), 
                               &bytes_read, 1000);
    
    Serial.printf("I2S Read Result: %s\n", esp_err_to_name(result));
    Serial.printf("Bytes Read: %d\n", bytes_read);
    
    if (bytes_read > 0) {
        Serial.println("✓ I2S is receiving data");
        
        // Show first few samples
        Serial.print("Sample data: ");
        for (int i = 0; i < min(8, (int)(bytes_read/2)); i++) {
            Serial.printf("%d ", test_samples[i]);
        }
        Serial.println();
    } else {
        Serial.println("❌ No I2S data received");
    }
}
```

## Example Implementation

### Simple Voice Recorder

```cpp
void setup() {
    Serial.begin(115200);
    
    // Initialize SD card
    SD_Init();
    
    // Initialize microphone
    MIC_Init();
    
    Serial.println("Voice Recorder Ready");
    Serial.println("Commands:");
    Serial.println("  's' - Start recording");
    Serial.println("  'e' - Stop recording");
    Serial.println("  'l' - Show audio levels");
}

void loop() {
    // Handle serial commands
    if (Serial.available()) {
        char cmd = Serial.read();
        
        switch (cmd) {
            case 's':
                start_recording("/recording.wav");
                break;
            case 'e':
                stop_recording();
                break;
            case 'l':
                display_audio_meter();
                break;
        }
    }
    
    // Continue recording if active
    recording_loop();
    
    delay(10);
}
```

### Voice-Activated System

```cpp
void setup() {
    Serial.begin(115200);
    
    // Initialize systems
    SD_Init();
    MIC_Init();
    
    Serial.println("Voice-Activated System Ready");
    Serial.println("Listening for voice commands...");
}

void loop() {
    // Check for voice activity
    if (detect_voice_activity()) {
        Serial.println("🎤 Voice detected - starting recording");
        
        // Record voice command
        start_recording("/command.wav");
        
        // Record for 3 seconds
        unsigned long start_time = millis();
        while (millis() - start_time < 3000) {
            recording_loop();
            delay(10);
        }
        
        stop_recording();
        Serial.println("Command recorded - processing...");
        
        // Here you would process the recorded command
        // For now, just play it back (if audio output is available)
        
        delay(1000);  // Pause before listening again
    }
    
    delay(50);
}
```

*Last Updated: October 2, 2025*