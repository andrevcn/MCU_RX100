/* Inclusoes de arquivos de cabecalho ----------------------------------------*/
#include "Button.h"

Button_t ButtonEnter;

void Button_Init(Button_t *Button, gpio_port_pin_t pin)
{
	Button->pin = pin;
	Button->debounce = 20;
	Button->DCgap = 250;
	Button->holdTime = 1000;
	Button->longHoldTime = 3000;
	Button->buttonVal = HIGH;
	Button->buttonLast = HIGH;
	Button->DCwaiting = 0;
	Button->DConUp = 0;
	Button->singleOK = 1;
	Button.downTime = -1;
	Button.upTime = -1;
	Button.ignoreUp = 0;
	Button.waitForUp = 0;
	Button.holdEventPast = 0;
	Button.longHoldEventPast = 0;
}

void Button_MainTask(Button_t *Button)
{
	buttonVal = R_GPIO_PinRead(Button->pin);
	
	// Button pressed down
   if (Button->buttonVal == LOW && Button->buttonLast == HIGH && (millis() - Button->upTime) > Button->debounce)
   {
       Button->downTime = millis();
       Button->ignoreUp = 0;
       Button->waitForUp = 0;
       Button->singleOK = 1;
       Button->holdEventPast = 0;
       Button->longHoldEventPast = 0;

       if ((millis()-Button->upTime) < Button->DCgap && Button->DConUp == 0 && Button->DCwaiting == 1)
		   Button->DConUp = 1;
       else
		   Button->DConUp = 0;

	   Button->DCwaiting = 0;
   }
   // Button released
   else if (Button->buttonVal == HIGH && Button->buttonLast == LOW && (millis() - Button->downTime) > Button->debounce)
   {
       if (!Button->ignoreUp)
       {
           Button->upTime = millis();

           if (Button->DConUp == 0)
			   Button->DCwaiting = 1;
           else
           {
               Button->event = 2;
               Button->DConUp = 0;
               Button->DCwaiting = 0;
               Button->singleOK = 0;
           }
       }
   }

   // Test for normal click event: DCgap expired
   if ( Button->buttonVal == HIGH && (millis()-Button->upTime) >= Button->DCgap && Button->DCwaiting == 1 && Button->DConUp == 0 && Button->singleOK == 1 && Button->event != 2)
   {
       Button->event = 1;
       Button->DCwaiting = 0;
   }

   // Test for hold
   if (Button->buttonVal == LOW && (millis() - Button->downTime) >= Button->holdTime)
   {
       // Trigger "normal" hold
       if (!Button->holdEventPast)
       {
           Button->event = 3;
           Button->waitForUp = 1;
           Button->ignoreUp = 1;
           Button->DConUp = 0;
           Button->DCwaiting = 0;
           Button->holdEventPast = 1;
       }

       // Trigger "long" hold
       if ((millis() - Button->downTime) >= Button->longHoldTime)
       {
           if (!Button->longHoldEventPast)
           {
               Button->event = 4;
               Button->longHoldEventPast = 1;
           }
       }
   }
   Button->buttonLast = Button->buttonVal;
}




/*****END OF FILE****/


