# Audio System - PCM5101 DAC

## Hardware Specifications

### Audio DAC: PCM5101

- **Interface**: I2S (Inter-IC Sound)
- **Resolution**: 16-bit/24-bit/32-bit
- **Sample Rates**: 8kHz to 384kHz
- **SNR**: 112dB
- **THD+N**: -93dB
- **Power**: Low power consumption

### Supported Audio Formats

- **PCM**: 16-bit, 22.05kHz/44.1kHz/48kHz
- **MP3**: Up to 320kbps
- **WAV**: Uncompressed PCM
- **AAC**: Basic profile support
- **FLAC**: Lossless compression

## Pin Configuration

### I2S Audio Output

```cpp
#define I2S_DOUT      47        // I2S Data Output
#define I2S_BCLK      48        // I2S Bit Clock
#define I2S_LRC       38        // I2S Left/Right Clock (WS)
```

### I2S Configuration

```cpp
#define EXAMPLE_Audio_TICK_PERIOD_MS  20
#define Volume_MAX                    21
```

## Audio Library Integration

### ESP32-audioI2S Library

The system uses the ESP32-audioI2S library for audio playback:

```cpp
#include "Audio.h"

// Global audio object
Audio audio;
uint8_t Volume = 10;  // Default volume (0-21)
```

## Initialization

### Audio System Setup

```cpp
void Audio_Init() {
    // Initialize I2S pins
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    
    // Set initial volume
    audio.setVolume(Volume);  // 0-21
    
    // Configure audio settings
    audio.setFileLoop(false);           // No loop by default
    audio.setConnectionTimeout(500);    // 500ms timeout
    audio.setAudioTaskPrio(2);         // Audio task priority
    
    Serial.println("✓ Audio system initialized");
}
```

### Volume Control

```cpp
void Volume_adjustment(uint8_t Volume) {
    if (Volume > Volume_MAX) Volume = Volume_MAX;
    audio.setVolume(Volume);
    
    Serial.printf("Volume set to: %d/%d\n", Volume, Volume_MAX);
}
```

## Audio Playback Functions

### Play Music from SD Card

```cpp
void Play_Music(const char* directory, const char* fileName) {
    char fullPath[256];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", directory, fileName);
    
    if (SD.exists(fullPath)) {
        audio.connecttoSD(fullPath);
        Serial.printf("Playing: %s\n", fullPath);
    } else {
        Serial.printf("File not found: %s\n", fullPath);
    }
}
```

### Playback Control

```cpp
// Pause current playback
void Music_pause() {
    audio.pauseResume();
    Serial.println("Music paused");
}

// Resume playback
void Music_resume() {
    audio.pauseResume();
    Serial.println("Music resumed");
}

// Stop playback
void Music_stop() {
    audio.stopSong();
    Serial.println("Music stopped");
}
```

### Audio Information

```cpp
// Get total duration in seconds
uint32_t Music_Duration() {
    return audio.getAudioFileDuration();
}

// Get current playback position in seconds
uint32_t Music_Elapsed() {
    return audio.getAudioCurrentTime();
}

// Get current audio energy level (0-100)
uint16_t Music_Energy() {
    // This would need to be implemented based on audio analysis
    // Return approximate energy based on volume and playback status
    return audio.isRunning() ? Volume * 4 : 0;
}
```

## Audio Processing Loop

### Main Audio Task

```cpp
void Audio_Loop() {
    // Process audio data
    audio.loop();
    
    // Handle audio events if needed
    if (audio.isRunning()) {
        // Audio is playing
        static unsigned long lastUpdate = 0;
        if (millis() - lastUpdate > 1000) {  // Update every second
            lastUpdate = millis();
            
            uint32_t elapsed = Music_Elapsed();
            uint32_t duration = Music_Duration();
            
            if (duration > 0) {
                Serial.printf("Playing: %02d:%02d / %02d:%02d\n",
                             elapsed / 60, elapsed % 60,
                             duration / 60, duration % 60);
            }
        }
    }
}
```

### Audio Callback Events

```cpp
// Optional audio event callbacks
void audio_info(const char *info) {
    Serial.printf("Audio info: %s\n", info);
}

void audio_id3data(const char *info) {
    Serial.printf("ID3 data: %s\n", info);
}

void audio_eof_mp3(const char *info) {
    Serial.printf("End of file: %s\n", info);
    // Handle end of playback
}

void audio_bitrate(const char *info) {
    Serial.printf("Bitrate: %s\n", info);
}

void audio_commercial(const char *info) {
    Serial.printf("Commercial: %s\n", info);
}

void audio_icydata(const char *info) {
    Serial.printf("ICY data: %s\n", info);
}

void audio_icyurl(const char *info) {
    Serial.printf("ICY URL: %s\n", info);
}

void audio_lasthost(const char *info) {
    Serial.printf("Last host: %s\n", info);
}
```

## Web Radio Streaming

### Internet Radio Playback

```cpp
void play_web_radio(const char* url) {
    if (WiFi.status() == WL_CONNECTED) {
        audio.connecttohost(url);
        Serial.printf("Connecting to: %s\n", url);
    } else {
        Serial.println("WiFi not connected - cannot play web radio");
    }
}

// Example stations
void play_example_stations() {
    // Play different web radio stations
    const char* stations[] = {
        "http://stream.live.vc.bbcmedia.co.uk/bbc_world_service",
        "http://mp3.ffh.de/radioffh/hqlivestream.mp3",
        "http://streams.calmradio.com/api/39/128/stream"
    };
    
    for (int i = 0; i < 3; i++) {
        Serial.printf("Playing station %d...\n", i + 1);
        play_web_radio(stations[i]);
        delay(30000);  // Play for 30 seconds
        audio.stopSong();
        delay(2000);
    }
}
```

## Advanced Audio Features

### Equalizer Control

```cpp
void set_equalizer(int8_t* bands) {
    // Set 10-band equalizer (-40 to +6 dB)
    for (int i = 0; i < 10; i++) {
        if (bands[i] >= -40 && bands[i] <= 6) {
            // audio.setEqualizer(i, bands[i]);  // If supported
        }
    }
}

// Example: Bass boost
void apply_bass_boost() {
    int8_t eq_bands[10] = {3, 2, 1, 0, 0, 0, 0, 0, 0, 0};
    set_equalizer(eq_bands);
}
```

### Audio Spectrum Analysis

```cpp
void get_audio_spectrum(float* spectrum, int bands) {
    // This would require additional FFT processing
    // For now, simulate with random values during playback
    if (audio.isRunning()) {
        for (int i = 0; i < bands; i++) {
            spectrum[i] = random(0, 100) / 100.0;
        }
    } else {
        memset(spectrum, 0, bands * sizeof(float));
    }
}
```

## File System Integration

### SD Card Audio Files

```cpp
void list_audio_files() {
    File root = SD.open("/music");
    if (!root) {
        Serial.println("Failed to open /music directory");
        return;
    }
    
    Serial.println("Audio files found:");
    while (true) {
        File entry = root.openNextFile();
        if (!entry) break;
        
        String fileName = entry.name();
        if (fileName.endsWith(".mp3") || fileName.endsWith(".wav") || 
            fileName.endsWith(".aac") || fileName.endsWith(".flac")) {
            
            Serial.printf("  📄 %s (%d bytes)\n", 
                         fileName.c_str(), entry.size());
        }
        entry.close();
    }
    root.close();
}
```

### Playlist Management

```cpp
struct Playlist {
    char files[50][100];  // Max 50 files, 100 chars each
    int count;
    int current;
};

Playlist playlist;

void create_playlist(const char* directory) {
    playlist.count = 0;
    playlist.current = 0;
    
    File dir = SD.open(directory);
    if (!dir) return;
    
    while (playlist.count < 50) {
        File entry = dir.openNextFile();
        if (!entry) break;
        
        String fileName = entry.name();
        if (fileName.endsWith(".mp3") || fileName.endsWith(".wav")) {
            snprintf(playlist.files[playlist.count], 100, "%s/%s", 
                    directory, fileName.c_str());
            playlist.count++;
        }
        entry.close();
    }
    dir.close();
    
    Serial.printf("Playlist created with %d files\n", playlist.count);
}

void play_next_in_playlist() {
    if (playlist.count == 0) return;
    
    playlist.current = (playlist.current + 1) % playlist.count;
    audio.connecttoSD(playlist.files[playlist.current]);
    
    Serial.printf("Playing: %s (%d/%d)\n", 
                 playlist.files[playlist.current], 
                 playlist.current + 1, playlist.count);
}

void play_previous_in_playlist() {
    if (playlist.count == 0) return;
    
    playlist.current = (playlist.current - 1 + playlist.count) % playlist.count;
    audio.connecttoSD(playlist.files[playlist.current]);
    
    Serial.printf("Playing: %s (%d/%d)\n", 
                 playlist.files[playlist.current], 
                 playlist.current + 1, playlist.count);
}
```

## Troubleshooting

### Common Issues

1. **No Audio Output**
   - Check I2S pin connections (47, 48, 38)
   - Verify PCM5101 power supply
   - Test with known good audio file
   - Check volume level (not muted)

2. **Distorted Audio**
   - Reduce volume level
   - Check for ground loops
   - Verify sample rate compatibility
   - Test different audio files

3. **Choppy Playback**
   - Increase audio task priority
   - Check SD card speed (Class 10 recommended)
   - Monitor free heap memory
   - Reduce other task loads

4. **Web Radio Issues**
   - Verify WiFi connection stability
   - Check internet bandwidth
   - Test with different radio stations
   - Monitor connection timeouts

### Debug Functions

```cpp
void audio_system_test() {
    Serial.println("=== Audio System Test ===");
    
    // Test volume levels
    for (int vol = 0; vol <= Volume_MAX; vol += 5) {
        Volume_adjustment(vol);
        delay(500);
    }
    Volume_adjustment(10);  // Reset to medium
    
    // Test audio file playback
    if (SD.exists("/test.mp3")) {
        Serial.println("Playing test file...");
        Play_Music("/", "test.mp3");
        delay(5000);
        Music_pause();
        delay(1000);
        Music_resume();
        delay(5000);
        audio.stopSong();
    }
    
    Serial.println("Audio test complete");
}
```

## Example Implementation

```cpp
void setup() {
    Serial.begin(115200);
    
    // Initialize SD card first
    SD_Init();
    
    // Initialize audio system
    Audio_Init();
    
    // Create playlist
    create_playlist("/music");
    
    // Start playing first song
    if (playlist.count > 0) {
        audio.connecttoSD(playlist.files[0]);
    }
}

void loop() {
    // Process audio
    Audio_Loop();
    
    // Check for end of song
    if (!audio.isRunning() && playlist.count > 0) {
        delay(1000);  // Brief pause
        play_next_in_playlist();
    }
    
    delay(10);
}
```

*Last Updated: October 2, 2025*