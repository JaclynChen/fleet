/*
  Fleet


*/

// mode switch button
int mode_button = 7;
int mode = 1;
int modeButtonState = 0;
int modeLastButtonState = 0;

bool justChanged = true;


// arcade button
int arcade_button = 6;
int arcade_timer = 0;
int arcadeButtonState = 0;
int arcadeLastButtonState = 0;
unsigned long lastTimestamp = 0;

int motorPin = 3;

// variable resistor 
int varResistor = A2;
bool changingColor = false;
int saved_r = 255;
int saved_g = 0;
int saved_b = 0;

// detect ambient light photoresistor
int ambient_photoresistor = A0;

// main rgb
int main_r = 11;
int main_g = 13;
int main_b = 12;

// sand photoresistor
int sand_photoresistor = A1;
bool sleepTime = false;
bool sleepTimeEnd = false;
// sand rgb
int sand_r = 8;
int sand_g = 10;
int sand_b = 9;


void setup() {
  pinMode(mode_button, INPUT);
  pinMode(arcade_button, INPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(varResistor, INPUT);
  pinMode(ambient_photoresistor, INPUT);
  pinMode(main_r, OUTPUT);
  pinMode(main_g, OUTPUT);
  pinMode(main_b, OUTPUT);
  pinMode(sand_photoresistor, INPUT);
  pinMode(sand_r, OUTPUT);
  pinMode(sand_g, OUTPUT);
  pinMode(sand_b, OUTPUT);

  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {

  checkButtonState();
  if (mode == 1) {
    modeOne();
  } else if (mode == 2) {
    modeTwo();
  } else if (mode == 3) {
    modeThree();
  } else {
     modeFour();
  }
}

// inverse ambient light
void modeOne() {
  // make sure motor is off
  if (justChanged) {
    digitalWrite(motorPin, LOW);
    justChangedSequence(0, 255, 0);
    justChanged = false;
  }
 
  int photoVal = analogRead(ambient_photoresistor); // read in photoresistor val
  //Serial.print("on inverse light mode: ");
  //Serial.println(photoVal);
  // crossfade on blue light, inverse
  int ledVal = map(photoVal, 0, 750, 255, 0);
  setBoth(0, 0, ledVal);
}

// voltage divider
void modeTwo() {
  if (justChanged) {
    justChangedSequence(0,0,255);
    justChanged = false;
  }

  arcadeButtonState = digitalRead(arcade_button);
  if (arcadeButtonState != arcadeLastButtonState) {
    if (arcadeButtonState == HIGH) {
      Serial.println("arcade pressed");
      changingColor = !changingColor;
      if (!changingColor) {
        Serial.println("color saved!");
      }

    }
     delay(5);
  }
  modeLastButtonState = modeButtonState;

  if (changingColor) {
    int val = analogRead(varResistor);
    // warmer tones
    // 0 - 200
    if (val >= 0 && val < 200) {
      setBoth(255, 0, 0);
      saved_r = 255;
      saved_g = 0; 
      saved_b = 0;
    } else if (val >= 200 && val < 400) {
      setBoth(255, 153, 53);
      saved_r = 255;
      saved_g = 153; 
      saved_b = 53;
    } else if (val >= 400 && val < 600) {
      setBoth(204, 244, 153);
      saved_r = 204;
      saved_g = 244; 
      saved_b = 153;
    } else if (val >= 600 && val <800) {
      setBoth(51, 51, 255);
      saved_r = 51;
      saved_g = 51; 
      saved_b = 255;
    } else if (val >= 800) {
      setBoth(153, 153, 255);
      saved_r = 153;
      saved_g = 153; 
      saved_b = 255;
    }
  } else {
    setBoth(saved_r, saved_g, saved_b);
  }
}

// sand timer
void modeThree() {
  if (justChanged) {
    sleepTime = false;
    sleepTimeEnd = false;
    justChangedSequence(128, 0, 128);

    setMainColor(255, 255, 255);
    setSandColor(255, 255, 255);
    justChanged = false;
  }

  int photoVal = analogRead(sand_photoresistor); // read in photoresistor val
  Serial.print("sand photoresistor is: ");
  Serial.println(photoVal);
  if (photoVal < 30) {
    sleepTime = true;
    setSandColor(0, 0, 0);
  }

  if (sleepTime && !sleepTimeEnd) {
    // fade out
    for (int i = 255; i >= 0; i--) {
      setMainColor(i, i, i);
      delay(100);
    }
    sleepTimeEnd = true;
   
  }
}


// worm turbulence
void modeFour() {
  if (justChanged) {
     justChangedSequence(255, 0, 0);
     justChanged = false;
  }
 
  unsigned long currTimestamp = millis();
  arcadeButtonState = digitalRead(arcade_button);
  if (arcadeButtonState != arcadeLastButtonState) {
    if (arcadeButtonState == HIGH) {
      Serial.println("button pressed");
      if (currTimestamp - lastTimestamp <= 4000) {
        arcade_timer += 1;
      } else {
        arcade_timer = 1;
        lastTimestamp = currTimestamp;
      }
      if (arcade_timer == 4) {
        Serial.println("sand activated");
        activateWorm();
      }
    }
    delay(5);
  }
  arcadeLastButtonState = arcadeButtonState;


  setMainColor(0, 0, 0);
  setSandColor(0, 0, 0);

}

void activateWorm() {
  digitalWrite(motorPin, HIGH);
  delay(2000);
  digitalWrite(motorPin, LOW);
  delay(1000);

}

void checkButtonState() {
  modeButtonState = digitalRead(mode_button);
  if (modeButtonState != modeLastButtonState) {
    if (modeButtonState == HIGH) {
      mode++;
      justChanged = true;
      if (mode == 5) {
        mode = 1;
      }
       Serial.print("mode is: ");
       Serial.println(mode);
    }
    delay(5);
  }
  modeLastButtonState = modeButtonState;
}




// helper to control main rgb led (dune one)
void setMainColor(int redValue, int greenValue,  int blueValue) {
  analogWrite(main_r, redValue);
  analogWrite(main_g,  greenValue);
  analogWrite(main_b, blueValue);
}

void setSandColor(int redValue, int greenValue,  int blueValue) {
  analogWrite(sand_r, redValue);
  analogWrite(sand_g,  greenValue);
  analogWrite(sand_b, blueValue);
}

void setBoth(int redValue, int greenValue, int blueValue) {
  setMainColor(redValue, greenValue, blueValue);
  setSandColor(redValue, greenValue, blueValue);
}

void justChangedSequence(int redValue, int greenValue, int blueValue) {
    setBoth(redValue, greenValue, blueValue);
    delay(500);
    setBoth(0, 0, 0);
    setBoth(redValue, greenValue, blueValue);
    delay(500);
    setBoth(0, 0, 0);

}




