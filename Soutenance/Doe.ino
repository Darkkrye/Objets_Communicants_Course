#define ECHO_RIGHT 0
#define ECHO_LEFT 1
#define TRIGGER 12

#define MAX_DISTANCE 20
#define TIME_MEASURE 200000
#define TIME_BLINK 10000000


// Déclaration des pins utilisées pour le branchement de la matrice
const int rowPins[] = {2, 3, 4, 5, 6, 7, 8, 9};
const int columnPins[] = {10, 11, A4, 13, A0, A1, A2, A3};


// Déclaration du tableau définissant l'image
// Un bit à '0' correspont à une LED éteinte, un bit à '1' à une LED allumée
byte eye_forward[] = {B00111100, B01000010, B01011010, B10101101, B10111101, B10011001, B01000010, B00111100};
byte eye_right[] = {B00111100, B01000010, B01110010, B11011001, B11111001, B10110001, B01000010, B00111100};
byte eye_left[] = {B00111100, B01000010, B01001110, B10010111, B10011111, B10001101, B01000010, B00111100};
byte eye_blink[] = {B00000000, B00111100, B01111110, B11111111, B10111101, B11000011, B01111110, B00111100};

bool Blink = false;
long startTimerBlink = 0;

volatile unsigned long LastPulseTimeLeft;
volatile unsigned long LastPulseTimeRight;
volatile static unsigned long startTime = 0;

byte * Image; // image actuelle
void setup()
{
	Serial.begin(9600);
	pinMode(TRIGGER, OUTPUT);
	digitalWrite(TRIGGER, LOW);

	pinMode(ECHO_RIGHT, INPUT);
	pinMode(ECHO_LEFT, INPUT);

	// Utilisation d'une boucle for pour optimiser l'initialisation
	for (int i = 0; i < 8; i++)
	{
		pinMode(rowPins[i], OUTPUT);        // lignes en sorties
		pinMode(columnPins[i], OUTPUT);     // colonnes en sorties

		/* Optionnel - permet d'éviter un "scintillement" au démarrage du programme
	      digitalWrite(columnPins[i], HIGH);  // déconnexion des cathodes de la masse
	    */
	}

	attachInterrupt(digitalPinToInterrupt(ECHO_LEFT), InterruptLeft, CHANGE);
	attachInterrupt(digitalPinToInterrupt(ECHO_RIGHT), InterruptRight, CHANGE);

	startTimerBlink = micros();
	Image = eye_forward;
}

// Appelé à chaque changement de frond
void InterruptLeft() {
	// Si la pin est en état haut
	if (digitalRead(ECHO_LEFT) == HIGH)
	{
		// Trigger lancé -> Start Time initialisé
		startTime = micros();
	}
	else
	{
		// Frond descendant -> Signal reçu / On calcul la distance
		// La distance est stockée pour être utilisée dans switchOnTrigger lors de son appel
		LastPulseTimeLeft = (micros() - startTime) / 148;
	}
}

// Appelé à chaque changement de frond
void InterruptRight() {
	// Si la pn est en état haut
	if (digitalRead(ECHO_RIGHT) == HIGH)
	{
		// Trigger lancé -> Start Time initialisé
		startTime = micros();
	}
	else
	{
		// Frond descendant -> Signal reçu / On calcul la distance
		// La distance est stockée pour être utilisée dans switchOnTrigger lors de son appel
		LastPulseTimeRight = (micros() - startTime) / 148;
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
			digitalWrite(rowPins[row], HIGH);
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
					digitalWrite(columnPins[situ], LOW);
				}
				// On attend un peu entre chaque LED (délai total ~20ms pour 64 LEDs)
				delayMicroseconds(300);
				// Et on déconnecte la colonne de la masse...
				digitalWrite(columnPins[situ], HIGH);
			}
			// ...puis la ligne de l'alimentation
			digitalWrite(rowPins[row], LOW);
		}
	}
}



void switchOnTrigger() {
	if (micros() - startTime >= TIME_MEASURE) {
		// Condition sur la distance la plus courte droite ou gauche avec un objet.
		if (LastPulseTimeLeft > MAX_DISTANCE && LastPulseTimeRight > MAX_DISTANCE)
		{
			Image = eye_forward;
		}
		else if (LastPulseTimeLeft > LastPulseTimeRight)
		{
			Image = eye_right;
		}
		else if (LastPulseTimeLeft < LastPulseTimeRight)
		{
			Image = eye_left;
		}

		// Emet l'ultrason
		digitalWrite(TRIGGER, HIGH);
		delayMicroseconds(10);
		digitalWrite(TRIGGER, LOW);

		// Reset
		LastPulseTimeLeft = LastPulseTimeRight = 0;
	}
}



void loop() {
	// Condition pour cligner des yeux
	if (startTimerBlink + TIME_BLINK < micros())
	{
		Image = eye_blink;
		startTimerBlink = micros();
		Blink = true;
	}

	// Afficher l'image en cours
	aff(Image, 200);

	// Si on a cligné des yeux -> On les rouvre devant
	if (Blink)
	{
		Image = eye_forward;
		Blink = false;
	}

	// On appelle pour savoir quelle est l'image à afficher
	switchOnTrigger();
}
