# ESP32-S3 Project Template Creator
# PowerShell script to create new ESP32 projects with proper configuration

param(
    [Parameter(Mandatory=$true)]
    [string]$ProjectName
)

$projectPath = "C:\Projects\ESP32BOX\$ProjectName"

# Create project directory
New-Item -ItemType Directory -Path $projectPath -Force

# Create VS Code configuration
$vscodeDir = "$projectPath\.vscode"
New-Item -ItemType Directory -Path $vscodeDir -Force

$arduinoConfig = @{
    board = "esp32:esp32:esp32s3"
    configuration = "JTAGAdapter=default,PSRAM=enabled,FlashMode=qio,FlashSize=16M,LoopCore=1,EventsCore=1,USBMode=hwcdc,CDCOnBoot=default,MSCOnBoot=default,DFUOnBoot=default,UploadMode=default,CPUFreq=240,UploadSpeed=921600,DebugLevel=none,EraseFlash=none"
    port = "COM4"
    sketch = "$ProjectName\$ProjectName.ino"
    output = "build"
} | ConvertTo-Json

$arduinoConfig | Out-File "$vscodeDir\arduino.json" -Encoding UTF8

# Create basic .ino file
$inoTemplate = @"
/*
 * ESP32-S3-Touch-LCD Project: $ProjectName
 * 
 * Auto-generated with ESP32-S3 configuration
 * Board: ESP32S3 Dev Module, 16MB Flash, PSRAM enabled
 */

#include <Wire.h>

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("=== ESP32-S3 Project: $ProjectName ===");
    Serial.printf("Chip: %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("Flash: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    Serial.printf("Free Heap: %d KB\n", ESP.getFreeHeap() / 1024);
    
    // Your code here
}

void loop() {
    // Your main loop here
    delay(1000);
}
"@

$inoTemplate | Out-File "$projectPath\$ProjectName.ino" -Encoding UTF8

Write-Host "✅ Created ESP32-S3 project: $ProjectName"
Write-Host "📁 Location: $projectPath"
Write-Host "🔧 VS Code configuration: .vscode\arduino.json"
Write-Host "📝 Main file: $ProjectName.ino"