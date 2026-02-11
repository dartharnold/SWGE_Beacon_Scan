// Platform: ESP32-S3 + Arduino + NimBLE-Arduino
#include <NimBLEDevice.h>

// === CONFIG ===
static NimBLEAddress targetAddr("AA:BB:CC:DD:EE:FF"); // <- set your target
static const float ALPHA = 0.20f;     // EMA smoothing factor (0.1–0.3 typical)
static const int   MAX_STEP = 12;     // optional jump clamp in dB
static const bool  ACTIVE_SCAN = true;// active scan gets quicker/more frequent updates

// === STATE ===
static float rssiFiltered = NAN;

static int clampStep(int raw, float prev) {
  if (isnan(prev)) return raw;
  int diff = (int)(raw - prev);
  if (diff >  MAX_STEP) return (int)prev + MAX_STEP;
  if (diff < -MAX_STEP) return (int)prev - MAX_STEP;
  return raw;
}

class AdvCallbacks : public NimBLEAdvertisedDeviceCallbacks {
  void onResult(NimBLEAdvertisedDevice* adv) override {
    // Match by address (preferred). You can also do name, manufacturer data, UUIDs, etc.
    if (adv->getAddress().equals(targetAddr)) {
      int rssiRaw = adv->getRSSI();
      // Optional: clamp sudden unrealistic jumps
      rssiRaw = clampStep(rssiRaw, rssiFiltered);

      if (isnan(rssiFiltered)) rssiFiltered = rssiRaw; // initialize
      rssiFiltered = ALPHA * rssiRaw + (1.0f - ALPHA) * rssiFiltered;

      Serial.printf("BLE ADV  addr=%s  raw=%d dBm  filtered=%.1f dBm\n",
                    adv->getAddress().toString().c_str(), rssiRaw, rssiFiltered);
    }
  }
};

void setup() {
  Serial.begin(115200);
  NimBLEDevice::init("ESP32S3");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9); // TX power for scan requests (optional)
  NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DEVICE); // reduce duplicates

  auto* scan = NimBLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new AdvCallbacks(), /*wantDuplicates=*/true);
  scan->setActiveScan(ACTIVE_SCAN);
  scan->setInterval(45); // in units of 0.625 ms -> ~28 ms
  scan->setWindow(40);   // must be <= interval -> ~25 ms (duty ~89%)
  scan->start(/*duration=*/0, /*cb=*/nullptr, /*is_continue=*/true); // continuous
}

void loop() {
  // Nothing required; callback handles updates
}