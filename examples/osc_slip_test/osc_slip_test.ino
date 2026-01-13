
#include <Arduino.h>
#include <MicroSetting.h>

MicroSettingGroup settings;
MicroSettingInt amp_attack(settings, "/atk", 0, 128, 2);
MicroSettingInt amp_decay(settings, "/dcy", 1, 2000, 400);

MicroSettingInt filter_attack(settings, "/vcf/atk", 0, 128, 4);
MicroSettingInt filter_decay(settings, "/vcf/dcy", 1, 2000, 256);
MicroSettingInt filter_mod(settings, "/vcf/mod", 0, 256, 32);

MicroSettingInt filter_cutoff(settings, "/vcf/cut", 0, 128, 64);
MicroSettingInt filter_resonance(settings, "/vcf/res", 0, 256, 128);

#include <MicroOscSlip.h>
// The number 128 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscSlip<128> myOsc(&Serial);

unsigned long chrono;

// FUNCTION THAT WILL BE CALLED WHEN AN OSC MESSAGE IS RECEIVED:
void myOscMessageParser(MicroOsc & source, MicroOscMessage & receivedOscMessage)
{
    // DO MESSAGE ADDRESS CHECKING AND ARGUMENT GETTING HERE

    size_t count = settings.getCount();

    for (int32_t i = 0; i < count; ++i)
    {
        MicroSetting *setting = settings.get(i);

        if (receivedOscMessage.checkOscAddress(setting->getName()))
        {
            if (receivedOscMessage.getTypeTag() == 'i')
            {
                setting->setInt(receivedOscMessage.nextAsInt());
            }
            else if (receivedOscMessage.getTypeTag() == 'f')
            {
                setting->setFloat(receivedOscMessage.nextAsFloat());
            }
            break;
        }
    }
}

void setup()
{
    Serial.begin(115200);

    chrono = millis();
}

void loop()
{

    myOsc.onOscMessageReceived(myOscMessageParser);

    if (millis() - chrono >= 1000)
    {
        chrono = millis();

        size_t count = settings.getCount();

        for (int32_t i = 0; i < count; ++i)
        {
            MicroSetting *setting = settings.get(i);
            if (setting->getType() == 'i')
            {
                myOsc.sendInt(setting->getName(), setting->getInt());
            }
            else if (setting->getType() == 'f')
            {
                myOsc.sendFloat(setting->getName(), setting->getFloat());
            }
        }

    }
}
