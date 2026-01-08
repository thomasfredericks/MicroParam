
#include <Arduino.h>
#include <MicroSetting.h>

MicroSettingInt amp_env_attack("/attack", 0, 128, 2);
MicroSettingInt amp_env_decay("/decay", 1, 2000, 400);
MICRO_SETTING_GROUP(amp_env, "/env", &amp_env_attack, &amp_env_decay);

MICRO_SETTING_GROUP(amp, "/amp", &amp_env);

MicroSettingInt filter_env_attack("/attack", 0, 128, 4);
MicroSettingInt filter_env_decay("/decay", 1, 2000, 256);
MicroSettingInt filter_env_amount("/amount", 0, 256, 32);
MICRO_SETTING_GROUP(filterEnv, "/env", &filter_env_amount, &filter_env_attack, &filter_env_decay);

MicroSettingInt filter_cutoff("/cutoff", 0, 128, 64);
MicroSettingInt filter_resonance("/resonance", 0, 256, 128);
MICRO_SETTING_GROUP(filter, "/filter", &filter_cutoff, &filter_resonance, &filterEnv);

MICRO_SETTING_GROUP(settings, "/", &amp, &filter);

#include <MicroOscSlip.h>
// The number 128 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscSlip<128> myOsc(&Serial);

unsigned long chrono;

// FUNCTION THAT WILL BE CALLED WHEN AN OSC MESSAGE IS RECEIVED:
void myOscMessageParser( MicroOscMessage& receivedOscMessage) {
   // DO MESSAGE ADDRESS CHECKING AND ARGUMENT GETTING HERE
   MicroSetting * match = matchPath( &settings, receivedOscMessage.getAddress(), '/' );
   if ( match ) {
    match->setInt(receivedOscMessage.nextAsInt());
   }
}

void setup() {
  Serial.begin(115200);

  chrono = millis();

}

// Recursive function using a fixed-size buffer for full OSC path
void sendOscRecursive(MicroSetting* setting, char* pathBuffer, size_t pathLen = 0) {
    if (!setting) return;

    const char* name = setting->getName();
    size_t nameLen = strlen(name);

    // Check buffer overflow
    if (pathLen + nameLen >= 256) return; // adjust 256 to your max path length

    // Append this setting's name to the path buffer
    memcpy(pathBuffer + pathLen, name, nameLen);
    size_t newLen = pathLen + nameLen;
    pathBuffer[newLen] = '\0';

    if (setting->getChildCount() == 0) {
        // Leaf: send value over OSC
        switch (setting->getType() ) {
            case 'i':
                myOsc.sendInt(pathBuffer, setting->getInt());
                break;
            case 'f':
                myOsc.sendFloat(pathBuffer, setting->getFloat());
                break;
            case 'e':
                myOsc.sendInt(pathBuffer, setting->getInt()); // enums as int
                break;
        }
    } else {
        // Group: recurse into children
        for (int i = 0; i < setting->getChildCount(); i++) {
            // Add '/' separator if needed (avoid double slashes)
            size_t sepLen = 0;
            if (setting->getChild(i)->getName()[0] != '/') {
                if (newLen < 255) {
                    pathBuffer[newLen] = '/';
                    newLen++;
                    pathBuffer[newLen] = '\0';
                    sepLen = 1;
                }
            }
            sendOscRecursive(setting->getChild(i), pathBuffer, newLen);
            newLen -= sepLen; // remove separator after recursion
        }
    }
}


void loop() {

      myOsc.onOscMessageReceived( myOscMessageParser );

      if ( millis() - chrono >= 1000 ) {
        chrono = millis();
        char oscPath[256];  // max path length
        oscPath[0] = '\0';  // start empty

        sendOscRecursive(&settings, oscPath);

        //myOsc.sendInt( filter_env_attack.getName(), filter_env_attack.getInt() );
      }

}

