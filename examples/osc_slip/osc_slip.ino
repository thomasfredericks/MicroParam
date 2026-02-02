
#include <Arduino.h>
#include <MicroParam.h>
#include <MicroParamOsc.h>

#include <MicroOscSlip.h>
// The number 128 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscSlip<128> myOsc(&Serial);

unsigned long chrono;

MicroParamInt input_a(2, 0, 127);
MicroParamInt input_b(400, 1, 2000);


MicroParamBind inputs[] = {
    {"/input/a", input_a},
    {"/input/b", input_b}
};
size_t inputsCount = sizeof(inputs) / sizeof(MicroParamBind);

MicroParamInt output_value(127, 0, 255);

MicroParamBind outputs[] = {
    {"/output/value", output_value},
};
size_t outputsCount = sizeof(outputs) / sizeof(MicroParamBind);


// FUNCTION THAT WILL BE CALLED WHEN AN OSC MESSAGE IS RECEIVED:
void myOscMessageParser(MicroOsc & source, MicroOscMessage & receivedOscMessage)
{
    if ( microParamOscDispatch(receivedOscMessage, inputs, inputsCount) )   
    {
        // successfully dispatched so we return
        return;
    }

    // handle other messages here:
    if ( receivedOscMessage.checkOscAddress("/?") )
    {
        // echo inputs
        microParamOscSend(myOsc, inputs, inputsCount);
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

        output_value = random(0, 256);

        microParamOscSend(myOsc, outputs, outputsCount);

        

    }
}
