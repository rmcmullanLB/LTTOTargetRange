// This project creates a target shooting game.  The target will show red for a random amount of time.
// Then it will change to green for a defined period of time, during which it can be shot to score.
// If it is shot during this time, it turns blue and scores a point.
// If it is not shot during this time, it returns to red without a point scored.
// The parameters of the game are set in the Game Parameters section below.

///---------------------------------------------------------------------------------------------------------
// LTTO set up
#define DEBUG
#define EI_ARDUINO_INTERRUPTED_PIN                      // This tells the ISR routine which was the last Pin that changed
#define IRTxPin 2                                       // Infrared LED Emitter Transmit Pin
#define IRRxPin 10                                      // Infrared Receiver Pin

// RGB LED and Speaker Pin Setup
// LED 1
#define redPin 3
#define bluePin 6
#define greenPin 5

#define SpeakerPin 9                                    // This must be on pin 9 to take full advantage of speaker capabilities

//---Color Code
//Pregame: Blue
//Target Waiting: Red
//Target Ready: Green
//Target Hit: Blue flashing
//Game Over: Yellow flashing

// Game Parameters
#define GameLength 5                                    // How many targets before game over?
#define MinWaitTime 3                                   // The minimum amount of time before the next target pops
#define MaxWaitTime 6                                   // The maximum amount of time before the next target pops
#define TargetTime 2.0                                  // How many seconds is the target available to shoot?
#define BeaconTime 0.25                                  // How many seconds delay between proximity beacons?

// Includes
#include <EnableInterrupt.h>                            //  This is required for the LTTO libary to work.  https://github.com/GreyGnome/EnableInterrupt
#include <LTTO.h>                                       //  This adds the LTTO library to the sketch
#include <millisDelay.h>                                //  This adds the millisDelay library to the sketch, used for simple timer code

// Initiate Variables
int PlayerScore = 0;                                    // How many targets has the player successfully hit?
long randomNumber;                                      // To randomize the time between rounds
int GameRound = 1;                                      // What is the current round in the game?
int GameState = 0;
//---Game States
// 0 = Pregame (waiting to start)
// 1 = Non-scoring Red Mode
// 2 = Scoring Green Mode
// 3 = Debrief Mode

LTTO ltto;                                              //  This creates an instance of the LTTO object, called ltto

millisDelay TargetDelay;                                //  Timer for the target pop-up
millisDelay BeaconDelay;                                //  Timer for the beacon ping



////---------------------------------------------------------------------------------------------------------

// Human audible hearing is typically 20 to 20,000 Hz
// My piezo speaker (Mouser# 665-AT1224TWT5V2R) resonant frequency (loudest) is 1900 to 2900 Hz

void whoopup(int maxReps, int riseSpeed, int startHz, int endHz) {  // make a whoop up sound of variable repetitions and rising speed
  for (int rep = 1; rep < (maxReps + 1); rep++) {                   // repeat it maxReps number of times
    for (int hz = startHz; hz < endHz; hz = hz + riseSpeed) {       // start at startHz and rise to endHz (try 440 and 1000 to start)
      tone(SpeakerPin, hz, 50);                                     // play tone
      delay(5);                                                     // delay to next tone in rise
    }
    delay(10);                                                      // delay between whoops
  }
  noTone(SpeakerPin);                                               // shut the speaker down for good measure
}

void whoopdown(int maxReps, int fallSpeed, int startHz, int endHz) {  // make a whoop down sound of variable repetitions and falling speed
  for (int rep = 1; rep < (maxReps + 1); rep++) {                     // repeat it maxReps number of times
    for (int hz = startHz; hz > endHz; hz = hz - fallSpeed) {         // start at startHz and fall to endHz (try 1000 and 440 to start)
      tone(SpeakerPin, hz, 50);                                       // play tone
      delay(5);                                                       // delay to next tone in fall
    }
    delay(10);                                                        // delay between whoops
  }
  noTone(SpeakerPin);                                                 // shut the speaker down for good measure
}


//-- START SETUP --------------------------------------------------
void setup()
{
  Serial.begin(115200);                               //  Starts the Serial port for debugging
  Serial.println(F("\nHere we go boys....."));        //  A short message so that we know something is happenning
  ltto.begin(IRTxPin, IRRxPin);                       //  Start the ltto IR comms , using [Pin 13 to Tx] and [Pin 11 to Rx] (see above for actual).
  pinMode(redPin, OUTPUT);                            // Set I/O pins to correct mode
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  whoopup(1, 5, 440, 1047);                           // startup tone
  delay(200);
  tone(SpeakerPin, 494, 500);

  digitalWrite(bluePin, HIGH);                        // Set startup colors for LED 1
  digitalWrite(greenPin, LOW);
  digitalWrite(redPin, LOW);


  randomSeed(analogRead(A0));
  GameState = 0;
  Serial.println(F("\nStart up in pre-game mode"));   //  A short message so that we know something is happenning


}

//-- START LOOP --------------------------------------------------

void loop() {

  //    ltto.clearIRmessage();                        // Flush any stored IR messages

  // What happens when a shot comes in
  if (ltto.available() )                              // available() checks if there is a message to receive. (1 = true, 0 = false)
  {
    if (ltto.readMessageType() == TAG )               // readMessageType() returns the type of Message (TAG, BEACON, PACKET, DATA, CHECKSUM)
    {
      // First report who shot it.
/*      Serial.print(F("\nTag - "));                    // Give us a read out of who hit it
      Serial.print("Team #: ");
      Serial.print(ltto.readTeamID() );               // readTeamID() returns the teamID (0 = None, 1 = team1, 2 = team2, 3 = team3)
      Serial.print(F("\tPlayer #: "));
      Serial.print(ltto.readPlayerID() );             // readPlayerID() returns the playerID (Player # 1 through 8)
      Serial.print(F("\tShotStrength: "));
      Serial.print(ltto.readShotStrength() );         // readShotStrength() returns the shotStrength (value 1 through 4, where 1 is a normal tag - non mega)
*/
      // If it's a TAG then react below based on the current state of the game.
      switch (GameState) {                            // The effect of an incoming tag depends on the current Game State, combined with the timer justFinished section below
        case 0:                                       // If in pre-game mode, start the game
          whoopdown(1, 5, 1047, 440);                 // startup tone
          digitalWrite(bluePin, LOW);                 // Turn RGB LED Off
          digitalWrite(greenPin, LOW);
          digitalWrite(redPin, LOW);
          Serial.print("\nStarting pre-game countdown");
          TargetDelay.start(MaxWaitTime * 1000);
          break;

        case 1:                                       // If in non-scoring mode, ignore the TAG
          break;

        case 2:                                       // If in scoring mode, score a point
          ltto.sendBeacon(1, 0, 1);                   // Send "was hit" beacon back to the shooter
          TargetDelay.start(MaxWaitTime * 1000);
          BeaconDelay.stop();
          PlayerScore++;                              // Score a point
          Serial.print("\nScored a Point!  ");
          Serial.print("The score is now ");
          Serial.print(PlayerScore);

          tone(SpeakerPin, 1900, 200);
          delay(300);
          tone(SpeakerPin, 1900, 400);
          digitalWrite(bluePin, LOW);                     // Turn RGB LED Off
          digitalWrite(greenPin, LOW);
          digitalWrite(redPin, LOW);
          for (int Flashes = 1; Flashes < 5; Flashes++) { // Flash the LED as Blue on and off
            digitalWrite(bluePin, HIGH);                  // Turn RGB LED to Blue for a half second
            delay(500);
            digitalWrite(bluePin, LOW);                   // Turn RGB LED off for a half second
            delay(500);
          }
          ltto.clearIRmessage();                          // Flush any stored IR messages
          TargetDelay.finish();
          break;

          /*          case 3:                                       // If in debrief mode, start a new game
                      digitalWrite(bluePin, LOW);                     // Turn RGB LED Off
                      digitalWrite(greenPin, LOW);
                      digitalWrite(redPin, LOW);
                      TargetDelay.start(MaxWaitTime*1000);
                    break;
          */
      }
    }
  }


  // Display patterns
  switch (GameState) {                            // The Pre-game and Debrief modes are handled here
    case 0:                                       // If in pre-game mode, wait until a tag starts the game
      break;

    case 1:                                       // If in non-scoring mode, keep going
      break;

    case 2:                                       // If in scoring mode, keep going
      break;

    case 3:                                                             // If in debrief mode, display the score
      for (int Displays = 1; Displays < 5; Displays++) {                // Display the score four times as a series of flashing lights
        delay(2000);
        for (int Flashes = 1; Flashes < (PlayerScore + 1); Flashes++) { // Flash once for each point scored
          tone(SpeakerPin, 31, 190);
          digitalWrite(redPin, HIGH);                                   // Turn RGB LED to Red + Green = yellow for a fifth of a second
          digitalWrite(greenPin, HIGH);
          delay(200);
          digitalWrite(redPin, LOW);                                    // Turn RGB LED off for a fifth of a second
          digitalWrite(greenPin, LOW);
          delay(200);
        }
      }
      Serial.print("\nResetting the game");
      ltto.clearIRmessage();                                  // Flush any stored IR messages so nothing is stored for the next game
      GameState = 0;
      GameRound = 1;                                          // Reset Game Round back to 1
      PlayerScore = 0;                                        // Reset the Player Score back to zero
      whoopup(1, 5, 440, 1047);                               // startup tone
      delay(100);
      tone(SpeakerPin, 523, 500);
      digitalWrite(bluePin, HIGH);                            // Set startup color Blue
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, LOW);
      break;
  }

  // Ping a normal beacon
  if (BeaconDelay.justFinished())
  {
          ltto.sendBeacon(0, 0, 1);                   // Send basic beacon to the shooter
          BeaconDelay.start(BeaconTime*1000);
  }

  // What happens when the timer runs out
  if (TargetDelay.justFinished())
  {
    switch (GameState)
    {
      case 0:                                                   // Finished the Pregame Game State countdown, move to first round red non-scoring mode
        ltto.clearIRmessage();                                  // Flush any stored IR messages
        GameState = 1;                                          // Set Game State to red non-scoring mode
        tone(SpeakerPin, 440, 1000);                            // play tone
        digitalWrite(bluePin, LOW);                             // Set colors to Red for Game State 1
        digitalWrite(greenPin, LOW);
        digitalWrite(redPin, HIGH);

        Serial.print("\nRound # ");
        Serial.print(GameRound);

        randomNumber = random(MinWaitTime * 1000, MaxWaitTime * 1000);  // pick a random amount of time between the min and max defined at the top of the code
        Serial.print("\nRed light for ");
        Serial.print(randomNumber);
        Serial.print(" milliseconds");
        TargetDelay.start(randomNumber);                                // start the countdown for the random amount of time
        break;

      case 1:                                                           // Finished the Red Non-Scoring Game State, move to Green Scoring Game State after a random interval
        GameState = 2;
        ltto.clearIRmessage();                                          // Flush any stored IR messages
        digitalWrite(bluePin, LOW);                                     // Set colors to Green for Game State 2
        digitalWrite(greenPin, HIGH);
        digitalWrite(redPin, LOW);
        whoopup(1, 10, 440, 1047);                                      // target up tone
        ltto.sendBeacon(0, 0, 1);                   // Send basic beacon to the shooter
        BeaconDelay.start(BeaconTime*1000);
        TargetDelay.start(TargetTime * 1000);                           // start target pop-down timer
        Serial.print("\nGreen Light Available. ");
        Serial.print("Timer Started for ");
        Serial.print(TargetDelay.delay() / 1000);
        Serial.print(" seconds.");
        break;

      case 2:                                                   // Finished the Green Scoring Game State, either end the game or move to next round
        TargetDelay.stop();
        BeaconDelay.stop();
        Serial.print("\nEnd green scoring period.");
        GameRound++;                                            // Advance to the next round
        if (GameRound > GameLength) {                           // If you're out of rounds move to Debrief Mode
          GameState = 3;
          Serial.print("\nGame Over.  Final score is ");
          Serial.print(PlayerScore);
          digitalWrite(bluePin, LOW);                           // Turn RGB LED Off
          digitalWrite(greenPin, LOW);
          digitalWrite(redPin, LOW);
          whoopdown(1, 10, 1047, 440);                          // target down tone
        }
        else {
          GameState = 1;                                        // Otherwise move on to the next round
          Serial.print("\nRound # ");
          Serial.print(GameRound);
          digitalWrite(bluePin, LOW);                           // Set colors to Red for Game State 1
          digitalWrite(greenPin, LOW);
          digitalWrite(redPin, HIGH);
          whoopdown(1, 10, 1047, 440);                          // target down tone
          randomNumber = random(MinWaitTime * 1000, MaxWaitTime * 1000); // pick a random amount of time between the min and max defined at the top of the code
          Serial.print("\nRed light for ");
          Serial.print(randomNumber);
          Serial.print(" milliseconds");
          TargetDelay.start(randomNumber);                      // start the countdown for the random amount of time
        }
        break;
        /*
                  case 3:                                                   // Finished the Debrief Game State countdown, start a new game in the first round
                    Serial.print("\nStarting new game");
                    GameRound = 1;                                          // Set Game Round to 1
                    PlayerScore = 0;                                        // Reset the player score to zero
                    GameState = 1;                                          // Set Game State to red non-scoring mode
                    digitalWrite(bluePin, LOW);                             // Set colors to Red for Game State 1
                    digitalWrite(greenPin, LOW);
                    digitalWrite(redPin, HIGH);

                    Serial.print("\nRound # ");
                    Serial.print(GameRound);

                    randomNumber=random(MinWaitTime*1000,MaxWaitTime*1000); // pick a random amount of time between the min and max defined at the top of the code
                    Serial.print("\nRed light for ");
                    Serial.print(randomNumber);
                    Serial.print(" milliseconds");
                    TargetDelay.start(randomNumber);                        // start the countdown for the random amount of time
                  break;
        */

    }

  }

}
