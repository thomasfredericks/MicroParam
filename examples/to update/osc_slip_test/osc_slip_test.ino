
#include <Arduino.h>
#include <MicroParam.h>

MicroParamInt amp_attack(2, 0, 128);
MicroParamInt amp_decay(400, 1, 2000);
MicroParamInt filter_attack(0, 0, 128);
MicroParamInt filter_decay(256, 1, 2000);
MicroParamInt filter_mod(32, 0, 256);
MicroParamInt filter_cutoff(64, 0, 128);
MicroParamInt filter_resonance(128, 0, 256);


MicroParamBind bindings[] = {
    {"/vca/atk", &amp_attack},
    {"/vca/dcy", &amp_decay},
    {"/vcf/atk", &filter_attack},
    {"/vcf/dcy", &filter_decay},
    {"/vcf/mod", &filter_mod},
    {"/vcf/cut", &filter_cutoff},
    {"/vcf/res", &filter_resonance}
};
MicroParamBinder binder(bindings, sizeof(bindings) / sizeof(MicroParamBind));

#include <MicroOscSlip.h>
// The number 128 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscSlip<128> myOsc(&Serial);

unsigned long chrono;

// FUNCTION THAT WILL BE CALLED WHEN AN OSC MESSAGE IS RECEIVED:
void myOscMessageParser(MicroOsc & source, MicroOscMessage & receivedOscMessage)
{
    // DO MESSAGE ADDRESS CHECKING AND ARGUMENT GETTING HERE

     MicroParam *param = binder.getPointer(receivedOscMessage.getOscAddress());

  if (param)
  {
    if (receivedOscMessage.getTypeTag() == 'i')
    {
      param->setInt(receivedOscMessage.nextAsInt());
    }
    else if (receivedOscMessage.getTypeTag() == 'f')
    {
      param->setFloat(receivedOscMessage.nextAsFloat());
    }
    return;
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

        size_t count = binder.getCount();

        for (int32_t i = 0; i < count; ++i)
        {
            MicroParam *param = binder.getPointer(i);
            if (param->getType() == 'i')
            {
                myOsc.sendInt(binder.getKey(i), param->getInt());
            }
            else if (param->getType() == 'f')
            {
                myOsc.sendFloat(binder.getKey(i), param->getFloat());
            }
        }

    }
}
