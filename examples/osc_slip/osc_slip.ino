/*
| Command                                           | Description                                                                  | Example                                  |
| ------------------------------------------------- | ---------------------------------------------------------------------------- | ---------------------------------------- |
| `microParamOscDispatch(msg, inputs, inputsCount)` | Update local parameters from received OSC messages. Returns true if handled. | `/input/a 42` → sets `input_a = 42`      |
| `receivedOscMessage.checkOscAddress("/?")`        | "Query" for current parameter values.                                        | `/?"` → triggers sending all inputs back |
| `microParamOscSend(myOsc, inputs, inputsCount)`   | Send the current values of input parameters via OSC (when queried).          | `/input/a 2`, `/input/b 400`             |
| `microParamOscSend(myOsc, outputs, outputsCount)` | Send the current values of output parameters via OSC.                        | `/output/value 127`                      |
*/



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


MicroParamBindKey inputs[] = {
    {"/input/a", input_a},
    {"/input/b", input_b}
};
size_t inputsCount = sizeof(inputs) / sizeof(MicroParamBindKey);

MicroParamInt output_value(127, 0, 255);

MicroParamBindKey outputs[] = {
    {"/output/value", output_value},
};
size_t outputsCount = sizeof(outputs) / sizeof(MicroParamBindKey);


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
