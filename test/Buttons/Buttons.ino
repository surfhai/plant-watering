int btn_up = 4;
int btn_down = 5;
int btn_sel = 6;

void setup() {
    pinMode(btn_up, INPUT);
    pinMode(btn_down, INPUT);
    pinMode(btn_sel, INPUT);
    Serial.begin(9600);
}

void loop() {
    if (digitalRead(btn_up) == LOW) {
        Serial.println("Button Up: pressed");
    } else {
        Serial.println("Button Up: not press");
    }
    
    if (digitalRead(btn_down) == LOW) {
        Serial.println("Button Down: pressed");
    } else {
        Serial.println("Button Down: not press");
    }

    if (digitalRead(btn_sel) == LOW) {
        Serial.println("Button Sel: pressed");
    } else {
        Serial.println("Button Sel: not press");
    }
    delay(2000);
}
