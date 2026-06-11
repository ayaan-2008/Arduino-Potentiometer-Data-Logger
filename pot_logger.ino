/*
 * ============================================================
 *   DYNAMIC MEMORY ALLOCATION ON ARDUINO
 *   Project: Potentiometer Data Logger
 * ============================================================
 *
 * WHAT THIS PROJECT DOES:
 *   1. Asks the user HOW MANY readings they want to record
 *   2. Dynamically allocates EXACTLY that much memory (int array)
 *   3. Reads from a potentiometer (connected to pin A0) one
 *      reading at a time, on user command
 *   4. Shows stats: min, max, average of recorded values
 *   5. Frees the memory when done — no leak!
 *
 * HARDWARE (set up in Wokwi):
 *   Potentiometer:
 *     Left  pin  → GND
 *     Middle pin → A0  (the signal / wiper)
 *     Right pin  → 5V
 *
 * HOW analogRead() WORKS:
 *   Arduino reads voltage on A0 and converts it to a number:
 *   0V → 0,   5V → 1023   (10-bit ADC, 2^10 = 1024 steps)
 *   Turn the knob left  → lower number
 *   Turn the knob right → higher number
 *
 * ============================================================
 */

// ---- PIN ----
const int POT_PIN = A0;   // Potentiometer wiper goes here

// ---- LIMITS ----
const int MAX_POINTS = 100;  // We support up to 100 data points

// ---- DYNAMIC ARRAY (pointer) ----
// This pointer will point to our dynamically allocated array.
// Right now it points to nothing (nullptr).
int* readings   = nullptr;
int  totalSlots = 0;   // how many slots were allocated
int  recorded   = 0;   // how many readings have been taken so far

// ============================================================
// setup() — runs once on power-on
// ============================================================
void setup() {
  Serial.begin(9600);
  delay(500);

  printBanner();
  askAndAllocate();
}

// ============================================================
// loop() — runs forever
// ============================================================
void loop() {
  printMenu();

  int choice = readIntFromSerial();

  switch (choice) {
    case 1: recordOneReading();  break;
    case 2: recordAllAuto();     break;
    case 3: viewReadings();      break;
    case 4: showStats();         break;
    case 5: freeAndReset();      break;
    default:
      Serial.println(F("⚠️  Invalid choice. Enter 1 to 5."));
  }

  Serial.println();
}

// ============================================================
// printBanner — welcome message
// ============================================================
void printBanner() {
  Serial.println(F("============================================"));
  Serial.println(F("  POTENTIOMETER LOGGER - Dynamic Memory     "));
  Serial.println(F("============================================"));
  Serial.println(F("Reads analog values from a potentiometer"));
  Serial.println(F("and stores them in a DYNAMICALLY allocated"));
  Serial.println(F("array — only using RAM for what you need!"));
  Serial.println();
  Serial.println(F("HARDWARE REMINDER:"));
  Serial.println(F("  Pot LEFT   → GND"));
  Serial.println(F("  Pot MIDDLE → A0"));
  Serial.println(F("  Pot RIGHT  → 5V"));
  Serial.println(F("============================================"));
  Serial.println();
}

// ============================================================
// askAndAllocate — core dynamic memory allocation step
// ============================================================
void askAndAllocate() {
  Serial.println(F("------ MEMORY SETUP ------"));
  Serial.print(F("Max supported data points: "));
  Serial.println(MAX_POINTS);
  Serial.println();
  Serial.print(F("How many readings do you want to record? (1–"));
  Serial.print(MAX_POINTS);
  Serial.print(F("): "));

  int count = readIntFromSerial();

  // Validate
  if (count < 1 || count > MAX_POINTS) {
    Serial.println(F("⚠️  Out of range! Defaulting to 10."));
    count = 10;
  }

  // -------------------------------------------------------
  //  🔑  THE KEY LINE: Dynamic Allocation
  //
  //  'new int[count]' asks Arduino's heap for memory big
  //  enough to hold 'count' integers, and gives us back
  //  a pointer to the first element.
  //
  //  Each int = 2 bytes on Arduino Uno.
  //  So 'new int[10]' allocates 20 bytes — only what we need.
  //  A static 'int arr[100]' would always waste 200 bytes.
  // -------------------------------------------------------
  readings   = new int[count];
  totalSlots = count;
  recorded   = 0;

  // Set all slots to -1 (means "not yet recorded")
  for (int i = 0; i < totalSlots; i++) {
    readings[i] = -1;
  }

  // Feedback
  Serial.println();
  Serial.println(F("✅  MEMORY ALLOCATED!"));
  Serial.print(F("   Slots allocated : "));
  Serial.println(totalSlots);
  Serial.print(F("   RAM used        : "));
  Serial.print(totalSlots * sizeof(int));
  Serial.println(F(" bytes"));
  Serial.print(F("   RAM saved       : "));
  Serial.print((MAX_POINTS - totalSlots) * sizeof(int));
  Serial.println(F(" bytes (skipped unused slots)"));
  Serial.println(F("--------------------------"));
  Serial.println();
  Serial.println(F("👉 Now turn the potentiometer knob and use"));
  Serial.println(F("   option 1 or 2 to record readings!"));
  Serial.println();
}

// ============================================================
// printMenu — show the options
// ============================================================
void printMenu() {
  Serial.println(F("========== MENU =========="));
  Serial.print(F("  Recorded: "));
  Serial.print(recorded);
  Serial.print(F(" / "));
  Serial.println(totalSlots);

  // Live potentiometer reading — so user can see the current value
  int live = analogRead(POT_PIN);
  Serial.print(F("  Pot now : "));
  Serial.print(live);
  Serial.print(F(" ("));
  Serial.print(map(live, 0, 1023, 0, 100));
  Serial.println(F("%)"));

  Serial.println(F("=========================="));
  Serial.println(F("  1. Record ONE reading (manual)"));
  Serial.println(F("  2. Auto-record ALL slots (with delay)"));
  Serial.println(F("  3. View all recorded readings"));
  Serial.println(F("  4. Show stats (min/max/avg)"));
  Serial.println(F("  5. Free memory & start over"));
  Serial.println(F("=========================="));
  Serial.print(F("Choice: "));
}

// ============================================================
// recordOneReading — reads pot RIGHT NOW and stores it
// ============================================================
void recordOneReading() {
  if (recorded >= totalSlots) {
    Serial.println(F("❌  All slots are full! View stats or reset."));
    return;
  }

  int val = analogRead(POT_PIN);   // Read 0–1023 from A0
  readings[recorded] = val;        // Store in dynamic array
  recorded++;

  Serial.print(F("📍 Slot #"));
  Serial.print(recorded);
  Serial.print(F(" recorded: "));
  Serial.print(val);
  Serial.print(F("  →  "));
  Serial.print(map(val, 0, 1023, 0, 100));
  Serial.println(F("%  (turn knob & record again for variety!)"));

  if (recorded == totalSlots) {
    Serial.println(F("🎉 All slots filled! Check stats with option 4."));
  }
}

// ============================================================
// recordAllAuto — fills all remaining slots automatically
// with a 1 second pause between each so you can turn the knob
// ============================================================
void recordAllAuto() {
  if (recorded >= totalSlots) {
    Serial.println(F("❌  Already full!"));
    return;
  }

  int remaining = totalSlots - recorded;
  Serial.print(F("⏱️  Auto-recording "));
  Serial.print(remaining);
  Serial.println(F(" readings (1 per second)..."));
  Serial.println(F("   👉 TURN THE KNOB between beeps for varied data!"));
  Serial.println();

  for (int i = 0; i < remaining; i++) {
    // Countdown
    Serial.print(F("  Reading in: 3..."));
    delay(1000);
    Serial.print(F("2..."));
    delay(1000);
    Serial.print(F("1... "));
    delay(1000);

    int val = analogRead(POT_PIN);
    readings[recorded] = val;
    recorded++;

    Serial.print(F("📍 #"));
    Serial.print(recorded);
    Serial.print(F(": "));
    Serial.print(val);
    Serial.print(F("  ("));
    Serial.print(map(val, 0, 1023, 0, 100));
    Serial.println(F("%)"));

    if (i < remaining - 1) {
      Serial.println(F("  ↪ Turn knob now for next reading..."));
    }
  }

  Serial.println();
  Serial.println(F("✅  All slots recorded! Use option 4 for stats."));
}

// ============================================================
// viewReadings — print every stored value with a bar graph
// ============================================================
void viewReadings() {
  Serial.println();
  Serial.println(F("--- RECORDED READINGS ---"));

  if (recorded == 0) {
    Serial.println(F("(Nothing recorded yet. Use option 1 or 2!)"));
    return;
  }

  for (int i = 0; i < totalSlots; i++) {
    Serial.print(F("  ["));
    if (i + 1 < 10) Serial.print(F("0"));
    Serial.print(i + 1);
    Serial.print(F("] "));

    if (readings[i] == -1) {
      Serial.println(F("--- not recorded yet ---"));
    } else {
      // Print the raw value
      Serial.print(readings[i]);
      Serial.print(F("\t"));

      // Print a simple bar proportional to the value
      int barLen = map(readings[i], 0, 1023, 0, 20);
      for (int b = 0; b < barLen; b++) Serial.print(F("█"));
      for (int b = barLen; b < 20; b++) Serial.print(F("░"));

      Serial.print(F("  "));
      Serial.print(map(readings[i], 0, 1023, 0, 100));
      Serial.println(F("%"));
    }
  }
  Serial.println(F("-------------------------"));
}

// ============================================================
// showStats — calculate min, max, average from dynamic array
// ============================================================
void showStats() {
  Serial.println();
  Serial.println(F("--- STATISTICS ---"));

  if (recorded == 0) {
    Serial.println(F("No data recorded yet!"));
    return;
  }

  long  sum = 0;
  int   minVal = 1024;
  int   maxVal = -1;
  int   minIdx = 0;
  int   maxIdx = 0;

  for (int i = 0; i < recorded; i++) {
    int v = readings[i];
    sum += v;
    if (v < minVal) { minVal = v; minIdx = i; }
    if (v > maxVal) { maxVal = v; maxIdx = i; }
  }

  float avg = (float)sum / recorded;

  Serial.print(F("  Data points : "));
  Serial.print(recorded);
  Serial.print(F(" / "));
  Serial.println(totalSlots);

  Serial.print(F("  Minimum     : "));
  Serial.print(minVal);
  Serial.print(F("  ("));
  Serial.print(map(minVal, 0, 1023, 0, 100));
  Serial.print(F("%)  at slot #"));
  Serial.println(minIdx + 1);

  Serial.print(F("  Maximum     : "));
  Serial.print(maxVal);
  Serial.print(F("  ("));
  Serial.print(map(maxVal, 0, 1023, 0, 100));
  Serial.print(F("%)  at slot #"));
  Serial.println(maxIdx + 1);

  Serial.print(F("  Average     : "));
  Serial.print(avg, 1);
  Serial.print(F("  ("));
  Serial.print((int)map((long)avg, 0, 1023, 0, 100));
  Serial.println(F("%)"));

  Serial.print(F("  Range       : "));
  Serial.println(maxVal - minVal);

  Serial.println(F("------------------"));
  Serial.println(F("(Values are 0–1023 where 0=GND, 1023=5V)"));
}

// ============================================================
// freeAndReset — 'delete[]' releases the dynamic memory
// ============================================================
void freeAndReset() {
  Serial.println();
  Serial.println(F("Type 'yes' to free memory and start fresh: "));

  String confirm = readStringFromSerial();
  confirm.trim();
  confirm.toLowerCase();

  if (confirm != "yes") {
    Serial.println(F("Cancelled."));
    return;
  }

  // -------------------------------------------------------
  //  🔑  THE OTHER KEY LINE: Free Dynamic Memory
  //
  //  'delete[]' is the OPPOSITE of 'new int[count]'
  //  It tells Arduino: "I'm done with this memory, reuse it."
  //  Without this, the RAM stays occupied forever — memory leak!
  // -------------------------------------------------------
  delete[] readings;
  readings   = nullptr;
  totalSlots = 0;
  recorded   = 0;

  Serial.println(F("🗑️  Memory freed! No leaks."));
  Serial.println();

  askAndAllocate();
}

// ============================================================
// HELPERS
// ============================================================
int readIntFromSerial() {
  while (Serial.available() == 0) delay(50);
  int val = Serial.parseInt();
  while (Serial.available()) Serial.read();
  Serial.println(val);
  return val;
}

String readStringFromSerial() {
  while (Serial.available() == 0) delay(50);
  String val = Serial.readStringUntil('\n');
  val.trim();
  Serial.println(val);
  return val;
}
