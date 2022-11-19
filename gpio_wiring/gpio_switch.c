#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h> 

#define LED18 18 /* GPIO18 */
#define LED23 23 /* GPIO23 */
#define SW25  25 /* GPIO25 */

int main(void) {

	int val = 0, last_val = 0;

	/* wiringPi 초기화 */
	if(wiringPiSetupGpio() == -1)
  		return 1;

	pinMode(LED18, OUTPUT);
    pinMode(LED23, OUTPUT);
	pinMode(SW25, INPUT);

    for(val = 0; val < 5; val++) {
   	    digitalWrite(LED18,HIGH);
		digitalWrite(LED23,HIGH);
  	    delay(500);
		digitalWrite(LED18,LOW);
		digitalWrite(LED23,LOW);
        delay(500);
	}

	while(1) {
		val = digitalRead(SW25);
		
		if(val != last_val) {
			if(val == HIGH) {
				digitalWrite(LED18,LOW);
				digitalWrite(LED23,LOW);
			}else {
				digitalWrite(LED18,HIGH);
				digitalWrite(LED23,HIGH);
			}
	    }

		last_val = val;
	}
	
	return 1;
}
