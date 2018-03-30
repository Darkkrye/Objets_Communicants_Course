#define ECHO_RIGHT 0
#define ECHO_LEFT 1
#define TRIGGER 12

#define MAX_DISTANCE 20
#define TIME_MEASURE 200000
#define TIME_BLINK 10000000

#define FORWARD_STATE 0
#define RIGHT_STATE 1
#define LEFT_STATE 2

// Déclaration des pins utilisées pour le branchement de la matrice
const int ROW_PINS[] = {2, 3, 4, 5, 6, 7, 8, 9};
const int COLUMN_PINS[] = {10, 11, A4, 13, A0, A1, A2, A3};


// Déclaration du tableau définissant l'image
// Un bit à '0' correspont à une LED éteinte, un bit à '1' à une LED allumée
byte eyeForward[] = {B00111100, B01000010, B01011010, B10101101, B10111101, B10011001, B01000010, B00111100};
byte eyeRight[] = {B00111100, B01000010, B01110010, B11011001, B11111001, B10110001, B01000010, B00111100};
byte eyeLeft[] = {B00111100, B01000010, B01001110, B10010111, B10011111, B10001101, B01000010, B00111100};
byte eyeBlink[] = {B00000000, B00111100, B01111110, B11111111, B10111101, B11000011, B01111110, B00111100};

bool blinked = false;

long startTimerBlink = 0;

volatile unsigned long lastPulseTimeLeft;
volatile unsigned long lastPulseTimeRight;
volatile static unsigned long startTime = 0;

byte * currentImage;
int imageState = FORWARD_STATE;

void setup()
{

  pinMode(TRIGGER, OUTPUT);
  digitalWrite(TRIGGER, LOW);

  pinMode(ECHO_RIGHT, INPUT);
  pinMode(ECHO_LEFT, INPUT);


  for (int i = 0; i < 8; i++)
  {
    pinMode(ROW_PINS[i], OUTPUT);
    pinMode(COLUMN_PINS[i], OUTPUT);
  }

  attachInterrupt(digitalPinToInterrupt(ECHO_LEFT), InterruptLeft, CHANGE  );
  attachInterrupt(digitalPinToInterrupt(ECHO_RIGHT), InterruptRight, CHANGE  );

  startTimerBlink = micros();
  currentImage = eyeForward;
  imageState = 0;
}


void InterruptLeft() {
  if ( digitalRead(ECHO_LEFT) == HIGH)
  {
    startTime = micros();
  }
  else
  {
    lastPulseTimeLeft = (micros() - startTime) / 148;
  }
}

void InterruptRight() {
  if ( digitalRead(ECHO_RIGHT) == HIGH)
  {
    startTime = micros();
  }
  else
  {
    lastPulseTimeRight = (micros() - startTime) / 148;
  }
}


// Sous-routine pour la commande des LEDs à partir des données du tableau
// Elle s'affiche pendant la durée donnée en second paramètre
void aff(byte * image, unsigned long duree)
{
  // On note le moment où l'on commence
  unsigned long debut = millis();
  // On joue la boucle tant qu'on n'a pas atteint la durée souhaitée
  while (debut + duree > millis())
  {
    // Première itération (boucle for) sur les lignes
    for (int row = 0; row < 8; row++)
    {
      // Connexion de la ligne au +5V
      digitalWrite(ROW_PINS[row], HIGH);
      // Seconde itération sur les colonnes
      for (int col = 0; col < 8; col++)
      {
        // Miroir sur le poids du bit pour avoir le bon numéro de colonne
        int situ = 7 - col;
        // Lecture du bit concerné
        boolean pixel = bitRead(image[row], col);
        // S'il est à un...
        if (pixel == 1)
        {
          // ...on connecte la colonne concernée à la masse pour allumer la LED
          digitalWrite(COLUMN_PINS[situ], LOW);
        }
        // On attend un peu entre chaque LED (délai total ~20ms pour 64 LEDs)
        delayMicroseconds(300);
        // Et on déconnecte la colonne de la masse...
        digitalWrite(COLUMN_PINS[situ], HIGH);
      }
      // ...puis la ligne de l'alimentation
      digitalWrite(ROW_PINS[row], LOW);
    }
  }
}



void SetImage() {
  if (  micros() - startTime >= TIME_MEASURE) {
    if (lastPulseTimeLeft > MAX_DISTANCE && lastPulseTimeRight > MAX_DISTANCE)
    {
      imageState = FORWARD_STATE;
      currentImage = eyeForward;
    }
    else if (lastPulseTimeLeft > lastPulseTimeRight)
    {
      currentImage = eyeRight;
      if (imageState != RIGHT_STATE) {
        currentImage = eyeForward;
        imageState = RIGHT_STATE;
      }
    }
    else if (lastPulseTimeLeft < lastPulseTimeRight)
    {
      currentImage = eyeLeft;
      if (imageState != LEFT_STATE)
      {
        currentImage = eyeForward;
        imageState = LEFT_STATE;
      }
    }

    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER, LOW);

    lastPulseTimeLeft = lastPulseTimeRight = 0;
  }
}

void loop() {
  if (startTimerBlink + TIME_BLINK < micros())
  {
    currentImage = eyeBlink;
    startTimerBlink = micros();
    blinked = true;
  }

  aff(currentImage, 200);

  if (blinked)
  {
    currentImage = eyeForward;
    blinked = false;
  }

  SetImage();

}
